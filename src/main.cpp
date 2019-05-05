#include <iostream>
#include <thread>
#include <ableton/Link.hpp>

extern "C" {
    #include <wiringPi.h>
}

#include "Max7219.h"
#include "State.h"
#include "Input.h"
#include "Output.h"

using namespace std;

namespace {

  void clearLine() {
      std::cout << "   \r" << std::flush;
      std::cout.fill(' ');
  }

  void printState(State& state)
  {
      const auto time = state.link.clock().micros();
      const auto sessionState = state.link.captureAppSessionState();
      const auto beats = sessionState.beatAtTime(time, state.quantum);
      const auto phase = sessionState.phaseAtTime(time, state.quantum);
      std::cout << "tempo: " << sessionState.tempo()
          << " | " << std::fixed << "beats: " << beats
          << " | " << std::fixed << "phase: " << phase;
      clearLine();
  }

}

int main(void) {
    wiringPiSetup();
    piHiPri(99);

    Max7219 max7219;
    State state;

    Input input(state);
    Output output(state);

    state.link.setTempoCallback([&](double bpm) {
      max7219.display(bpm);
    });

    while (state.running) {
        //printState(state);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

