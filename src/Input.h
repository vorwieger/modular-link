#pragma once

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <bitset>

#include "State.h"

class Input {

	// Using WiringPi numbering scheme
	enum InPin {
		PlayButton = 21,
		EncoderButton = 22,  // BCM 6
		EncoderLeft = 27,      // BCM 19
		EncoderRight = 24     // BCM 16
	};

	public:

		Input(State& /*state*/);
		~Input();

		void playButtonPressed();
		void encoderButtonPressed();
		void encoderTurned(bool /*clockwise*/);

	private:

		void process();

		bool isPlayButtonPressed();
		bool isEncoderButtonPressed();
		static void encoderHandler();

		State& m_state;
  		std::thread m_thread;

};