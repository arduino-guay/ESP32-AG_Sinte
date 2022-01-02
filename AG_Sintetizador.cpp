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

#include "AG_Sintetizador.h"

void IRAM_ATTR AG_Sintetizador::Process(float *left, float *right)
{
    
    outMono = 0;
    count += 1;
    float valLFO = lfo.getSiguienteValor();
    float modLFOFreq = 1 + param->getPitchBendValue() + valLFO * param->getCoefLFOFrec();   
    float modLFOCutoff = param->getCoefLFOCutoff();
    
    if (count % 50 == 0 &&  modLFOCutoff > 0)
    {
        modLFOCutoff = (param->getCutOffGen() + (valLFO + 1) / 2.0 * modLFOCutoff);
    }

    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        if (!voces[i].estaLibre())
        {
            voces[i].setModulacion(modLFOFreq);
            if ( modLFOCutoff > 0 )
            {
                voces[i].getFistro()->setCutOff(modLFOCutoff);
                voces[i].getFistro()->CalculateCoeff();
            }
            outMono += voces[i].Process(count);
        }
    }

    outMono *= param->getVolumenGen();
    outMono += outMono * param->getCoefLFOMod() * valLFO;

    *left = outMono;
    *right = outMono;
}

AG_Voz *AG_Sintetizador::getFreeVoice()
{
    unsigned long timeRelease = millis();
    AG_Voz *voice;
    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        if (voces[i].estaLibre())
        {
            return &voces[i];
        }
        // Guardamos la más antigua en release por si no ahy libres
        if ((voces[i].getTRelease() > 0) && (voces[i].getTRelease() < timeRelease))
        {
            timeRelease = voces[i].getTRelease();
            voice = &voces[i];
        }
    }
    return voice;
}

void AG_Sintetizador::Init(AG_Param* _param)
{
    AG_Util::Init();
    param = _param;
    param->Init(&lfo);
    lfo.setFrecuencia(0.5f);
    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        voces[i].init(_param);
    }
}

void AG_Sintetizador::NoteOn(uint8_t ch, uint8_t note, float vel)
{
    lfo.reset();
    for (uint8_t y = 0; y < param->getVocesUnison() ; y++)
    {
        AG_Voz *voice = getFreeVoice();
        voice->setPortamento(param->getPortamento());
        voice->setCentsDetuneUnison(y * param->getCentsDetune());
        voice->NoteOn(note, vel);
    }
}

void AG_Sintetizador::NoteOff(uint8_t ch, uint8_t note)
{
    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        voces[i].NoteOff(note);
    }
}
