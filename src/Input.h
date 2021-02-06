#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <bitset>

#include "Engine.h"

class Input {

  // Using WiringPi numbering scheme
  enum InPin {
    PlayButton = 21,
    EncoderButton = 22,  // BCM 6
    EncoderLeft = 27,    // BCM 19
    EncoderRight = 24    // BCM 16
  };

  public:

    Input(Engine& /*engine*/);

    void playButtonPressed();
    void encoderButtonPressed();
    void encoderTurned(bool /*clockwise*/);

  private:

    Engine& m_engine;

    static void playButtonHandler();
    static void encoderButtonHandler();
    static void encoderHandler();

  };