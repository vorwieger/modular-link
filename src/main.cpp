#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <bitset>
#include <ableton/Link.hpp>

extern "C" {
    #include <wiringPi.h>
}

#include "State.h"
#include "Input.h"
#include "Max7219.h"

using namespace std;

namespace {

  const double PULSES_PER_BEAT = 4.0;
  const double PULSE_LENGTH = 0.02; // seconds
  const double QUANTUM = 4;

  enum OutPin {
      Clock = 1,
      Reset = 23,
      PlayIndicator = 11
  };

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

  void outputClock(double beats, double phase, double tempo, float clockDiv) {
      const double secondsPerBeat = 60.0 / tempo;

      // Fractional portion of current beat value
      double intgarbage;
      const auto beatFraction = std::modf(beats * PULSES_PER_BEAT * clockDiv, &intgarbage);

      // Fractional beat value for which clock should be high
      const auto highFraction = PULSE_LENGTH / secondsPerBeat;

      const bool resetHigh = (phase <= highFraction);
      digitalWrite(Reset, resetHigh ? LOW : HIGH);

      const bool clockHigh = (beatFraction <= highFraction);
      digitalWrite(Clock, clockHigh ? LOW : HIGH);
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
                  outputClock(beats, phase, tempo, state.clockDivValue());
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
    wiringPiSetup();
    piHiPri(99);
    pinMode(Clock, OUTPUT);
    pinMode(Reset, OUTPUT);
    pinMode(PlayIndicator, OUTPUT);

    Max7219 max7219;
    State state;

    Input input(state);
    std::thread outputThread(output, std::ref(state));

    state.link.setTempoCallback([&](double bpm) {
      max7219.display(bpm);
    });

    while (state.running) {
        //printState(state);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    outputThread.join();

    return 0;
}

