#pragma once

#include <list>
#include <ableton/Link.hpp>

enum PlayState {
    Stopped,
    Cued,
    Playing
};

enum ViewState {
    Tempo,
    PulsesPerBeat,
    NUM_VIEW_STATES
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

      int quantum();
      void setQuantum(int /*quantum_*/);

      int pulsesPerBeat();
      void setPulsesPerBeat(int /*pulsesPerBeat_*/);

      bool running();


  private:

      ableton::Link link;

      std::atomic<ViewState> m_viewState;
      std::atomic<PlayState> m_playState;
      std::atomic<int> m_quantum;
      std::atomic<int> m_pulsesPerBeat;
      std::atomic<bool> m_running;

      std::list<StateObserver*> observers;
      void stateChanged();

};
