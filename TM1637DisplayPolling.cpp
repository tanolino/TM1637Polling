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

#include "TM1637DisplayPolling.h"
#include <Arduino.h>

namespace
{
    constexpr const uint8_t TM1637_CMD1 = 0b01000000;
    constexpr const uint8_t TM1637_CMD2 = 0b11000000;
    constexpr const uint8_t TM1637_CMD3 = 0b10001111;

    //
    //      A
    //     ---
    //  F |   | B
    //     -G-
    //  E |   | C
    //     ---
    //      D
    constexpr const uint8_t digitToSegment[] = {
        // XGFEDCBA
        0b00111111,    // 0
        0b00000110,    // 1
        0b01011011,    // 2
        0b01001111,    // 3
        0b01100110,    // 4
        0b01101101,    // 5
        0b01111101,    // 6
        0b00000111,    // 7
        0b01111111,    // 8
        0b01101111,    // 9
        0b01110111,    // A
        0b01111100,    // b
        0b00111001,    // C
        0b01011110,    // d
        0b01111001,    // E
        0b01110001     // F
    };

    constexpr const uint8_t minusSegments = 0b01000000;
    

    constexpr const uint8_t endForStatesLookup[] = {
        TM1637DisplayPolling::SendByteState::End_1, // CMD1,
        TM1637DisplayPolling::SendByteState::Ack_2, // CMD2,
        TM1637DisplayPolling::SendByteState::Ack_2, // CMD2_D0,
        TM1637DisplayPolling::SendByteState::Ack_2, // CMD2_D1,
        TM1637DisplayPolling::SendByteState::Ack_2, // CMD2_D2,
        TM1637DisplayPolling::SendByteState::End_1, // CMD2_D3,
        TM1637DisplayPolling::SendByteState::End_1, // CMD3,
    };

    constexpr const uint8_t startForStatesLookup[] = {
        TM1637DisplayPolling::SendByteState::Start_0, // CMD1,
        TM1637DisplayPolling::SendByteState::Start_0, // CMD2,
        TM1637DisplayPolling::SendByteState::Start_1, // CMD2_D0,
        TM1637DisplayPolling::SendByteState::Start_1, // CMD2_D1,
        TM1637DisplayPolling::SendByteState::Start_1, // CMD2_D2,
        TM1637DisplayPolling::SendByteState::Start_1, // CMD2_D3,
        TM1637DisplayPolling::SendByteState::Start_0, // CMD3,
    };
}

TM1637DisplayPolling::TM1637DisplayPolling(const int clk_pin, const int data_pin)
    : m_cmd_1 { TM1637_CMD1 }
    , m_cmd_2 { TM1637_CMD2, }
    , m_cmd_2_data {0,0,0,0}
    , m_cmd_3 { TM1637_CMD3 }
    , m_writeBitProgress { 0 }
    , m_pinClk{ clk_pin }
    , m_pinDIO{ data_pin }
    , m_InternalState{ TM1637DisplayPolling::InternalState::Idle }
{
    pinMode(m_pinClk, INPUT);
    pinMode(m_pinDIO,INPUT);
    digitalWrite(m_pinClk, LOW);
    digitalWrite(m_pinDIO, LOW);
}

void TM1637DisplayPolling::setBrightness(uint8_t brightness, bool on = true)
{
    m_cmd_3 = brightness & 0b0111;
    if (on)
        m_cmd_3 |= 0b1000;
    m_cmd_3 |= TM1637_CMD3;
}

void
TM1637DisplayPolling::setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0)
{
    const uint8_t end = pos + length;
    for (uint8_t i = pos; i < end; ++i)
        m_cmd_2_data[i] = segments[i];
}

void
TM1637DisplayPolling::clear()
{
    for (uint8_t i = 0; i < 4; ++i )
        m_cmd_2_data[i] = 0;
}

void
TM1637DisplayPolling::showNumberDec(int num, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0)
{
    showNumberDecEx(num, 0, leading_zero, length, pos);
}

void
TM1637DisplayPolling::showNumberDecEx(int num, uint8_t dots = 0, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0)
{
    showNumberBaseEx(num < 0? -10 : 10, num < 0? -num : num, dots, leading_zero, length, pos);
}

void
TM1637DisplayPolling::showNumberHexEx(uint16_t num, uint8_t dots = 0, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0)
{
    showNumberBaseEx(16, num, dots, leading_zero, length, pos);
}

static constexpr uint8_t
TM1637DisplayPolling::encodeDigit(uint8_t digit)
{
	return digitToSegment[digit & 0x0f];
}

void
TM1637DisplayPolling::update()
{
    m_InternalState = InternalState::CMD1;
    sendState(SendByteState::Start_0, 0);
}

void
TM1637DisplayPolling::step()
{
    if (idle())
        return;
    
    uint8_t end = endForStatesLookup[m_InternalState];
    if (m_SendByteState >= end)
    {
        m_InternalState = m_InternalState + 1;
        sendState(startForStatesLookup[m_InternalState], 0);
    }
    else
    {
        uint8_t data = 0;
        switch (m_InternalState)
        {
        case InternalState::CMD1:
            data = m_cmd_1;
            break;
        case InternalState::CMD2:
            data = m_cmd_2;
            break;
        case InternalState::CMD2_D0:
            data = m_cmd_2_data[0];
            break;
        case InternalState::CMD2_D1:
            data = m_cmd_2_data[1];
            break;
        case InternalState::CMD2_D2:
            data = m_cmd_2_data[2];
            break;
        case InternalState::CMD2_D3:
            data = m_cmd_2_data[3];
            break;
        case InternalState::CMD3:
            data = m_cmd_3;
            break;
        }
        sendState(m_SendByteState + 1, data);
    }
}

void
TM1637DisplayPolling::flush()
{
    update();
    while (!idle()) {
        delayMicroseconds(DEFAULT_BIT_DELAY);
        step();
    }
}

void
TM1637DisplayPolling::showDots(uint8_t dots, uint8_t* digits)
{
    for(uint8_t i = 0; i < 4; ++i)
    {
        digits[i] |= (dots & 0x80);
        dots <<= 1;
    }
}

void
TM1637DisplayPolling::showNumberBaseEx(int8_t base, uint16_t num, uint8_t dots, bool leading_zero,
                                    uint8_t length, uint8_t pos)
{
    bool negative = false;
    if (base < 0) {
        base = -base;
        negative = true;
    }

    uint8_t digits[4];

	if (num == 0 && !leading_zero) {
		// Singular case - take care separately
		for(uint8_t i = 0; i < (length-1); i++)
			digits[i] = 0;
		digits[length-1] = encodeDigit(0);
	}
	else {
		for(int8_t i = length-1; i >= 0; --i)
		{
		    uint8_t digit = num % base;
			
			if (digit == 0 && num == 0 && leading_zero == false)
			    // Leading zero is blank
				digits[i] = 0;
			else
			    digits[i] = encodeDigit(digit);
				
			if (digit == 0 && num == 0 && negative) {
			    digits[i] = minusSegments;
				negative = false;
			}

			num /= base;
		}
    }

	if(dots != 0)
	{
		showDots(dots, digits);
	}
    
    setSegments(digits, length, pos);
}

void
TM1637DisplayPolling::sendState(SendByteState state, uint8_t data)
{
    m_SendByteState = state;
    switch (state) {
    case SendByteState::Start_0:
        goto data_down;
    case SendByteState::Start_1:
        goto clk_down;
    case SendByteState::WriteBit_0:
        if (data &= (1 << m_writeBitProgress))
            goto data_up;
        else
            goto data_down;
    case SendByteState::WriteBit_1:
        goto clk_up;
    case SendByteState::WriteBit_2:
        if (++m_writeBitProgress < 8)
            m_SendByteState = SendByteState::Start_1;
        goto clk_down;
    case SendByteState::Ack_0:
        m_writeBitProgress = 0;
        goto data_up;
    case SendByteState::Ack_1:
        goto clk_up;
    case SendByteState::Ack_2:
        if(digitalRead(m_pinDIO))
            m_SendByteState = SendByteState::Ack_1;
        else
            goto data_down;
        break;
    case SendByteState::Ack_3:
        goto clk_down;
    case SendByteState::End_0:
        goto clk_up;
    case SendByteState::End_1:
        goto data_up;
    }
    return;

clk_up:
    pinMode(m_pinClk, INPUT);
    return;
clk_down:
    pinMode(m_pinClk, OUTPUT);
    return;
data_up:
    pinMode(m_pinDIO, INPUT);
    return;
data_down:
    pinMode(m_pinDIO, OUTPUT);
    return;
}