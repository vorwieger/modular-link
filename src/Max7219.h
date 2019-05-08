#pragma once

#include <string>
#include <mutex>

class Max7219 {

  // Max7219 Registers
  enum {
    MAX7219_REG_NOOP        = 0x0,
    // codes 1 to 8 are digit positions 1 to 8
    MAX7219_REG_DECODEMODE  = 0x9,
    MAX7219_REG_INTENSITY   = 0xA,
    MAX7219_REG_SCANLIMIT   = 0xB,
    MAX7219_REG_SHUTDOWN    = 0xC,
    MAX7219_REG_DISPLAYTEST = 0xF,
  };

public:

  Max7219();

  void display(int /*value_*/);
  void display(double /*value_*/);
  void display(const std::string& /*value_*/);

  static const unsigned char HYPHEN = 0b0000001;

private:

  void send16bits (unsigned short /*output*/);
  void send(unsigned char /*reg_number*/, unsigned char /*dataout*/);
  void sendChars(const std::string& /*value_*/, unsigned char /*dot*/);

  std::mutex m_updating;

  const unsigned char Max7219_font[91] = {
    0b0000000, // ' '
    Max7219::HYPHEN, // '!'
    Max7219::HYPHEN, // '"'
    Max7219::HYPHEN, // '#'
    Max7219::HYPHEN, // '$'
    Max7219::HYPHEN, // '%'
    Max7219::HYPHEN, // '&'
    Max7219::HYPHEN, // '''
    0b1001110,       // '('   - same as [
    0b1111000,       // ')'   - same as ]
    Max7219::HYPHEN, // '*'
    Max7219::HYPHEN, // '+'
    Max7219::HYPHEN, // ','
    Max7219::HYPHEN, // '-' - LOL *is* a hyphen
    0b0000000,       // '.'  (done by turning DP on)
    Max7219::HYPHEN, // '/'
    0b1111110,       // '0'
    0b0110000,       // '1'
    0b1101101,       // '2'
    0b1111001,       // '3'
    0b0110011,       // '4'
    0b1011011,       // '5'
    0b1011111,       // '6'
    0b1110000,       // '7'
    0b1111111,       // '8'
    0b1111011,       // '9'
    Max7219::HYPHEN, // ':'
    Max7219::HYPHEN, // ';'
    Max7219::HYPHEN, // '<'
    Max7219::HYPHEN, // '='
    Max7219::HYPHEN, // '>'
    Max7219::HYPHEN, // '?'
    Max7219::HYPHEN, // '@'
    0b1110111,       // 'A'
    0b0011111,       // 'B'
    0b1001110,       // 'C'
    0b0111101,       // 'D'
    0b1001111,       // 'E'
    0b1000111,       // 'F'
    0b1011110,       // 'G'
    0b0110111,       // 'H'
    0b0110000,       // 'I' - same as 1
    0b0111100,       // 'J'
    Max7219::HYPHEN, // 'K'
    0b0001110,       // 'L'
    Max7219::HYPHEN, // 'M'
    0b0010101,       // 'N'
    0b1111110,       // 'O' - same as 0
    0b1100111,       // 'P'
    Max7219::HYPHEN, // 'Q'
    0b0000101,       // 'R'
    0b1011011,       // 'S'
    0b0000111,       // 'T'
    0b0111110,       // 'U'
    Max7219::HYPHEN, // 'V'
    Max7219::HYPHEN, // 'W'
    Max7219::HYPHEN, // 'X'
    0b0100111,       // 'Y'
    Max7219::HYPHEN, // 'Z'
    0b1001110,       // '['  - same as C
    Max7219::HYPHEN, // backslash
    0b1111000,       // ']'
    Max7219::HYPHEN, // '^'
    0b0001000,       // '_'
    Max7219::HYPHEN, // '`'
    0b1110111,       // 'a'
    0b0011111,       // 'b'
    0b0001101,       // 'c'
    0b0111101,       // 'd'
    0b1001111,       // 'e'
    0b1000111,       // 'f'
    0b1011110,       // 'g'
    0b0010111,       // 'h'
    0b0010000,       // 'i'
    0b0111100,       // 'j'
    Max7219::HYPHEN, // 'k'
    0b0001110,       // 'l'
    Max7219::HYPHEN, // 'm'
    0b0010101,       // 'n'
    0b0011101,       // 'o' - same as 0
    0b1100111,       // 'p'
    Max7219::HYPHEN, // 'q'
    0b0000101,       // 'r'
    0b1011011,       // 's'
    0b0000111,       // 't'
    0b0011100,       // 'u'
    Max7219::HYPHEN, // 'v'
    Max7219::HYPHEN, // 'w'
    Max7219::HYPHEN, // 'x'
    0b0100111,       // 'y'
    Max7219::HYPHEN  // 'z'
  };  //  end of Max7219_font

};