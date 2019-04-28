#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <ableton/Link.hpp>

extern "C" {
    #include <wiringPi.h>
}
#include "Max7219.h"

using namespace std;

namespace {

  const double PULSES_PER_BEAT = 4.0;
  const double PULSE_LENGTH = 0.02; // seconds
  const double QUANTUM = 4;

  float clock_div = 1.0; // clock division for supporting volca/eurorack/etc, multiply by PULSES_PER_BEAT

  // for first command line argument to set clock division
  enum ClockDivModes {
      Sixteenth = 0,
      Eighth,
      Quarter,
      NUM_CLOCK_DIVS
  };

  int selectedClockDiv = Sixteenth;

  // Using WiringPi numbering scheme
  enum InPin {
      PlayStop = 21,
      ClockDiv = 22
  };

  enum OutPin {
      Clock = 1,
      Reset = 23,
      PlayIndicator = 11
  };

  enum PlayState {
      Stopped,
      Cued,
      Playing
  };

  struct State {
      ableton::Link link;
      std::atomic<bool> running;
      std::atomic<bool> playPressed;
      std::atomic<PlayState> playState;
      std::atomic<bool> clockDivPressed;

      State()
        : link(120.0)
        , running(true)
        , playPressed(false)
        , playState(Stopped)
        , clockDivPressed(false)
      {
        link.enable(true);
      }
  };

  void configurePins() {
      wiringPiSetup();
      pinMode(PlayStop, INPUT);
      pullUpDnControl(PlayStop, PUD_DOWN);
      pinMode(ClockDiv, INPUT);
      pullUpDnControl(ClockDiv, PUD_DOWN);
      pinMode(Clock, OUTPUT);
      pinMode(Reset, OUTPUT);
      pinMode(PlayIndicator, OUTPUT);
  }

  void clearLine() {
      std::cout << "   \r" << std::flush;
      std::cout.fill(' ');
  }

  void printState(State& state)
  {
      const auto time = state.link.clock().micros();
      const auto sessionState = state.link.captureAppSessionState();
      const auto beats = sessionState.beatAtTime(time, QUANTUM);
      const auto phase = sessionState.phaseAtTime(time, QUANTUM);
      std::cout << "tempo: " << sessionState.tempo()
          << " | " << std::fixed << "beats: " << beats
          << " | " << std::fixed << "phase: " << phase;
      clearLine();
  }

  void outputClock(double beats, double phase, double tempo) {
      const double secondsPerBeat = 60.0 / tempo;

      // Fractional portion of current beat value
      double intgarbage;
      const auto beatFraction = std::modf(beats * PULSES_PER_BEAT * clock_div, &intgarbage);

      // Fractional beat value for which clock should be high
      const auto highFraction = PULSE_LENGTH / secondsPerBeat;

      const bool resetHigh = (phase <= highFraction);
      digitalWrite(Reset, resetHigh ? LOW : HIGH);

      const bool clockHigh = (beatFraction <= highFraction);
      digitalWrite(Clock, clockHigh ? LOW : HIGH);
  }

  void input(State& state) {
      while (state.running) {

          const bool clockDivPressed = digitalRead(ClockDiv) == HIGH;
          const bool playPressed = digitalRead(PlayStop) == HIGH;
          if (playPressed && !state.playPressed) {
              switch (state.playState) {
                  case Stopped:
                      state.playState.store(Cued);
                      break;
                  case Cued:
                  case Playing:
                      state.playState.store(Stopped);
                      break;
              }
          }

          if (clockDivPressed && !state.clockDivPressed) {
              selectedClockDiv = (selectedClockDiv + 1) % NUM_CLOCK_DIVS;
              switch (selectedClockDiv) {
                  case Sixteenth:
                      clock_div = 1.0;
                      break;
                  case Eighth:
                      clock_div = 0.5;
                      break;
                  case Quarter:
                      clock_div = 0.25;
                      break;
                  default:
                      clock_div = 1.0;
                      break;
              }
          }
          state.playPressed.store(playPressed);
          state.clockDivPressed.store(clockDivPressed);
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
  }

  void output(State& state) {
      while (state.running) {
          const auto time = state.link.clock().micros();
          auto sessionState = state.link.captureAppSessionState();

          const double beats = sessionState.beatAtTime(time, QUANTUM);
          const double phase = sessionState.phaseAtTime(time, QUANTUM);
          const double tempo = sessionState.tempo();

          switch (state.playState) {
              case Cued: {
                      // Tweak this
                      const bool playHigh = (long)(beats * 2) % 2 == 0;
                      digitalWrite(PlayIndicator, playHigh ? HIGH : LOW);
                      if (phase <= 0.01) {
                          state.playState.store(Playing);
                      }
                  break;
              }
              case Playing:
                  digitalWrite(PlayIndicator, HIGH);
                  outputClock(beats, phase, tempo);
                  break;
              default:
                  digitalWrite(PlayIndicator, LOW);
                  break;
          }

          std::this_thread::sleep_for(std::chrono::microseconds(250));
      }
  }
}

int main(void) {
    configurePins();
    Max7219 max7219;
    State state;

    std::thread inputThread(input, std::ref(state));
    std::thread outputThread(output, std::ref(state));

    state.link.setTempoCallback([&](double bpm) {
      max7219.display(bpm);
    });

    max7219.display(10000.0);

    while (state.running) {
        //printState(state);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    inputThread.join();
    outputThread.join();

    return 0;
}

