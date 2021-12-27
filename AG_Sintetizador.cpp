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

#define TIPOS_ONDA 10

AG_WTSeno seno = AG_WTSeno();
AG_WTSierra sierra = AG_WTSierra();
AG_WTCuadrada cuadrada = AG_WTCuadrada();
AG_WTPulso pulso = AG_WTPulso();
AG_WTTriangular triangular = AG_WTTriangular();
AG_WTRuidoBlanco ruidoBlanco = AG_WTRuidoBlanco();
AG_WTSenoDiscreto senoDiscreto = AG_WTSenoDiscreto();
AG_WTLFR lfr = AG_WTLFR();
AG_WTAKWF1 wtAkwf1 = AG_WTAKWF1();
AG_WTAKWF2 wtAkwf2 = AG_WTAKWF2();

AG_WaveTable *tablaOndas[TIPOS_ONDA];

void cargarTablas()
{
    tablaOndas[0] = &seno;
    tablaOndas[1] = &cuadrada;
    tablaOndas[2] = &sierra;
    tablaOndas[3] = &pulso;
    tablaOndas[4] = &triangular;
    tablaOndas[5] = &senoDiscreto;
    tablaOndas[6] = &ruidoBlanco;
    tablaOndas[7] = &lfr;
    tablaOndas[8] = &wtAkwf1;
    tablaOndas[9] = &wtAkwf2;
}

void IRAM_ATTR AG_Sintetizador::Process(float *left, float *right)
{
    /* gerenate a noise signal */
    float noise_signal = (random(10000) / 5000.0f - 1.0f) * soundNoiseLevel;

    outMono = 0;
    count += 1;
    lfo.setVolumen(volLFO + wheel);
    float valLFO = lfo.getSiguienteValor();

    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        if (!voces[i].estaLibre())
        {
            voces[i].setModulacion(1 + pitchBendValue + valLFO * coefLFOFrec);
            if (count % 50 == 0 && coefLFOCutoff > 0)
            {
            voces[i].getFistro()->setCutOff((cutOffGen + (valLFO + 1) / 2.0 * coefLFOCutoff));
            voces[i].getFistro()->CalculateCoeff();
            }
            outMono += voces[i].Process(count, noise_signal);
        }
    }

    //outMono = fistroGlobal.Process(outMono);
    outMono *= volumenGen;
    outMono += outMono * coefLFOMod * valLFO;

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
        if (  (voces[i].getTRelease() > 0) && (voces[i].getTRelease() < timeRelease) )
        {
           timeRelease = voces[i].getTRelease();
           voice = &voces[i];
        }     
    }
    return voice;
}

void AG_Sintetizador::Init()
{
    AG_Util::Init();
    cargarTablas();
    wfOsc1 = &seno;
    wfOsc2 = &seno;
    lfo.setOnda(&seno);
    lfo.setFrecuencia(0.5f);
}

void AG_Sintetizador::NoteOn(uint8_t ch, uint8_t note, float vel)
{
    lfo.reset();
    for (uint8_t y = 0; y < vocesUnison; y++)
    {
        AG_Voz *voice = getFreeVoice();
        voice->setCentsDetuneUnison(y * centsDetuneUnison);
        voice->getOsc1()->setWfOsc(wfOsc1);
        voice->getOsc2()->setWfOsc(wfOsc2);
        voice->getOsc2()->setIncNota(incNotaOsc2);
        voice->getOsc2()->setVolumen(volOsc2);
        voice->getFistro()->setCutOff(cutOffGen);
        voice->getFistro()->setResonance(resoGen);
        voice->getFistro()->setTipo(tipoFiltroGen);
        voice->setPortamento(portamento);
        voice->NoteOn(note, vel, adsrVolumen, adsrFiltro, resoFiltVoz, tipoFiltroVoz);
    }
}

void AG_Sintetizador::NoteOff(uint8_t ch, uint8_t note)
{
    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        voces[i].NoteOff(note);
    }
}

void AG_Sintetizador::setWaveFormOsc1(uint8_t selWaveForm)
{
    wfOsc1 = tablaOndas[selWaveForm];
}

void AG_Sintetizador::setWaveFormOsc2(uint8_t selWaveForm)
{
    wfOsc2 = tablaOndas[selWaveForm];
}

void AG_Sintetizador::setWaveFormLFO(uint8_t selWaveForm)
{
    lfo.setOnda(tablaOndas[selWaveForm]);
}

void AG_Sintetizador::setCicloPulso(float ciclo)
{
    pulso.setCiclo(ciclo);
}

void AG_Sintetizador::setSaltosSeno(uint8_t saltosSeno)
{
    senoDiscreto.setSaltos(saltosSeno);
}

float AG_Sintetizador::synth_Log(float value, float maxSeg)
{
    return pow(value, 5) * maxSeg * (SAMPLE_RATE / TICS_ADSR_VOZ);
}

void AG_Sintetizador::setTipoFiltroGeneral(uint8_t tipo)
{
    tipoFiltroGen = static_cast<AG_Filtro::TipoFiltro>(tipo);
}

void AG_Sintetizador::setTipoFiltroVoz(uint8_t tipo)
{
    tipoFiltroVoz = static_cast<AG_Filtro::TipoFiltro>(tipo);
}

void AG_Sintetizador::setDestinoLFO(float mod, float frec, float cutoff)
{
    coefLFOMod = 1.5 * mod;
    coefLFOFrec = frec * 2;
    coefLFOCutoff = cutoff;
}

void AG_Sintetizador::setUnison(uint8_t _voces, uint8_t _centsDetuneUnison)
{
    vocesUnison = _voces;
    centsDetuneUnison = _centsDetuneUnison;
}

void AG_Sintetizador::setPortamento(float _portamento)
{
  portamento = synth_Log(_portamento,2);
}

void AG_Sintetizador::setParam(uint8_t slider, float value)
{
    int octavas;
    float ratio;
    switch (slider)
    {
    case SYNTH_PARAM_VOL_ENV_ATTACK:
        adsrVolumen.a = synth_Log(value, 5);
        break;
    case SYNTH_PARAM_VOL_ENV_DECAY:
        adsrVolumen.d = synth_Log(value, 5);
        break;
    case SYNTH_PARAM_VOL_ENV_SUSTAIN:
        adsrVolumen.s = value;
        break;
    case SYNTH_PARAM_VOL_ENV_RELEASE:
        adsrVolumen.r = synth_Log(value, 3);
        break;
    case SYNTH_PARAM_FIL_ENV_ATTACK:
        adsrFiltro.a = synth_Log(value, 5);
        //Serial.printf("Attack %f\n", adsrFiltro.a);
        break;
    case SYNTH_PARAM_FIL_ENV_DECAY:
        adsrFiltro.d = synth_Log(value, 5);
        break;
    case SYNTH_PARAM_FIL_ENV_SUSTAIN:
        adsrFiltro.s = value;
        break;
    case SYNTH_PARAM_FIL_ENV_RELEASE:
        adsrFiltro.r = synth_Log(value, 3);
        break;
    case SYNTH_PARAM_OFFSET_OSC2:
        volOsc2 = value;
        break;
    case SYNTH_PARAM_PITCH_OSC2:
        // Movemos entre +/- 5 octavas
        incNotaOsc2 = ((uint8_t)(value * 10) - 5) * 12;
        //ratio = pow(2,(value * 10) - 5);
        //incNotaOsc2 = log2(ratio) * 12;
        //Serial.printf("Ratio %f\n", ratio);
        break;
    case SYNTH_PARAM_MAIN_FILT_CUTOFF:
        cutOffGen = (value * value * value) / 2 ;
        //Serial.printf("Cutoff %f\n", cutOffGen);
        break;
    case SYNTH_PARAM_MAIN_FILT_RESO:
        resoGen = 0.01f + 20 * value * value * value;
        //Serial.printf("Resonancia %f\n", 0.5f + 10 * value * value * value);
        //fistroGlobal.debug();
        break;
    case SYNTH_PARAM_VOICE_FILT_RESO:
        resoFiltVoz = 0.01f + 20 * value * value * value;
        break;
    case SYNTH_PARAM_VOICE_NOISE_LEVEL:
        soundNoiseLevel = value;
        break;
    case SYNTH_PARAM_LFO_DEPTH:
        volLFO = value;
        break;
    case SYNTH_PARAM_LFO_RATE:
        lfo.setFrecuencia(value * 5);
        break;
    case SYNTH_PARAM_VOLUMEN:
        volumenGen = pow(5, value - 1) - 0.2;
        break;
    default:
        /* not connected */
        break;
    }
}
