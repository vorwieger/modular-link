#include "State.h"

#include <algorithm>

const int MAX_LOOP = 16; // Up to 16 beats per loop
const int MAX_PULSE = 8;  // 1, 2, 4, 8 pulses per beat

// -------------------------------------------------------------------------------------------

State::State()
  : link(120.0)
  , m_running(true)
  , m_viewState(TEMPO)
  , m_playState(STOPPED)
  , m_pulse(1)
  , m_loop(4)
{
  link.enable(true);
  link.enableStartStopSync(true);
  link.setTempoCallback([&](double bpm) {
    stateChanged();
  });

  link.setStartStopCallback([this](const bool isPlaying) {
    if (isPlaying) {
      auto sessionState = link.captureAppSessionState();
      sessionState.requestBeatAtStartPlayingTime(0, m_loop);
      link.commitAppSessionState(sessionState);
      setPlayState(CUED);
    } else {
      setPlayState(STOPPED);
    }
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
  std::lock_guard<std::mutex> lock(m_updateViewState);
  if (m_viewState == viewState_) { return; }
  m_viewState.store(viewState_);
  stateChanged();
}

// -------------------------------------------------------------------------------------------

PlayState State::playState() {
  return m_playState;
}

void State::setPlayState(PlayState playState_) {
  std::lock_guard<std::mutex> lock(m_updatePlayState);
  if (m_playState == playState_) { return; }
  m_playState.store(playState_);
  //stateChanged();
}

// -------------------------------------------------------------------------------------------

int State::loop() {
  return m_loop;
}

void State::setLoop(int loop_) {
  std::lock_guard<std::mutex> lock(m_updateLoop);
  loop_ = std::max(1, std::min(loop_, MAX_LOOP));
  if (m_loop == loop_) { return; }
  m_loop = loop_;
  stateChanged();
}

// -------------------------------------------------------------------------------------------

int State::pulse() {
  return m_pulse;
}

void State::setPulse(int pulse_) {
  std::lock_guard<std::mutex> lock(m_updatePulse);
  pulse_ = std::max(1, std::min(pulse_, MAX_PULSE));
  if (m_pulse == pulse_) { return; }
  m_pulse = pulse_;
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
  const double beats = sessionState.beatAtTime(time, loop());
  const double phase = sessionState.phaseAtTime(time, loop());
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

void State::startTimeline() {
  auto sessionState = link.captureAppSessionState();
  auto now = link.clock().micros();
  sessionState.setIsPlayingAndRequestBeatAtTime(true, now, 0, m_loop);
  link.commitAppSessionState(sessionState);
}

void State::stopTimeline() {
  auto sessionState = link.captureAppSessionState();
  auto now = link.clock().micros();
  sessionState.setIsPlayingAndRequestBeatAtTime(false, now, 0, m_loop);
  link.commitAppSessionState(sessionState);
}
