#include "Engine.h"

#include <algorithm>

const int MAX_QUANTUM = 16; // Up to 16 beats per loop

// -------------------------------------------------------------------------------------------

Engine::Engine()
  : m_settings(Settings::Load())
  , m_running(true)
  , m_viewState(TEMPO)
  , m_playState(STOPPED)
  , link(m_settings.load().tempo)
{
  link.enable(true);
  link.enableStartStopSync(m_settings.load().start_stop_sync);
  link.setTempoCallback([&](double bpm) {
    stateChanged();
  });

  link.setStartStopCallback([this](const bool isPlaying) {
    if (isPlaying) {
      auto sessionState = link.captureAppSessionState();
      sessionState.requestBeatAtStartPlayingTime(0, quantum());
      link.commitAppSessionState(sessionState);
      setPlayState(CUED);
    } else {
      setPlayState(STOPPED);
    }
  });

}

// -------------------------------------------------------------------------------------------

bool Engine::running() {
  return m_running;
}

void Engine::registerObserver(StateObserver* observer) {
  observers.push_back(observer);
  observer->stateChanged();
}

void Engine::stateChanged() {
  for (StateObserver* observer : observers) {
    observer->stateChanged();
  }
}

// -------------------------------------------------------------------------------------------

LinkState Engine::getLinkState() {
  const auto time = link.clock().micros();
  auto sessionState = link.captureAppSessionState();
  auto settings = m_settings.load();
  const double beat = sessionState.beatAtTime(time, settings.quantum);
  const double phase = sessionState.phaseAtTime(time, settings.quantum);
  const double tempo = sessionState.tempo();
  return LinkState { beat, phase, tempo };
}

// -------------------------------------------------------------------------------------------

ViewState Engine::viewState() {
  return m_viewState;
}

void Engine::setViewState(ViewState viewState_) {
  std::lock_guard<std::mutex> lock(m_updateViewState);
  if (m_viewState == viewState_) { return; }
  m_viewState.store(viewState_);
  stateChanged();
}

// -------------------------------------------------------------------------------------------

PlayState Engine::playState() {
  return m_playState;
}

void Engine::setPlayState(PlayState playState_) {
  std::lock_guard<std::mutex> lock(m_updatePlayState);
  if (m_playState == playState_) { return; }
  m_playState.store(playState_);
}

// -------------------------------------------------------------------------------------------

int Engine::quantum() {
  return m_settings.load().quantum;
}

void Engine::setQuantum(int quantum) {
  quantum = std::max(1, std::min(quantum, MAX_QUANTUM));
  Settings settings = m_settings.load();
  if (settings.quantum != quantum) {
    settings.quantum = quantum;
    m_settings = settings;
    stateChanged();
    Settings::Save(settings);    
  }
}

// -------------------------------------------------------------------------------------------

int Engine::ppqn() {
  return m_settings.load().getPPQN();
}

void Engine::changePpqn(bool increase) {
  Settings settings = m_settings.load();
  int max_index = Settings::ppqn_options.size() - 1;
  int ppqn_index = settings.ppqn_index + (increase ? 1 : -1);
  ppqn_index = std::max(0, std::min(ppqn_index, max_index));
  if (settings.ppqn_index != ppqn_index) {
    settings.ppqn_index = ppqn_index;
    m_settings = settings;
    stateChanged();
    Settings::Save(settings);
  }
}

// -------------------------------------------------------------------------------------------

float Engine::tempo() {
  return link.captureAppSessionState().tempo();
}

void Engine::setTempo(float tempo) {
  const auto time = link.clock().micros();
  auto sessionState = link.captureAppSessionState();
  sessionState.setTempo(tempo, time);
  link.commitAudioSessionState(sessionState);
  Settings settings = m_settings.load();
  if (settings.tempo != tempo) {
    settings.tempo = tempo;
    m_settings = settings;
    Settings::Save(settings);
  }
}

// -------------------------------------------------------------------------------------------

void Engine::startTimeline() {
  auto sessionState = link.captureAppSessionState();
  auto now = link.clock().micros();
  sessionState.setIsPlayingAndRequestBeatAtTime(true, now, 0, quantum());
  link.commitAppSessionState(sessionState);
}

void Engine::stopTimeline() {
  auto sessionState = link.captureAppSessionState();
  auto now = link.clock().micros();
  sessionState.setIsPlayingAndRequestBeatAtTime(false, now, 0, quantum());
  link.commitAppSessionState(sessionState);
}
