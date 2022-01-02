/*
	AG_Sintetizador Main Synthesizer class of AG_Sinte  
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

#ifndef AG_SINTETIZADOR_H
#define AG_SINTETIZADOR_H

#include <Arduino.h>
#include "AG_Voz.h"
#include "AG_Param.h"
#include "AG_Oscilador.h"

#define MAX_POLY_OSC	18 /* osc polyphony, always active reduces single voices max poly */
#define MAX_POLY_VOICE	9 /* max single voices, can use multiple osc */

#define NOTA_BASE_KEYTRACK 64


class AG_Sintetizador 
{
    public:
        void Init(AG_Param* _param);
        void Process(float *left, float *right);
        void NoteOn(uint8_t ch, uint8_t note, float vel);
        void NoteOff(uint8_t ch, uint8_t note);

    private:
        AG_Oscilador lfo;
        AG_Voz voces[MAX_POLY_VOICE];
        AG_Voz* getFreeVoice();
        AG_Param* param;
        uint8_t notaActual;
        uint8_t notaAnterior;
};

static uint32_t count = 0;
static float outMono = 0;


#endif

