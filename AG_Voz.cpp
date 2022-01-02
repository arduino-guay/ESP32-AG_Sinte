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
#include "AG_Param.h"

AG_Voz::AG_Voz() 
{
    activa = false;
    valor = 0;
    adsr = new ADSR();
    adsrF = new ADSR();
    osc1 = new AG_Oscilador();
    osc2 = new AG_Oscilador();
    fistroDinamico = new AG_Filtro();
    fistroGral= new AG_Filtro();
    fistroGral->setTipo(AG_Filtro::LOWPASS);

    portamento = false;
    adsrPorta = new ADSR();
    adsrPorta->setTargetRatioA(200);
    adsrPorta->setAttackRate(0);
}

void AG_Voz::init(AG_Param* _param) 
{ 
    param = _param; 
    osc1->setWfOsc(param->getWaveFormOsc1());
    osc2->setWfOsc(param->getWaveFormOsc2());
}

float IRAM_ATTR AG_Voz::Process(uint32_t tics)
{   
    if ( fm )
    {
        osc1->setModulacion(1 + osc2->getSiguienteValor());
        valor = osc1->getSiguienteValor();
    } else 
    {
        valor = osc1->getSiguienteValor();
        valor += osc2->getSiguienteValor();
    }

    if (tics % TICS_ADSR_VOZ == 0)
    {
        if ( portamento ) {
          adsrPorta->process();
          osc1->setFrecuencia(anteriorNota, notaMidi, adsrPorta->getValue());
          osc2->setFrecuencia(anteriorNota, notaMidi, adsrPorta->getValue());
        }
        
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
        fistroDinamico->setCutOff(adsrF->getValue());
        fistroDinamico->CalculateCoeff();
    }

    valor = fistroGral->Process(valor);
    valor = fistroDinamico->Process(valor);
    
    return valor;
}

void AG_Voz::NoteOff(uint8_t nota)
{
    if (activa && notaMidi == nota)
    {
        adsr->gate(false);
        adsrF->gate(false);
        tRelease = millis();
    }
}

void AG_Voz::NoteOn(uint8_t nota, float vel)
{
    anteriorNota = notaMidi;
    notaMidi = nota;
    velocidad = vel;
    valor = 0;
    
    setFM(param->getFM());
    osc1->setWfOsc(param->getWaveFormOsc1());
    osc2->setWfOsc(param->getWaveFormOsc2());
    osc2->setVolumen(param->getVolOsc2());
    osc2->setIncNota(param->getIncNotaOsc2());
    fistroGral->setCutOff(param->getCutOffGen());
    fistroGral->setResonance(param->getRresoGen());
    fistroGral->setTipo(param->getTipoFiltroGen());

    if ( portamento ) 
    {
      osc1->setNotaMidi(anteriorNota, 0, centsDetuneUnison);
      osc2->setNotaMidi(anteriorNota, 0, centsDetuneUnison);
    } else 
    {
      osc1->setNotaMidi(nota, 0, centsDetuneUnison);
      osc2->setNotaMidi(nota, 0, centsDetuneUnison);
    }
    adsr->setAll(param->getADSRVolumen());
    adsr->reset();

    adsrF->setAll(param->getADSRFiltro());
    adsrF->reset();

    adsrPorta->reset();

    fistroDinamico->Reset();
    fistroDinamico->setResonance(param->getResoFiltVoz());
    fistroDinamico->setNotaMidi(nota);
    fistroDinamico->setCutOff(0);
    fistroDinamico->setTipo(param->getTipoFiltroVoz());    

    fistroGral->Reset();
    fistroGral->setNotaMidi(nota);
    fistroGral->CalculateCoeff();

    adsr->gate(true);
    adsrF->gate(true);
    adsrPorta->gate(true);
    activa = true;
}
