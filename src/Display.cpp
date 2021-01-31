#include "Display.h"

#include <iostream>
#include <sstream>
#include <string>

// -------------------------------------------------------------------------------------------

Display::Display(Engine& engine_)
  : m_engine(engine_)
  , max7219()
{
  m_engine.registerObserver(this);
}

Display::~Display() {
}

// -------------------------------------------------------------------------------------------

void Display::stateChanged() {
  switch (m_engine.viewState()) {
    case TEMPO: {
      max7219.display(m_engine.tempo());
      break;
    }
    case PPQN: {
      auto ppqn = m_engine.ppqn();
      std::ostringstream oss;
      oss << (ppqn >= 10 ? "pu" : "pu ") << ppqn;
      max7219.display(oss.str());
      break;
    }
    case QUANTUM: {
      auto quantum = m_engine.quantum();
      std::ostringstream oss;
      oss << (quantum >= 10 ? "qn" : "qn ") << quantum;
      max7219.display(oss.str());
      break;
    }
  }
}