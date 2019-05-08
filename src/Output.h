#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <bitset>
#include <cmath>

#include "State.h"

class Output {

  // Using WiringPi numbering scheme
  enum OutPin {
    Clock = 1,
    Reset = 23,
    PlayIndicator = 11
  };

  public:

    Output(State& /*state*/);
    ~Output();

private:

  void process();

  void setPlayIndicator(bool /*high*/);
  void setClock(bool /*high*/);
  void setReset(bool /*high*/);

  bool m_playHigh = false;
  bool m_clockHigh = false;
  bool m_resetHigh = false;

  void outputClock(double /*beats*/, double /*phase*/, double /*tempo*/, int /*pulsesPerBeat*/);

  State& m_state;
  std::thread m_thread;

};