#pragma once

#include <list>
#include <mutex>
#include <ableton/Link.hpp>

enum PlayState {
    STOPPED,
    CUED,
    PLAYING
};

enum ViewState {
    TEMPO,
    PULSE,
    LOOP
};

class StateObserver {
  public:
    virtual void stateChanged() = 0;
};

struct LinkState {
  const double beats;
  const double phase;
  const double tempo;
};

class State {

  public:

      State();
      LinkState getLinkState();

      void registerObserver(StateObserver* /*observer*/);

      ViewState viewState();
      void setViewState(ViewState /*viewState_*/);

      PlayState playState();
      void setPlayState(PlayState /*playState_*/);

      float tempo();
      void setTempo(float /*tempo_*/);

      int loop();
      void setLoop(int /*loop_*/);

      int pulse();
      void setPulse(int /*pulse_*/);

      bool running();


  private:

      ableton::Link link;

      std::atomic<ViewState> m_viewState;
      std::atomic<PlayState> m_playState;
      std::atomic<int> m_loop;
      std::atomic<int> m_pulse;
      std::atomic<bool> m_running;

      std::list<StateObserver*> observers;
      void stateChanged();

      std::mutex m_updateViewState;
      std::mutex m_updatePlayState;
      std::mutex m_updateLoop;
      std::mutex m_updatePulse;

};
