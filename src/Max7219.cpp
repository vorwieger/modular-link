#include "Max7219.h"

#include <cmath>
#include <chrono>
#include <thread>
#include <wiringPi.h>

#define MAX7219_PIN_DATA    12 // GPIO 10, WiringPi 12, Raspi PIN 19
#define MAX7219_PIN_CLOCK   14 // GPIO 11, WiringPi 14, Raspi PIN 23
#define MAX7219_PIN_LOAD    10 // GPIO  8, WiringPi 10, Raspi PIN 24

#define MAX7219_NUM_DIGITS  4  // 4 Digits

const auto MAX7219_WRITE_DELAY = std::chrono::microseconds(10); // microseconds delay after write

// -------------------------------------------------------------------------------------------------

Max7219::Max7219() {
  wiringPiSetup();
  pinMode(MAX7219_PIN_DATA, OUTPUT);
  pinMode(MAX7219_PIN_CLOCK, OUTPUT);
  pinMode(MAX7219_PIN_LOAD, OUTPUT);

  send(MAX7219_REG_SCANLIMIT, 7);     // Set up to scan all eight digits
  send(MAX7219_REG_DECODEMODE, 0);    // Set BCD decode mode off
  send(MAX7219_REG_DISPLAYTEST, 0);   // Disable test mode
  send(MAX7219_REG_INTENSITY, 0);     // Set brightness 0 to 15
  send(MAX7219_REG_SHUTDOWN, 1);      // Come out of shutdown mode / Turn on the digits

  display(" ");           // Cleanup display
}

// -------------------------------------------------------------------------------------------------

void Max7219::send16bits (unsigned short output) {
  unsigned char i;
  for (i=16; i>0; i--) {
    unsigned short mask = 1 << (i - 1); // calculate bitmask
    digitalWrite(MAX7219_PIN_CLOCK, 0);  // set clock to 0
    std::this_thread::sleep_for(MAX7219_WRITE_DELAY);

    // Send one bit on the data pin
    if (output & mask) {
      digitalWrite(MAX7219_PIN_DATA, 1);
    } else {
      digitalWrite(MAX7219_PIN_DATA, 0);
    }
    std::this_thread::sleep_for(MAX7219_WRITE_DELAY);

    digitalWrite(MAX7219_PIN_CLOCK, 1);  // set clock to 1
    std::this_thread::sleep_for(MAX7219_WRITE_DELAY);
  }
}

void Max7219::send(unsigned char reg_number, unsigned char dataout) {
  digitalWrite(MAX7219_PIN_LOAD, 0);  // set LOAD 0 to start
    std::this_thread::sleep_for(MAX7219_WRITE_DELAY);
  send16bits((reg_number << 8) + dataout);
  digitalWrite(MAX7219_PIN_LOAD, 1);  // set LOAD 1 to finish
    std::this_thread::sleep_for(MAX7219_WRITE_DELAY);
}

void Max7219::sendChars(const std::string& value, unsigned char dot) {
  for (unsigned i = 0; i < MAX7219_NUM_DIGITS; i++)  {
    unsigned char converted = 0b0000001;    // hyphen as default
    if (value[i] >= ' ' && value[i] <= 'z') {
      converted = Max7219_font[value[i] - ' '];
    }
    bool showDot = dot & (1 << (MAX7219_NUM_DIGITS - i - 1));
    send(MAX7219_NUM_DIGITS - i, converted | (showDot ? 0b10000000 : 0));
    }
}

// -------------------------------------------------------------------------------------------------

void Max7219::display(int value_) {
  std::lock_guard<std::mutex> lock(m_updating);

  std::string strValue = std::to_string(static_cast<int>(value_));
  std::string strFill = "";

  if (strValue.length() < 4) {
    strFill = std::string(4 - strValue.length(), ' ');
  } else if (strValue.length() > 4) {
    strValue = "-HI-";
  }

  sendChars(strFill + strValue, 0);
}

// -------------------------------------------------------------------------------------------------

void Max7219::display(double value_) {
  std::lock_guard<std::mutex> lock(m_updating);

  double integral;
  double fractional = modf(value_, &integral);
  std::string strValue = std::to_string(static_cast<int>(integral));
  std::string strFill = "";
  bool showDot = false;

  if (strValue.length() < 4) {
    auto uFractional = lround(fractional * 10);
    if (uFractional > 9) {
      uFractional = 0;
      strValue = std::to_string(static_cast<int>(integral + 1));
    }
    std::string strFractional = std::to_string(uFractional);
    unsigned length = strValue.length() + strFractional.length();
    unsigned leftFills = length > 4 ? 0 : 4 - length;
    strFill = std::string(leftFills, ' ');
    strValue.append(strFractional);
    showDot = true;
  } else if (strValue.length() > 4) {
    strValue = "-HI-";
  }

  sendChars(strFill + strValue, showDot ? 0b0010 : 0);
}

// -------------------------------------------------------------------------------------------------

void Max7219::display(const std::string& value_) {
  std::lock_guard<std::mutex> lock(m_updating);

  std::string strValue(value_);
  std::string strFill = "";

  if (strValue.length() < 4) {
    auto length = strValue.length();
    auto leftFills = length > 4 ? 0 : 4 - length;
    strFill = std::string(leftFills, ' ');
  } else {
    strValue = strValue.substr(0, 4);
  }

  sendChars(strFill + strValue, 0);
}
