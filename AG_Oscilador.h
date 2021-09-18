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
    
class AG_Oscilador {
    public:
        AG_Oscilador() { enUso = false; volumen = 1.0; pitchMultiplier = 1; addCents = 0;}
        void setModulacion (float _pitchMultiplier) { pitchMultiplier = _pitchMultiplier; }
        float getSiguienteValor();
        float getValor() { return valor; }
        void setNotaMidi(uint8_t nota, AG_WaveTable *_waveForm, uint32_t _samplePos, uint8_t cents) ;
        void setOnda(AG_WaveTable *_waveForm) { waveForm = _waveForm; }
        void setFrecuencia(float fHz);
        void setEnUso (boolean _enUso) { enUso = _enUso; }
        void setVolumen (float _volumen) { volumen = _volumen; }
        boolean estaLibre() {return !enUso; }

    private:
        float pitchMultiplier;
        AG_WaveTable *waveForm;
        uint32_t samplePos;
        uint32_t addVal;
        uint32_t addCents;
        boolean enUso;
        float volumen;
        float valor;
};

inline void AG_Oscilador::setFrecuencia(float fHz)
{
    addVal = (uint32_t)(fHz * ((float)(1ULL << 32ULL) / ((float)SAMPLE_RATE)));
}

inline void AG_Oscilador::setNotaMidi(uint8_t nota, AG_WaveTable *_waveForm, uint32_t _samplePos, uint8_t cents)
{
    addVal = AG_Util::saltosMidi(nota);
    samplePos = _samplePos;
    waveForm = _waveForm;
    addCents = AG_Util::incCents(nota, cents);
}

inline float AG_Oscilador::getSiguienteValor()
{
    samplePos += (uint32_t)( pitchMultiplier * ((float) (addVal +  addCents)));
    valor = waveForm->getValor(WAVEFORM_I(samplePos));
    return valor * volumen;
}

#endif

