#include <iostream>
#include <thread>
#include <ableton/Link.hpp>

extern "C" {
    #include <wiringPi.h>
}

#include "State.h"
#include "Display.h"
#include "Input.h"
#include "Output.h"

int main(void) {
    wiringPiSetup();
    piHiPri(99);

    State state;
    Input input(state);
    Output output(state);
    Display display(state);

    while (state.running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}

