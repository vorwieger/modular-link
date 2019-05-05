#pragma once

#include <atomic>
#include <iostream>
#include <thread>
#include <bitset>
#include <cmath>

#include "State.h"

class Output {

	// Using WiringPi numbering scheme
	enum OutPin {
      Clock = 1,
      Reset = 23,
      PlayIndicator = 11
  	};

	public:

		Output(State& /*state*/);
		~Output();


	private:

		void process();

		void outputClock(double /*beats*/, double /*phase*/, double /*tempo*/, float /*clockDiv*/);

		State& m_state;
  		std::thread m_thread;

};