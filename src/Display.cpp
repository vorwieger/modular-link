#include "Display.h"


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

		case Tempo:
			max7219.display(m_state.tempo());
		break;

		case ClockDiv:
			max7219.display(m_state.clockDivValue());
		break;
	}
}