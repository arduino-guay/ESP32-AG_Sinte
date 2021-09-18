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

#include "AG_Util.h"
#include "AG_Filtro.h"

static float _senos[WAVEFORM_CNT];
static float _senoDiscreto[WAVEFORM_CNT];
static float _lfr[WAVEFORM_CNT];

static uint32_t _saltosMidi[MIDI_NOTE_CNT]; 

// costantes para los Incrementos de cents  1,00057778950655 ^ x  (0 <= x < 100)
// Para alcular frecuencia final   f + nCents =  f * _incCents[n]
static float _incCents[100]; 
static float _frecuenciasMidi[MIDI_NOTE_CNT];

uint8_t saltosSenoDiscreto = 5; 

namespace AG_Util
{
    void calculaSenoDiscreto() {
        for (int i = 0; i < WAVEFORM_CNT; i++)
        {
            _senoDiscreto[i] = ((int)(_senos[i] * saltosSenoDiscreto) % saltosSenoDiscreto) * 1.0 / (saltosSenoDiscreto-1);
        }
    }
    
    void calculaLFR() {
        float valor = random(100000) / 100000.0f;
        AG_Filtro fistro1;
        AG_Filtro fistro2;
        fistro1.setCutOff(0.1);
        fistro1.setResonance(1);
        fistro1.CalculateCoeff();
        for (int i = 0; i < WAVEFORM_CNT; i++)
        {
            _lfr[i] = valor;
            if ( i % 200 == 0 )
            {
                valor = random(100000) / 100000.0f;
            }
        }
        for (int i = 0; i < WAVEFORM_CNT; i++)
        {
            _lfr[i] = fistro1.Process(_lfr[i]);
        }
    }

    void Init()
    {
        for (int i = 0; i < WAVEFORM_CNT; i++)
        {
            _senos[i] = (float)sin(i * 2.0 * PI / WAVEFORM_CNT);
        }
        
        calculaSenoDiscreto();
        calculaLFR();
        
        for (int i = 0; i < MIDI_NOTE_CNT; i++)
        {
            _frecuenciasMidi[i] = pow(2.0f, (float)(i - 69) / 12.0f) * 440.0f;
            _saltosMidi[i] = (uint32_t)(_frecuenciasMidi[i] * ((float)(1ULL << 32ULL) / ((float)SAMPLE_RATE)));
        }

        for (int i = 0; i < 100; i++) 
        {
            _incCents[i] = pow(1.00057778950655, i);
        }        
    }

    float seno(uint32_t x)
    {
        return _senos[x];
    }

    float senoDiscreto(uint32_t x)
    {
        return _senoDiscreto[x];
    }

    float LFR(uint32_t x)
    {
        return _lfr[x];
    }

    uint32_t saltosMidi(uint8_t nota)
    {
        return _saltosMidi[nota];
    }

    uint32_t incCents(uint8_t nota,  uint8_t cents)
    {
        return (uint32_t)((_incCents[cents] - 1) * _frecuenciasMidi[nota] * ((float)(1ULL << 32ULL) / ((float)SAMPLE_RATE)));
    }

    void setSaltosSenoDiscreto(uint8_t saltos)
    {
        saltosSenoDiscreto = saltos;
        calculaSenoDiscreto();
    }
    
}

