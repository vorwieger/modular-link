#include "Output.h"
#include <wiringPi.h>

const double PULSE_LENGTH = 0.02; // seconds
const auto OUTPUT_THREAD_SLEEP = std::chrono::microseconds(250);

// -------------------------------------------------------------------------------------------

Output::Output(State& state_)
  : m_state(state_)
  , m_thread(&Output::process, this) {

  pinMode(Clock, OUTPUT);
  pinMode(Reset, OUTPUT);
  pinMode(PlayIndicator, OUTPUT);

  digitalWrite(Clock, HIGH); // inverted -> Clock/LED off
  digitalWrite(Reset, HIGH); // inverted -> Reset/LED off
  digitalWrite(PlayIndicator, LOW); // inverted -> Reset/LED off
}

Output::~Output() {
    if (m_thread.joinable()) {
      m_thread.join();
    }
}

// -------------------------------------------------------------------------------------------

void Output::setPlayIndicator(bool high) {
  if (m_playHigh == high) { return; }
  m_playHigh = high;
  digitalWrite(PlayIndicator, high ? HIGH : LOW);
}

void Output::setClock(bool high) {
  if (m_clockHigh == high) { return; }
  m_clockHigh = high;
  digitalWrite(Clock, high ? LOW : HIGH);  // inverted
}

void Output::setReset(bool high) {
  if (m_resetHigh == high) { return; }
  m_resetHigh = high;
  digitalWrite(Reset, high ? LOW : HIGH); // inverted
}

void Output::outputClock(double beats, double phase, double tempo, int pulse) {
  const double secondsPerBeat = 60.0 / tempo;

  // Fractional portion of current beat value
  double intgarbage;
  const auto beatFraction = std::modf(beats * pulse, &intgarbage);

  // Fractional beat value for which clock should be high
  const auto highFraction = PULSE_LENGTH / secondsPerBeat;

  setReset(phase <= highFraction);
  setClock(beatFraction <= highFraction);
}

// -------------------------------------------------------------------------------------------

void Output::process() {
  while (m_state.running()) {
    LinkState linkState = m_state.getLinkState();
    switch (m_state.playState()) {
      case STOPPED: {
        setPlayIndicator(false);
        setClock(false);
        setReset(false);
        break;
      }
      case CUED: {
        setPlayIndicator((long)(linkState.beats * 2) % 2 == 0);
        setClock(false);
        setReset(false);
        if (linkState.phase <= 0.01) {
            m_state.setPlayState(PLAYING);
        }
        break;
      }
      case PLAYING: {
        setPlayIndicator(true);
        outputClock(linkState.beats, linkState.phase, linkState.tempo, m_state.pulse());
        break;
      }
    }
    std::this_thread::sleep_for(OUTPUT_THREAD_SLEEP);
  }
}
