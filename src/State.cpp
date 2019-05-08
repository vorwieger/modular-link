#include "State.h"

#include <algorithm>

State::State()
  : link(120.0)
  , m_running(true)
  , m_viewState(Tempo)
  , m_playState(Stopped)
  , m_pulsesPerBeat(1)
  , m_quantum(4)
{
  link.enable(true);
  link.setTempoCallback([&](double bpm) {
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
  if (m_viewState == viewState_) { return; }
  m_viewState.store(viewState_);
  stateChanged();
}

// -------------------------------------------------------------------------------------------

PlayState State::playState() {
  return m_playState;
}

void State::setPlayState(PlayState playState_) {
  if (m_playState == playState_) { return; }
  m_playState.store(playState_);
  stateChanged();
}

// -------------------------------------------------------------------------------------------

int State::quantum() {
  return m_quantum;
}

void State::setQuantum(int quantum_) {
  if (m_quantum == quantum_) { return; }
  m_quantum = quantum_;
  stateChanged();
}

// -------------------------------------------------------------------------------------------

int State::pulsesPerBeat() {
  return m_pulsesPerBeat;
}

void State::setPulsesPerBeat(int pulsesPerBeat_) {
  pulsesPerBeat_ = std::max(1, std::min(pulsesPerBeat_, 4)); // zwischen 1 und 4
  if (m_pulsesPerBeat == pulsesPerBeat_) { return; }
  m_pulsesPerBeat = pulsesPerBeat_;
  stateChanged();
}

// -------------------------------------------------------------------------------------------

bool State::running() {
  return m_running;
}

// -------------------------------------------------------------------------------------------

float State::tempo() {
  return link.captureAppSessionState().tempo();
}

LinkState State::getLinkState() {
  const auto time = link.clock().micros();
  auto sessionState = link.captureAppSessionState();

  const double beats = sessionState.beatAtTime(time, quantum());
  const double phase = sessionState.phaseAtTime(time, quantum());
  const double tempo = sessionState.tempo();

  return LinkState { beats, phase, tempo };
}

void State::setTempo(float tempo_) {
  const auto time = link.clock().micros();
  auto sessionState = link.captureAppSessionState();
  sessionState.setTempo(tempo_, time);
  link.commitAudioSessionState(sessionState);
}

// -------------------------------------------------------------------------------------------
