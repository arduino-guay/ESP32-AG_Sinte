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
    void NoteOn(uint8_t canal, uint8_t nota, float vel, adsrParam pAdsrV, adsrParam pAdsrF, float fResonance, AG_Filtro::TipoFiltro tipo);
    inline void setModulacion(float _pitchMultiplier)
    {
        for (uint8_t i = 0; i < numOsc; i++)
        {
            osc[i]->setModulacion(_pitchMultiplier);
        }
    }
    void NoteOff(uint8_t canal, uint8_t nota);
    float Process(uint32_t tics, float noise_signal, float fResonance);
    void addOscilador(AG_Oscilador *_osc);
    float getValor() { return valor; }
    boolean estaLibre() { return !activa; }

private:
    float velocidad;
    uint8_t notaMidi;
    boolean activa;
    ADSR *adsr;
    ADSR *adsrF;
    AG_Filtro fistro;
    AG_Oscilador *osc[MAX_OSC_VOZ];
    uint8_t numOsc;
    float valor;
};

//[[gnu::optimize ("fast-math")]]
inline float AG_Voz::Process(uint32_t tics, float noise_signal, float fResonance)
{
    valor = noise_signal;
    for (uint8_t i = 0; i < numOsc; i++)
    {
        valor += osc[i]->getSiguienteValor();
    }

    if (tics % TICS_ADSR_VOZ == 0)
    {
        adsr->process();
        activa = adsr->isActive();
        if (!activa)
        {
            for (uint8_t i = 0; i < numOsc; i++)
            {
                osc[i]->setEnUso(false);
                osc[i] = nullptr;
            }
            numOsc = 0;
        }
        adsrF->process();
    }

    valor *= adsr->getValue() * velocidad;

    if (tics % TICS_ADSR_FILTRO == 0)
    {
        fistro.setResonance(fResonance);
        fistro.setCutOff(adsrF->getValue());
        fistro.CalculateCoeff();
    }

    valor = fistro.Process(valor);
    
    return valor;
}

#endif
