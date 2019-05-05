#include "Input.h"

#include <wiringPi.h>

// -------------------------------------------------------------------------------------------

Input::Input(State& state_)
	: m_state(state_)
	, m_thread(&Input::process, this) {

	pinMode(PlayButton, INPUT);
	pullUpDnControl(PlayButton, PUD_DOWN);
	pinMode(EncoderButton, INPUT);
	pullUpDnControl(EncoderButton, PUD_DOWN);
}

Input::~Input() {
  	if (m_thread.joinable()) {
    	m_thread.join();
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
	int mode = (m_state.clockDivMode + 1) % NUM_CLOCK_DIVS;
	m_state.clockDivMode.store(static_cast<ClockDivMode>(mode));
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
