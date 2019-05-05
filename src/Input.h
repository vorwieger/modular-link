#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <bitset>

#include "State.h"

class Input {

	// Using WiringPi numbering scheme
	enum InPin {
		PlayButton = 21,
		EncoderButton = 22,  // BCM 6
		EncoderUp = 27,      // BCM 19
		EncoderDown = 24     // BCM 16
	};

	public:

		Input(State& /*state*/);
		~Input();


	private:

		void process();

		bool isPlayButtonPressed();
		bool isEncoderButtonPressed();

		void playButtonPressed();
		void encoderButtonPressed();

		State& m_state;
  		std::thread m_thread;

};