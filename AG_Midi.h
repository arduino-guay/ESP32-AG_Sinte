/*
	AG_Midi - Midi Interface for  AG_Sinte  
	Copyright (C) 2021, J. Daniel Herrera Gomariz
    https://github.com/arduino-guay/AG_Sinte

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

#ifndef AG_MIDI_H
#define AG_MIDI_H

#include "AG_Sintetizador.h"
#define NORM127MUL  0.007874f

class AG_Midi 
{
    public:
        AG_Midi(AG_Sintetizador* _sinte) { sinte = _sinte; }
        void Process();
        void Setup();

    private:
        static AG_Sintetizador* sinte;
        static void NoteOn(uint8_t ch, uint8_t note, uint8_t vel) 
        {
            sinte->NoteOn(ch, note, vel * NORM127MUL);
            //Serial.printf("ON Ch:%d Nota:%d Vel:%d\n", ch, note, vel);
        }
        static void NoteOff(uint8_t ch, uint8_t note, uint8_t vel)
        {
            sinte->NoteOff(ch, note);
        }
        static void ControlChange(uint8_t ch, uint8_t data1, uint8_t data2)
        {
            //Serial.printf("CC Ch,D1, D2 %d %d %d\n", ch, data1, data2);
            if (data1 == 1)
            {
                sinte->setModWheel(ch, (float)data2 * NORM127MUL);
            }
        }
        static void PitchBend(uint8_t ch, int bend)
        {
            //Serial.printf("Bend Ch,bend, %d %d\n", ch, bend);
            sinte->setPitchBend(ch, ((float)bend - 8192.0f) * (1.0f / 8192.0f) - 1.0f);
        }

};

#endif
