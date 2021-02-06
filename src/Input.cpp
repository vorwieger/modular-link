#include "Input.h"

#include <wiringPi.h>


Input* pInstance; // wiringPi only allows to set an ISR handler through a void pointer

// -------------------------------------------------------------------------------------------

Input::Input(Engine& engine_)
  : m_engine(engine_)
{
  pInstance = this;

  wiringPiISR(PlayButton, INT_EDGE_FALLING, playButtonHandler);
  wiringPiISR(EncoderButton, INT_EDGE_FALLING, encoderButtonHandler);
  wiringPiISR(EncoderLeft, INT_EDGE_BOTH, encoderHandler);
  wiringPiISR(EncoderRight, INT_EDGE_BOTH, encoderHandler);
}

// -------------------------------------------------------------------------------------------

void Input::playButtonHandler() {
  static unsigned int lastPlayButtonTime = 0;
  unsigned now = millis() ;
  if (now > lastPlayButtonTime + 250) {
    lastPlayButtonTime = now;
    pInstance->playButtonPressed();
  }
}

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

// -------------------------------------------------------------------------------------------

void Input::encoderButtonHandler() {
  static unsigned int lastEncoderButtonTime = 0;
  unsigned now = millis() ;
  if (now > lastEncoderButtonTime + 250) {
    lastEncoderButtonTime = now;
    pInstance->encoderButtonPressed();
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

// -------------------------------------------------------------------------------------------

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
