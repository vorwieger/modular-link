#include "Output.h"
#include <wiringPi.h>

const double PULSES_PER_BEAT = 4.0;
const double PULSE_LENGTH = 0.02; // seconds

// -------------------------------------------------------------------------------------------

Output::Output(State& state_)
  : m_state(state_)
  , m_thread(&Output::process, this) {

  pinMode(Clock, OUTPUT);
    pinMode(Reset, OUTPUT);
    pinMode(PlayIndicator, OUTPUT);
}

Output::~Output() {
    if (m_thread.joinable()) {
      m_thread.join();
    }
}

// -------------------------------------------------------------------------------------------

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

void Output::outputClock(double beats, double phase, double tempo, float clockDiv) {
  const double secondsPerBeat = 60.0 / tempo;

  // Fractional portion of current beat value
  double intgarbage;
  const auto beatFraction = std::modf(beats * PULSES_PER_BEAT * clockDiv, &intgarbage);

  // Fractional beat value for which clock should be high
  const auto highFraction = PULSE_LENGTH / secondsPerBeat;

  setReset(phase <= highFraction);
  setClock(beatFraction <= highFraction);
}

// -------------------------------------------------------------------------------------------

void Output::process() {
  while (m_state.running) {

    const auto time = m_state.link.clock().micros();
    auto sessionState = m_state.link.captureAppSessionState();

    const double beats = sessionState.beatAtTime(time, m_state.quantum);
    const double phase = sessionState.phaseAtTime(time, m_state.quantum);
    const double tempo = sessionState.tempo();

    switch (m_state.playState) {
      case Cued: {
        const bool playHigh = (long)(beats * 2) % 2 == 0;
        digitalWrite(PlayIndicator, playHigh ? HIGH : LOW);
        if (phase <= 0.01) {
            m_state.playState.store(Playing);
        }
        break;
      }
      case Playing:
          digitalWrite(PlayIndicator, HIGH);
          outputClock(beats, phase, tempo, m_state.clockDivValue());
          break;
      default:
          digitalWrite(PlayIndicator, LOW);
          break;
    }

    std::this_thread::sleep_for(std::chrono::microseconds(250));
  }
}
