/*
	AG_Filtro  
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


#ifndef AG_FILTRO_H
#define AG_FILTRO_H
#include <Arduino.h>
#include "AG_Util.h"

#define MAX_FREQ_CUTOFF 15000
#define NOTA_BASE_FILTRO 64

class AG_Filtro
{
public:
    enum TipoFiltro
        {
            LOWPASS = 0,
            HIGHPASS
        };

    AG_Filtro(void);
    void CalculateCoeff();
    float Process(float const signal);
    void Reset();
    float getCutOff() { return cutOff; }
    float getResonace() { return resonance; }
    // Cutoff en Hz
    void setCutOff(float _cutOff) { _cutOff >= 1.0 ? cutOff = 0.99: cutOff = _cutOff; apagado = (cutOff < 0.00001 || resonance <= 0.015) ; }
    //void setCutOff(float _cutOff) {cutOff = _cutOff; apagado = (cutOff < 0.1 || resonance <= 0.01) ; }
    void setResonance(float _resonance) { resonance = _resonance; apagado = (cutOff < 0.00001 || resonance <= 0.015) ;}
    //void setNotaMidi(uint8_t nota) { ratioNota = (AG_Util::getFrecuencia(nota) / AG_Util::getFrecuencia(64)) * MAX_FREQ_CUTOFF * PI_MEDIO / SAMPLE_RATE; };
    void setNotaMidi(uint8_t nota) { ratioNota = nota * PI_MEDIO * MAX_FREQ_CUTOFF / NOTA_BASE_FILTRO / SAMPLE_RATE; };
    void setTipo(TipoFiltro _tipo) { tipo = _tipo; }
    TipoFiltro getTipo () { return tipo; }
    void debug(); 

private:
    float cutOff;
    float resonance;
    float ratioNota;
    float aNorm[2];
    float bNorm[3];
    float w[2];
    uint32_t fSeno;
    uint32_t fCoseno;
    boolean apagado;
    TipoFiltro tipo;
};


#endif
