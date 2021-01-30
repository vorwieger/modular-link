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
    case PULSE: {
      std::ostringstream oss;
      oss << (m_engine.pulse() >= 10 ? "Pu" : "Pu ") << m_engine.pulse();
      max7219.display(oss.str());
      break;
    }
    case LOOP: {
      std::ostringstream oss;
      oss << (m_engine.loop() >= 10 ? "Lo" : "Lo ") << m_engine.loop();
      max7219.display(oss.str());
      break;
    }
  }
}