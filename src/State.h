#pragma once

#include <ableton/Link.hpp>

enum PlayState {
      Stopped,
      Cued,
      Playing
};

enum ClockDivMode {
      Sixteenth = 0,
      Eighth,
      Quarter,
      NUM_CLOCK_DIVS
  };

struct State {
    ableton::Link link;
    std::atomic<bool> running;
    std::atomic<PlayState> playState;
    std::atomic<ClockDivMode> clockDivMode;

    State()
      : link(120.0)
      , running(true)
      , playState(Stopped)
      , clockDivMode(Quarter)
    {
      link.enable(true);
    }

    float clockDivValue() {
      switch (clockDivMode) {
        case Sixteenth:
          return 1.0;
        case Eighth:
          return 0.5;
        case Quarter:
          return 0.25;
        default:
          return 1.0;
      }
    }

};