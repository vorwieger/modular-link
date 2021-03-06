#pragma once

#include <atomic>
#include <iostream>
#include <memory>
#include <thread>
#include <bitset>
#include <cmath>

#include "Engine.h"

class Output {

  // Using WiringPi numbering scheme
  enum OutPin {
    Clock = 1,
    Reset = 23,
    PlayIndicator = 11
  };

  public:

    Output(Engine& /*engine*/);
    ~Output();

private:

  void process();

  void setPlayIndicator(bool /*high*/);
  void setClock(bool /*high*/);
  void setReset(bool /*high*/);

  bool m_playHigh = false;
  bool m_clockHigh = false;
  bool m_resetHigh = false;

  void outputClock(LinkState /*linkState*/);
  
  Engine& m_engine;
  std::unique_ptr<std::thread> m_thread;

};