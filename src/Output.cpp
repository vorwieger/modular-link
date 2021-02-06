#include "Output.h"
#include <wiringPi.h>

const double PULSE_LENGTH = 0.01; // seconds
const auto OUTPUT_THREAD_SLEEP = std::chrono::microseconds(250);

// -------------------------------------------------------------------------------------------

Output::Output(Engine& engine_)
  : m_engine(engine_)
  , m_thread(std::unique_ptr<std::thread>(new std::thread(&Output::process, this)))
{
  sched_param param;
  param.sched_priority = 99;
  if(::pthread_setschedparam(m_thread->native_handle(), SCHED_FIFO, &param) < 0) {
    std::cerr << "Failed to set output thread priority\n";
  }

  pinMode(Clock, OUTPUT);
  pinMode(Reset, OUTPUT);
  pinMode(PlayIndicator, OUTPUT);

  digitalWrite(Clock, HIGH); // inverted -> Clock/LED off
  digitalWrite(Reset, HIGH); // inverted -> Reset/LED off
  digitalWrite(PlayIndicator, LOW); // inverted -> Reset/LED off
}

Output::~Output() {
  if (m_thread == nullptr) { return; }
  m_thread->join();
  m_thread = nullptr;
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

void Output::outputClock(LinkState linkState) {
  // Fractional beat value for which clock should be high
  const double secondsPerBeat = 60.0 / linkState.tempo;
  const auto highFraction = PULSE_LENGTH / secondsPerBeat;

  // Fractional portion of current beat value
  const auto ppqn = m_engine.ppqn();
  double iptr; //ignore integral part
  const auto beatFraction = std::modf(linkState.beat * ppqn, &iptr) / ppqn;

  setClock(beatFraction <= highFraction);
  setReset(linkState.phase <= highFraction);
}

// -------------------------------------------------------------------------------------------

void Output::process() {
  while (m_engine.running()) {
    LinkState linkState = m_engine.getLinkState();
    switch (m_engine.playState()) {
      case STOPPED: {
        setPlayIndicator(false);
        setClock(false);
        setReset(false);
        break;
      }
      case CUED: {
        setPlayIndicator((long)(linkState.beat * 2) % 2 == 0);
        setClock(false);
        setReset(false);
        if (linkState.beat < 0) {
          break;
        }
        m_engine.setPlayState(PLAYING);
      }
      case PLAYING: {
        outputClock(linkState);
        setPlayIndicator(true);
        break;
      }
    }
    std::this_thread::sleep_for(OUTPUT_THREAD_SLEEP);
  }
}
