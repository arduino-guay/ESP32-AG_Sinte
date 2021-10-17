/*
	AG_Util - Utility class for AG_Sinte  
	Copyright (C) 2021, J. Daniel Herrera Gomariz
    https://github.com/arduino-guay/AG_Sinte

    Based on https://github.com/marcel-licence/esp32_basic_synth by Marcel Licence

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef AG_UTIL_H
#define AG_UTIL_H

#include <Arduino.h>
#include "config.h"
/*
 * this is just a kind of magic to go through the waveforms
 * - WAVEFORM_BIT sets the bit length of the pre calculated waveforms
 */
#define WAVEFORM_BIT 10UL
#define WAVEFORM_CNT (1 << WAVEFORM_BIT)
#define WAVEFORM_Q4 (1 << (WAVEFORM_BIT - 2))
#define WAVEFORM_MSK ((1 << WAVEFORM_BIT) - 1)
#define WAVEFORM_I(i) ((i) >> (32 - WAVEFORM_BIT)) & WAVEFORM_MSK
#define MIDI_NOTE_CNT 160

namespace AG_Util
{
    void Init();
    float seno(uint32_t x);
    float senoDiscreto(uint32_t x);
    float akwf1(uint32_t x);
    float akwf2(uint32_t x);
    float LFR(uint32_t x);
    uint32_t saltosMidi(uint8_t nota);
    uint32_t incCents(uint8_t nota,  uint8_t cents);
    float getFrecuencia(uint8_t nota);
    void setSaltosSenoDiscreto(uint8_t saltos);
	  void setAKWF1(uint8_t valor);
	  void setAKWF2(uint8_t valor);
}

#endif // AGUAY_UTIL_H

