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

class AG_Filtro
{
public:
    enum TipoFiltro
        {
            LOWPASS = 0,
            HIGHPASS
        };
    const TipoFiltro TiposFiltro[2] = { LOWPASS, HIGHPASS };
    AG_Filtro(void);
    void CalculateCoeff();
    float Process(float const signal);
    void Reset();
    float getCutOff() { return cutOff; }
    float getResonace() { return resonance; }
    void setCutOff(float _cutOff) { _cutOff >= 1.0 ? cutOff = 0.99: cutOff = _cutOff; apagado = (cutOff < 0.00001 || resonance <= 0.5) ; }
    void setResonance(float _resonance) { resonance = _resonance; apagado = (cutOff < 0.00001 || resonance <= 0.5) ;}
    void setTipo(TipoFiltro _tipo) { tipo = _tipo; }

private:
    float cutOff;
    float resonance;
    float aNorm[2];
    float bNorm[3];
    float w[3];
    uint32_t fSeno;
    uint32_t fCoseno;
    boolean apagado;
    TipoFiltro tipo;
};


#endif
