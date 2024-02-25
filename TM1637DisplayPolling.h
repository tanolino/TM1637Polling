//  This library is a heavily modified fork of TM1637Display
//  Initial Author: vishorp@gmail.com
//  Modification Author: taniga_danae@gmx.de
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __TM1637DISPLAY_POLL__
#define __TM1637DISPLAY_POLL__

extern "C" {
    #include <inttypes.h>
}

constexpr uint8_t SEG_A = 0b00000001;
constexpr uint8_t SEG_B = 0b00000010;
constexpr uint8_t SEG_C = 0b00000100;
constexpr uint8_t SEG_D = 0b00001000;
constexpr uint8_t SEG_E = 0b00010000;
constexpr uint8_t SEG_F = 0b00100000;
constexpr uint8_t SEG_G = 0b01000000;
constexpr uint8_t SEG_DP= 0b10000000;

constexpr unsigned int DEFAULT_BIT_DELAY = 100;

class TM1637DisplayPolling
{
public:
    //! Initialize a TM1637Display object, setting the clock and
    //! data pins.
    //!
    //! @param pinClk - The number of the digital pin connected to the clock pin of the module
    //! @param pinDIO - The number of the digital pin connected to the DIO pin of the module
    //!                   bus connected to the display
    TM1637DisplayPolling(const int clk_pin, const int data_pin);

    //! Sets the brightness of the display.
    //!
    //! The setting takes effect when a command is given to change the data being
    //! displayed.
    //!
    //! @param brightness A number from 0 (lowes brightness) to 7 (highest brightness)
    //! @param on Turn display on or off
    void setBrightness(uint8_t brightness, bool on = true);

    //! Display arbitrary data on the module
    //!
    //! This function receives raw segment values as input and displays them. The segment data
    //! is given as a byte array, each byte corresponding to a single digit. Within each byte,
    //! bit 0 is segment A, bit 1 is segment B etc.
    //! The function may either set the entire display or any desirable part on its own. The first
    //! digit is given by the @ref pos argument with 0 being the leftmost digit. The @ref length
    //! argument is the number of digits to be set. Other digits are not affected.
    //!
    //! @param segments An array of size @ref length containing the raw segment values
    //! @param length The number of digits to be modified
    //! @param pos The position from which to start the modification (0 - leftmost, 3 - rightmost)
    void setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0);

    //! Clear the display buffer
    void clear();

    //! Display a decimal number
    //!
    //! Display the given argument as a decimal number.
    //!
    //! @param num The number to be shown
    //! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
    //!        blank. NOTE: leading zero is not supported with negative numbers.
    //! @param length The number of digits to set. The user must ensure that the number to be shown
    //!        fits to the number of digits requested (for example, if two digits are to be displayed,
    //!        the number must be between 0 to 99)
    //! @param pos The position of the most significant digit (0 - leftmost, 3 - rightmost)
    void showNumberDec(int num, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0);

    //! Display a decimal number, with dot control
    //!
    //! Display the given argument as a decimal number. The dots between the digits (or colon)
    //! can be individually controlled.
    //!
    //! @param num The number to be shown
    //! @param dots Dot/Colon enable. The argument is a bitmask, with each bit corresponding to a dot
    //!        between the digits (or colon mark, as implemented by each module). i.e.
    //!        For displays with dots between each digit:
    //!        * 0.000 (0b10000000)
    //!        * 00.00 (0b01000000)
    //!        * 000.0 (0b00100000)
    //!        * 0.0.0.0 (0b11100000)
    //!        For displays with just a colon:
    //!        * 00:00 (0b01000000)
    //!        For displays with dots and colons colon:
    //!        * 0.0:0.0 (0b11100000)
    //! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
    //!        blank. NOTE: leading zero is not supported with negative numbers.
    //! @param length The number of digits to set. The user must ensure that the number to be shown
    //!        fits to the number of digits requested (for example, if two digits are to be displayed,
    //!        the number must be between 0 to 99)
    //! @param pos The position of the most significant digit (0 - leftmost, 3 - rightmost)
    void showNumberDecEx(int num, uint8_t dots = 0, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0);

    //! Display a hexadecimal number, with dot control
    //!
    //! Display the given argument as a hexadecimal number. The dots between the digits (or colon)
    //! can be individually controlled.
    //!
    //! @param num The number to be shown
    //! @param dots Dot/Colon enable. The argument is a bitmask, with each bit corresponding to a dot
    //!        between the digits (or colon mark, as implemented by each module). i.e.
    //!        For displays with dots between each digit:
    //!        * 0.000 (0b10000000)
    //!        * 00.00 (0b01000000)
    //!        * 000.0 (0b00100000)
    //!        * 0.0.0.0 (0b11100000)
    //!        For displays with just a colon:
    //!        * 00:00 (0b01000000)
    //!        For displays with dots and colons colon:
    //!        * 0.0:0.0 (0b11100000)
    //! @param leading_zero When true, leading zeros are displayed. Otherwise unnecessary digits are
    //!        blank
    //! @param length The number of digits to set. The user must ensure that the number to be shown
    //!        fits to the number of digits requested (for example, if two digits are to be displayed,
    //!        the number must be between 0 to 99)
    //! @param pos The position of the most significant digit (0 - leftmost, 3 - rightmost)
    void showNumberHexEx(uint16_t num, uint8_t dots = 0, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0);

    //! Translate a single digit into 7 segment code
    //!
    //! The method accepts a number between 0 - 15 and converts it to the
    //! code required to display the number on a 7 segment display.
    //! Numbers between 10-15 are converted to hexadecimal digits (A-F)
    //!
    //! @param digit A number between 0 to 15
    //! @return A code representing the 7 segment image of the digit (LSB - segment A;
    //!         bit 6 - segment G; bit 7 - always zero)
    static constexpr uint8_t encodeDigit(uint8_t digit);

    enum InternalState : uint8_t
    {
        CMD1 = 0,
        CMD2,
        CMD2_D0,
        CMD2_D1,
        CMD2_D2,
        CMD2_D3,
        CMD3,
        Idle
    };

    enum SendByteState : uint8_t
    {
        Start_0 = 0,
        Start_1,
        WriteBit_0,
        WriteBit_1,
        WriteBit_2,
        Ack_0,
        Ack_1,
        Ack_2,
        Ack_3,
        End_0,
        End_1,
    };

    //! This state is important as you should not change any internal variables
    //! as long as the internal state is not idle
    inline bool idle() const noexcept { return m_InternalState == InternalState::Idle; }

    //! Unlike the original Library the TM1637DisplayPoll Library will not send anything
    //! unless you call update().
    void update();

    //! As long as idle() returns false, you can step() every ~100 Microseconds.
    //! This will proceed the sending process.
    //! The general Idea is to set all the things and then call:
    //!     display.update();
    //!     while(!display.idle()) {
    //!         delayMicroseconds(100);
    //!         display.step();
    //!     }
    //! The whole reason for this library however is to not write that.
    //! Instead you should eaily schedule other work inbetween the step() calls.
    void step();

    //! Write all data into the display, blocking.
    void flush();
    
private:
    void static showDots(uint8_t dots, uint8_t* digits);
    void showNumberBaseEx(int8_t base, uint16_t num, uint8_t dots, bool leading_zero, uint8_t length, uint8_t pos);

    void sendState(SendByteState state, uint8_t data);

    const uint8_t m_pinClk;
    const uint8_t m_pinDIO;
    
    uint8_t m_cmd_1;
    uint8_t m_cmd_2;
    uint8_t m_cmd_2_data[4];
    uint8_t m_cmd_3;
    uint8_t m_writeBitProgress;
    InternalState m_InternalState;
    SendByteState m_SendByteState;
};

#endif // __TM1637DISPLAY_POLL__