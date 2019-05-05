#include "Input.h"

#include <wiringPi.h>

Input* pInstance; // wiringPi only allows to set an ISR handler through a void pointer

// -------------------------------------------------------------------------------------------

Input::Input(State& state_)
	: m_state(state_)
	, m_thread(&Input::process, this) {

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
	int mode = (m_state.clockDivMode + 1) % NUM_CLOCK_DIVS;
	m_state.clockDivMode.store(static_cast<ClockDivMode>(mode));
}

void Input::encoderTurned(bool clockwise) {
	const auto time = m_state.link.clock().micros();
	auto sessionState = m_state.link.captureAppSessionState();

	auto tempo = sessionState.tempo();
    tempo += clockwise ? 0.5 : -0.5;
    tempo = std::llround(tempo * 2) / 2.0; // round to nearest half
    sessionState.setTempo(tempo, time);

	m_state.link.commitAudioSessionState(sessionState);
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
