#pragma once

#include <list>
#include <mutex>
#include <ableton/Link.hpp>
#include "Settings.h"

enum PlayState {
    STOPPED,
    CUED,
    PLAYING
};

enum ViewState {
    TEMPO,
    PPQN,
    QUANTUM
};

class StateObserver {
  public:
    virtual void stateChanged() = 0;
};

struct LinkState {
  const double beat;
  const double phase;
  const double tempo;
};

class Engine {

  public:

      Engine();

      bool running();

      LinkState getLinkState();

      void registerObserver(StateObserver* /*observer*/);

      ViewState viewState();
      void setViewState(ViewState /*viewState_*/);

      PlayState playState();
      void setPlayState(PlayState /*playState_*/);

      float tempo();
      void setTempo(float /*tempo_*/);

      int quantum();
      void setQuantum(int /*quantum_*/);

      int ppqn();
      void changePpqn(bool /*increase*/);

      void startTimeline();
      void stopTimeline();


  private:

      std::atomic<Settings> m_settings;
      std::atomic<bool> m_running;
      std::atomic<ViewState> m_viewState;
      std::atomic<PlayState> m_playState;
      
      ableton::Link link;

      std::list<StateObserver*> observers;
      void stateChanged();

      std::mutex m_updateViewState;
      std::mutex m_updatePlayState;

};
