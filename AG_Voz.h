/*
	AG_VOZ - Voice class for AG_Sinte  
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

#ifndef AG_VOZ_H_
#define AG_VOZ_H_

#include "AG_Oscilador.h"
#include "AG_Filtro.h"
#include "ADSR.h"

#define MAX_OSC_VOZ 10
#define TICS_ADSR_VOZ 4
#define TICS_ADSR_FILTRO 32

class AG_Voz
{
public:
    
    AG_Voz(void);
    void NoteOn(uint8_t nota, float vel, adsrParam pAdsrV, adsrParam pAdsrF, float fResonance, AG_Filtro::TipoFiltro tipo);
    inline void setModulacion(float _pitchMultiplier)
    {
        osc1->setModulacion(_pitchMultiplier);
        osc2->setModulacion(_pitchMultiplier);
    }
    void NoteOff(uint8_t nota);
    float Process(uint32_t tics, float noise_signal);
    float getValor() { return valor; }
    boolean estaLibre() { return !activa; }
    uint8_t getNotaMidi() { return notaMidi; }
    inline AG_Oscilador* getOsc1() { return osc1; }
    inline AG_Oscilador* getOsc2() { return osc2; }
    void setWfOsc1 (AG_WaveTable* _wfOsc) { wfOsc1 = _wfOsc; }
    void setWfOsc2 (AG_WaveTable* _wfOsc) { wfOsc2 = _wfOsc; }
    void setCentsDetuneUnison(uint8_t _centsDetuneUnison) { centsDetuneUnison = _centsDetuneUnison;}
    unsigned long getTRelease() { return tRelease; } 
    void setPortamento(uint16_t value) { portamento = value > 0; adsrPorta->setAttackRate(value); } 

private:
    float velocidad;
    uint8_t notaMidi;
    uint8_t anteriorNota;
    boolean activa;
    ADSR *adsr;
    ADSR *adsrF;
    ADSR *adsrPorta;
    AG_Filtro *fistro;
    AG_Oscilador *osc1;
    AG_Oscilador *osc2;
    AG_WaveTable *wfOsc1;
    AG_WaveTable *wfOsc2;
    uint8_t centsDetuneUnison;
    float valor;
    unsigned long tRelease;
    boolean portamento;
};

#endif
