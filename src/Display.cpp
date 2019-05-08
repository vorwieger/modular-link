#include "Display.h"

#include <iostream>
#include <sstream>
#include <string>

// -------------------------------------------------------------------------------------------

Display::Display(State& state_)
  : m_state(state_)
  , max7219()
{
  m_state.registerObserver(this);
}

Display::~Display() {
}

// -------------------------------------------------------------------------------------------

void Display::stateChanged() {
  switch (m_state.viewState()) {
    case TEMPO: {
      max7219.display(m_state.tempo());
      break;
    }
    case PULSE: {
      std::ostringstream oss;
      oss << (m_state.pulse() >= 10 ? "Pu" : "Pu ") << m_state.pulse();
      max7219.display(oss.str());
      break;
    }
    case LOOP: {
      std::ostringstream oss;
      oss << (m_state.loop() >= 10 ? "Lo" : "Lo ") << m_state.loop();
      max7219.display(oss.str());
      break;
    }
  }
}