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

#include "AG_Voz.h"

AG_Voz::AG_Voz() 
{
    activa = false;
    valor = 0;
    adsr = new ADSR();
    adsrF = new ADSR();
    osc1 = new AG_Oscilador();
    osc2 = new AG_Oscilador();
    fistro = new AG_Filtro();
}

float IRAM_ATTR AG_Voz::Process(uint32_t tics, float noise_signal)
{
    valor = noise_signal;
    valor += osc1->getSiguienteValor();
    valor += osc2->getSiguienteValor();

    if (tics % TICS_ADSR_VOZ == 0)
    {
        adsr->process();
        activa = adsr->isActive();
        if ( !activa  )
        {
            tRelease = 0;
        }
        adsrF->process();
    }

    valor *= adsr->getValue() * velocidad;

    if (tics % TICS_ADSR_FILTRO == 0)
    {
        fistro->setCutOff(adsrF->getValue());
        fistro->CalculateCoeff();
    }

    valor = fistro->Process(valor);
    
    return valor;
}

void AG_Voz::NoteOff(uint8_t canal, uint8_t nota)
{
    if (activa && notaMidi == nota)
    {
        adsr->gate(false);
        adsrF->gate(false);
        tRelease = millis();
    }
}

void AG_Voz::NoteOn(uint8_t canal, uint8_t nota, float vel, adsrParam pAdsrV, adsrParam pAdsrF, float fResonance, AG_Filtro::TipoFiltro tipo)
{
 
    notaMidi = nota;
    velocidad = vel;
    valor = 0;
    adsr->setAll(pAdsrV.a, pAdsrV.d, pAdsrV.s, pAdsrV.r);
    adsr->reset();

    adsrF->setAll(pAdsrF.a, pAdsrF.d, pAdsrF.s, pAdsrF.r);
    adsrF->reset();

    fistro->setResonance(fResonance);
    fistro->setCutOff(0);
    fistro->setTipo(tipo);    

    fistro->Reset();
    valor = fistro->Process(valor);
    valor = fistro->Process(valor);
    valor = fistro->Process(valor);

    adsr->gate(true);
    adsrF->gate(true);
    activa = true;
}
