#pragma once

#include <list>
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

enum ViewState {
    Tempo,
    ClockDiv,
    NUM_VIEW_STATES
};

class StateObserver {
  public:
    virtual void stateChanged() = 0;
};

class State {

  public:

      State();

      void registerObserver(StateObserver* /*observer*/);

      ViewState viewState();
      void setViewState(ViewState /*viewState_*/);

      float tempo();
      void setTempo(float /*tempo_*/);

      ClockDivMode clockDivMode();
      float clockDivValue();
      void setClockDivMode(ClockDivMode /*clockDivMode_*/);

      ableton::Link link;
      std::atomic<bool> running;
      std::atomic<PlayState> playState;
      std::atomic<int> quantum;

  private:

      std::atomic<ViewState> m_viewState;
      std::atomic<double> m_tempo;
      std::atomic<ClockDivMode> m_clockDivMode;

      std::list<StateObserver*> observers;
      void stateChanged();

};
