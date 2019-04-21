/* Copyright 2016, Ableton AG, Berlin. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  If you would like to incorporate Link into a proprietary software application,
 *  please contact <link-devs@ableton.com>.
 */

#include "AudioPlatform_Portaudio.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#if defined(LINK_PLATFORM_UNIX)
#include <termios.h>
#endif

#include <wiringPi.h>

namespace
{

const unsigned kPinPWM1{1};  // BCM 18
const unsigned kPinPWM2{23}; // BCM 13
const unsigned kPinLedRunning{11}; // BCM 7

//#define DATA        0 // GPIO 17 (WiringPi pin num 0)  header pin 11
//#define CLOCK       3 // GPIO 22 (WiringPi pin num 3)  header pin 15
//#define LOAD        4 // GPIO 23 (WiringPi pin num 4)  header pin 16

#define DATA        12 // GPIO 10 (WiringPi pin num 12)  header pin 19
#define CLOCK       14 // GPIO 11 (WiringPi pin num 14)  header pin 23
#define LOAD        10 // GPIO  8 (WiringPi pin num 10)  header pin 24

// The Max7219 Registers :

#define DECODE_MODE   0x09
#define INTENSITY     0x0a
#define SCAN_LIMIT    0x0b
#define SHUTDOWN      0x0c
#define DISPLAY_TEST  0x0f

struct State
{
  std::atomic<bool> running;
  ableton::Link link;
  ableton::linkaudio::AudioPlatform audioPlatform;

  State()
    : running(true)
    , link(120.)
    , audioPlatform(link)
  {
    link.enable(true);
  }
};

void disableBufferedInput()
{
#if defined(LINK_PLATFORM_UNIX)
  termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag &= static_cast<unsigned long>(~ICANON);
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif
}

void enableBufferedInput()
{
#if defined(LINK_PLATFORM_UNIX)
  termios t;
  tcgetattr(STDIN_FILENO, &t);
  t.c_lflag |= ICANON;
  tcsetattr(STDIN_FILENO, TCSANOW, &t);
#endif
}

void clearLine()
{
  std::cout << "   \r" << std::flush;
  std::cout.fill(' ');
}

void printHelp()
{
  std::cout << std::endl << " < L I N K  H U T >" << std::endl << std::endl;
  std::cout << "usage:" << std::endl;
  std::cout << "  start / stop: p" << std::endl;
  std::cout << "  decrease / increase tempo: w / e" << std::endl;
  std::cout << "  decrease / increase quantum: r / t" << std::endl;
  std::cout << "  enable / disable start stop sync: s" << std::endl;
  std::cout << "  quit: q" << std::endl << std::endl;
}

void printState(const std::chrono::microseconds time,
  const ableton::Link::SessionState sessionState,
  const std::size_t numPeers,
  const bool isPlaying,
  const double quantum,
  const bool startStopSyncOn)
{
  const auto beats = sessionState.beatAtTime(time, quantum);
  const auto phase = sessionState.phaseAtTime(time, quantum);
  const auto startStop = startStopSyncOn ? "on" : "off";
  std::cout << std::defaultfloat << "peers: " << numPeers << " | "
            << "playing: " << (isPlaying ? "on" : "off") << " | "
            << "quantum: " << quantum << " | "
            << "start stop sync: " << startStop << " | "
            << "tempo: " << sessionState.tempo() << " | " << std::fixed
            << "beats: " << beats << " | ";
  for (int i = 0; i < ceil(quantum); ++i)
  {
    if (i < phase)
    {
      std::cout << 'X';
    }
    else
    {
      std::cout << 'O';
    }
  }
  clearLine();

  // set LED
  digitalWrite(kPinLedRunning, isPlaying ? 1 : 0);
}

void input(State& state)
{
  char in;

#if defined(LINK_PLATFORM_WINDOWS)
  HANDLE stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
  DWORD numCharsRead;
  INPUT_RECORD inputRecord;
  do
  {
    ReadConsoleInput(stdinHandle, &inputRecord, 1, &numCharsRead);
  } while ((inputRecord.EventType != KEY_EVENT) || inputRecord.Event.KeyEvent.bKeyDown);
  in = inputRecord.Event.KeyEvent.uChar.AsciiChar;
#elif defined(LINK_PLATFORM_UNIX)
  in = static_cast<char>(std::cin.get());
#endif

  const auto tempo = state.link.captureAppSessionState().tempo();
  auto& engine = state.audioPlatform.mEngine;

  switch (in)
  {
  case 'q':
    state.running = false;
    clearLine();
    return;
  case 'w':
    engine.setTempo(tempo - 1);
    break;
  case 'e':
    engine.setTempo(tempo + 1);
    break;
  case 'r':
    engine.setQuantum(engine.quantum() - 1);
    break;
  case 't':
    engine.setQuantum(std::max(1., engine.quantum() + 1));
    break;
  case 's':
    engine.setStartStopSyncEnabled(!engine.isStartStopSyncEnabled());
    break;
  case 'p':
    if (engine.isPlaying())
    {
      engine.stopPlaying();
    }
    else
    {
      engine.startPlaying();
    }
    break;
  }

  input(state);
}

} // namespace


static void MAX7219_Send16bits (unsigned short output)
{
  unsigned char i;
  for (i=16; i>0; i--) {
    unsigned short mask = 1 << (i - 1); // calculate bitmask
    digitalWrite(CLOCK, 0);  // set clock to 0
    // Send one bit on the data pin
    if (output & mask)
      digitalWrite(DATA, 1);
    else
      digitalWrite(DATA, 0);
    digitalWrite(CLOCK, 1);  // set clock to 1
  }
}

static void MAX7219_Send (unsigned char reg_number, unsigned char dataout)
{
  digitalWrite(LOAD, 1);  // set LOAD 1 to start
  MAX7219_Send16bits((reg_number << 8) + dataout);
  digitalWrite(LOAD, 0);  // LOAD 0 to latch
  digitalWrite(LOAD, 1);  // set LOAD 1 to finish
}

static void MAX7219_Init (void)
{
  pinMode(DATA, 1);               // OUTPUT
  pinMode(CLOCK, 1);              // OUTPUT
  pinMode(LOAD, 1);               // OUTPUT
  MAX7219_Send(SCAN_LIMIT, 7);     // set up to scan all eight digits
  MAX7219_Send(DECODE_MODE, 0xFF); // Set BCD decode mode on
  MAX7219_Send(DISPLAY_TEST, 0);   // Disable test mode
  MAX7219_Send(INTENSITY, 0);      // set brightness 0 to 15
  MAX7219_Send(SHUTDOWN, 1);       // come out of shutdown mode / turn on the digits
}

int main(int, char**)
{
  State state;
  printHelp();
  std::thread thread(input, std::ref(state));
  disableBufferedInput();

  if (wiringPiSetup () == -1) exit (1) ;
  // setup pwm pins (clock & reset)
  pinModeAlt(kPinPWM1, 2); // ALT5
  pinModeAlt(kPinPWM2, 4); // ALT0
  pinMode(kPinLedRunning, 1); // OUTPUT

  // Display
  MAX7219_Init();
  MAX7219_Send(4,'1');
  MAX7219_Send(3,'2');
  MAX7219_Send(2,'3');
  MAX7219_Send(1,'4');

  while (state.running)
  {
    const auto time = state.link.clock().micros();
    auto sessionState = state.link.captureAppSessionState();
    printState(time, sessionState, state.link.numPeers(),
      state.audioPlatform.mEngine.isPlaying(),
      state.audioPlatform.mEngine.quantum(),
      state.audioPlatform.mEngine.isStartStopSyncEnabled());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  enableBufferedInput();
  thread.join();
  return 0;
}
