/*
	AG_Oscilador - Oscilator for  AG_Sinte  
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

#ifndef AG_OSCILADOR_H
#define AG_OSCILADOR_H

#include "config.h"
#include "AG_WaveTable.h"

class AG_Oscilador
{
public:
    AG_Oscilador()
    {
        volumen = 1.0;
        pitchMultiplier = 1;
        addCents = 0;
        incNota = 0;
    }

    void setModulacion(float _pitchMultiplier)
    {
        pitchMultiplier = _pitchMultiplier;
    }

    float getSiguienteValor();
    void setWfOsc (AG_WaveTable *_wfOsc) { waveForm = _wfOsc; }
    float getValor();
    void setNotaMidi(uint8_t nota);
    void setNotaMidi(uint8_t nota, uint32_t _samplePos, uint8_t cents);
    void setFrecuencia(float fHz);   
    void setFrecuencia(uint8_t notaDesde, uint8_t notaHasta, float porc);

    void setVolumen(float _volumen)
    {
        volumen = _volumen;
    }

    void setIncNota(int _incNota)
    {
        incNota = _incNota;
    }
    void reset() { samplePos = 0; }

private:
    float pitchMultiplier;
    AG_WaveTable *waveForm;
    uint32_t samplePos;
    uint32_t addVal;
    uint32_t addCents;
    float volumen;
    float valor;
    int incNota;
};

#endif
