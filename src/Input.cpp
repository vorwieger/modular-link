#include "Input.h"

#include <wiringPi.h>

const auto INPUT_THREAD_SLEEP = std::chrono::milliseconds(10);

Input* pInstance; // wiringPi only allows to set an ISR handler through a void pointer

// -------------------------------------------------------------------------------------------

Input::Input(Engine& engine_)
  : m_engine(engine_)
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
  switch (m_engine.playState()) {
    case STOPPED: {
      m_engine.startTimeline();
      break;
    }
    case CUED: {
      m_engine.stopTimeline();
      break;
    }
    case PLAYING: {
      m_engine.stopTimeline();
      break;
    }
  }
}

void Input::encoderButtonPressed() {
  switch (m_engine.viewState()) {
    case TEMPO: {
      m_engine.setViewState(PPQN);
      break;
    }
    case PPQN: {
       m_engine.setViewState(QUANTUM);
      break;
    }
    case QUANTUM: {
      m_engine.setViewState(TEMPO);
      break;
    }
  }
}

void Input::encoderTurned(bool clockwise) {
  switch (m_engine.viewState()) {
    case TEMPO: { // change Tempo
      int step = m_engine.tempo() >= 300 ? 10 : 1;
      m_engine.setTempo(std::llround(m_engine.tempo() + (clockwise ? step : -step)));
      break;
    }
    case PPQN: { // change ppqn
      m_engine.changePpqn(clockwise);
      break;
    }
    case QUANTUM: { // change quantum
      m_engine.setQuantum(m_engine.quantum() + (clockwise ? 1 : -1));
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
  static unsigned int lastEncoderAxisStateChange = 0;
  static uint8_t encoderAxisState = 0;
  int msb = digitalRead(pInstance->EncoderLeft);
  int lsb = digitalRead(pInstance->EncoderRight);
  int encoded = (msb << 1) | lsb;
  if (encoded == 0b00) {
    if (encoderAxisState == 0b01) {
      if (millis() - lastEncoderAxisStateChange < 10) { return; } // bounce -> ignore
      lastEncoderAxisStateChange = millis();
      pInstance->encoderTurned(true); // turned clockwise
    } else if (encoderAxisState == 0b10) {
      if (millis() - lastEncoderAxisStateChange < 10) { return; } // bounce -> ignore
      lastEncoderAxisStateChange = millis();
      pInstance->encoderTurned(false); // turned counterclockwise
    }
  }
  encoderAxisState = encoded;
}

// -------------------------------------------------------------------------------------------

void Input::process() {
  while (m_engine.running()) {
    if (isPlayButtonPressed()) {
      playButtonPressed();
    }
    if (isEncoderButtonPressed()) {
      encoderButtonPressed();
    }
    std::this_thread::sleep_for(INPUT_THREAD_SLEEP);
  }
}
