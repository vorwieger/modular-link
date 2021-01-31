#include <iostream>
#include <thread>
#include <ableton/Link.hpp>

extern "C" {
    #include <wiringPi.h>
}

#include "Engine.h"
#include "Display.h"
#include "Input.h"
#include "Output.h"

int main(void) {
  wiringPiSetup();
  piHiPri(99);

  Engine engine;
  Input input(engine);
  Output output(engine);
  Display display(engine);

  while (engine.running()) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}

