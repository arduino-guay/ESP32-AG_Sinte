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
#include "AG_Arpegiador.h"
#include "AG_Param.h"

#define NORM127MUL  0.007874f
#define SOLO_TECLADO 0
#define SOLO_ARPEGIADOR 1
#define MEZCLA 2

class AG_Midi 
{
    public:
        AG_Midi(AG_Sintetizador* _sinte, AG_Param* _param, AG_Arpegiador* _arp) { sinte = _sinte; arp = _arp; param = _param; }
        void Process();
        void Setup();
        void setModo(uint8_t _modo) { 
            arp->setEncendido(_modo != SOLO_TECLADO);
            modo = _modo; 
        }

    private:
        static AG_Sintetizador* sinte;
        static AG_Arpegiador* arp;
        static uint8_t modo;  // 0->Teclado 1->Arpegiador 2->Mezcla
        static AG_Param* param;

        static void NoteOn(uint8_t ch, uint8_t note, uint8_t vel) 
        {
            switch (modo)
            {
            case SOLO_TECLADO:
            case MEZCLA:
                sinte->NoteOn(ch, note, vel * NORM127MUL);
                break;
            case SOLO_ARPEGIADOR:
                arp->noteOn(note);
                break;
            }
            //Serial.printf("ON Ch:%d Nota:%d Vel:%d\n", ch, note, vel);
        }

        static void NoteOff(uint8_t ch, uint8_t note, uint8_t vel)
        {
            switch (modo)
            {
            case SOLO_TECLADO:
            case MEZCLA:
                sinte->NoteOff(ch, note);
                break;
            case SOLO_ARPEGIADOR:
                arp->noteOff(note);
                break;
            }
        }

        static void ControlChange(uint8_t ch, uint8_t data1, uint8_t data2)
        {
            //Serial.printf("CC Ch,D1, D2 %d %d %d\n", ch, data1, data2);
            if (data1 == 1)
            {
                //sinte->setModWheel(ch, (float)data2 * NORM127MUL);
                param->setPortamento((float)data2 * NORM127MUL);
            }
        }

        static void PitchBend(uint8_t ch, int bend)
        {
            //Serial.printf("Bend Ch,bend, %d %d\n", ch, bend);
            param->setPitchBend(ch, bend * (0.2f / 8192.0f));
        }

};

#endif
