#include "Input.h"

#include <wiringPi.h>

Input* pInstance; // wiringPi only allows to set an ISR handler through a void pointer

// -------------------------------------------------------------------------------------------

Input::Input(State& state_)
	: m_state(state_)
	, m_thread(&Input::process, this)
{
	pInstance = this;

	pinMode(PlayButton, INPUT);
	pullUpDnControl(PlayButton, PUD_DOWN);
	pinMode(EncoderButton, INPUT);
	pullUpDnControl(EncoderButton, PUD_DOWN);

	pinMode(EncoderLeft, INPUT);
  	pinMode(EncoderRight, INPUT);
  	pullUpDnControl(EncoderLeft, PUD_UP);
  	pullUpDnControl(EncoderRight, PUD_UP);
  	wiringPiISR(EncoderLeft, INT_EDGE_BOTH, encoderHandler);
  	wiringPiISR(EncoderRight, INT_EDGE_BOTH, encoderHandler);
}

Input::~Input() {
  	if (m_thread.joinable()) {
    	m_thread.join();
  	}
}

// -------------------------------------------------------------------------------------------

void Input::playButtonPressed() {
	switch (m_state.playState) {
		case Stopped:
			m_state.playState.store(Cued);
			break;
		case Cued:
		case Playing:
			m_state.playState.store(Stopped);
			break;
  }
}

void Input::encoderButtonPressed() {
	int viewState = (m_state.viewState() + 1) % NUM_VIEW_STATES;
	m_state.setViewState(static_cast<ViewState>(viewState));
}

void Input::encoderTurned(bool clockwise) {
	switch (m_state.viewState()) {

		case Tempo: { // change Tempo
			float tempo = m_state.tempo();
		    tempo += clockwise ? 0.5 : -0.5;
		    tempo = std::llround(tempo * 2) / 2.0; // round to nearest half
		    m_state.setTempo(tempo);
			break;
		}

		case ClockDiv: { // change ClockDiv
			int mode = (m_state.clockDivMode() + (clockwise ? 1 : -1)) % NUM_CLOCK_DIVS;
			m_state.setClockDivMode(static_cast<ClockDivMode>(mode));
			break;
		}
	}

}

// -------------------------------------------------------------------------------------------

bool Input::isPlayButtonPressed() {
	static uint8_t playButtonState = 0; // Current debounce status
	playButtonState = (playButtonState<<1) | digitalRead(PlayButton) | 0xe0;
	return playButtonState == 0xf0;
}

bool Input::isEncoderButtonPressed() {
	static uint8_t encoderButtonState = 0; // Current debounce status
	encoderButtonState = (encoderButtonState<<1) | digitalRead(EncoderButton) | 0xe0;
	return encoderButtonState == 0xf0;
}

void Input::encoderHandler() {
	static uint8_t encoderAxisState = 0;
  	int msb = digitalRead(pInstance->EncoderLeft);
  	int lsb = digitalRead(pInstance->EncoderRight);
  	int encoded = (msb << 1) | lsb;
  	if (encoded == 0b00) {
	    if (encoderAxisState == 0b01) {
	      pInstance->encoderTurned(true); // turned clockwise
	    } else if (encoderAxisState == 0b10) {
	      pInstance->encoderTurned(false); // turned counterclockwise
	    }
  	}
	encoderAxisState = encoded;
}


// -------------------------------------------------------------------------------------------

void Input::process() {
	while (m_state.running) {
		if (isPlayButtonPressed()) {
			playButtonPressed();
		}
		if (isEncoderButtonPressed()) {
			encoderButtonPressed();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}
