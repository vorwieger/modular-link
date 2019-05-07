#include "State.h"

State::State()
  : link(m_tempo)
  , running(true)
  , m_viewState(Tempo)
  , playState(Stopped)
  , m_clockDivMode(Quarter)
  , quantum(4)
{
  link.enable(true);
  link.setTempoCallback([&](double bpm) {
    m_tempo.store(bpm);
    stateChanged();
  });
}

// -------------------------------------------------------------------------------------------

void State::registerObserver(StateObserver* observer) {
  observers.push_back(observer);
  observer->stateChanged();
}

void State::stateChanged() {
  for (StateObserver* observer : observers) {
    observer->stateChanged();
  }
}

// -------------------------------------------------------------------------------------------

ViewState State::viewState() {
  return m_viewState;
}

void State::setViewState(ViewState viewState_) {
  m_viewState.store(viewState_);
  stateChanged();
}

float State::tempo() {
  return m_tempo;
}

void State::setTempo(float tempo_) {
  const auto time = link.clock().micros();
  auto sessionState = link.captureAppSessionState();
  sessionState.setTempo(tempo_, time);
  link.commitAudioSessionState(sessionState);
}

ClockDivMode State::clockDivMode() {
  return m_clockDivMode;
}

float State::clockDivValue() {
  switch (m_clockDivMode) {
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

void State::setClockDivMode(ClockDivMode clockDivMode_) {
  m_clockDivMode.store(clockDivMode_);
  stateChanged();
}

// -------------------------------------------------------------------------------------------
