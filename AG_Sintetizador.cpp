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

#define TIPOS_ONDA 8

AG_WTSeno seno = AG_WTSeno();
AG_WTSierra sierra = AG_WTSierra();
AG_WTCuadrada cuadrada = AG_WTCuadrada();
AG_WTPulso pulso = AG_WTPulso();
AG_WTTriangular triangular = AG_WTTriangular();
AG_WTRuidoBlanco ruidoBlanco = AG_WTRuidoBlanco();
AG_WTSenoDiscreto senoDiscreto = AG_WTSenoDiscreto();
AG_WTLFR lfr = AG_WTLFR();

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
}

void IRAM_ATTR AG_Sintetizador::Process(float *left, float *right)
{
    /* gerenate a noise signal */
    float noise_signal = (random(1024) / 512.0f - 1.0f) * soundNoiseLevel;

    outMono = 0;
    count += 1;
    float valLFO = lfo.getSiguienteValor();

    if (count % 100 == 0)
    {
        //float pitchVar = pitchBendValue * (1.0 - lfo.getSiguienteValor());
        // pitchMultiplier = pow(2.0f, valLFO * 5 / 12.0f);
        if (coefLFOCutoff > 0)
        {
            fistroGlobal.setCutOff(cutOffGen + (valLFO + 1) / 2.0 * coefLFOCutoff);
            fistroGlobal.CalculateCoeff();
        }
    }

    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        if (!voces[i].estaLibre())
        {
            voces[i].setModulacion(1 + valLFO * coefLFOFrec);
            outMono += voces[i].Process(count, noise_signal);
        }
    }

    outMono = fistroGlobal.Process(outMono);
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
    for (uint8_t y = 0; y < vocesUnison; y++)
    {
        AG_Voz *voice = getFreeVoice();
        voice->getOsc1()->setNotaMidi(note, wfOsc1, 0, y * centsDetuneUnison);
        // Movemos la octava entre -5 y +5
        if (note + incNotaOsc2 < MIDI_NOTE_CNT)
        {
            voice->getOsc2()->setNotaMidi(note + incNotaOsc2, wfOsc2, 0, y * centsDetuneUnison);
            voice->getOsc2()->setVolumen(volOsc2);
        }
        voice->NoteOn(ch, note, vel, adsrVolumen, adsrFiltro, resoFiltVoz, tipoFiltro);
    }
}

void AG_Sintetizador::NoteOff(uint8_t ch, uint8_t note)
{
    for (int i = 0; i < MAX_POLY_VOICE; i++)
    {
        voces[i].NoteOff(ch, note);
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
    fistroGlobal.setTipo(fistroGlobal.TiposFiltro[tipo]);
}

void AG_Sintetizador::setTipoFiltroVoz(uint8_t tipo)
{
    tipoFiltro = fistroGlobal.TiposFiltro[tipo];
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

void AG_Sintetizador::setParam(uint8_t slider, float value)
{
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
        break;
    case SYNTH_PARAM_MAIN_FILT_CUTOFF:
        cutOffGen = value * value * value;
        fistroGlobal.setCutOff(value * value * value);
        //Serial.printf("Cutoff %f\n", cutOffGen);
        fistroGlobal.CalculateCoeff();
        //fistroGlobal.debug();
        break;
    case SYNTH_PARAM_MAIN_FILT_RESO:
        fistroGlobal.setResonance(0.5f + 10 * value * value * value);
        fistroGlobal.CalculateCoeff();
        //Serial.printf("Resonancia %f\n", 0.5f + 10 * value * value * value);
        //fistroGlobal.debug();
        break;
    case SYNTH_PARAM_VOICE_FILT_RESO:
        resoFiltVoz = 0.5f + 10 * value * value * value;
        break;
    case SYNTH_PARAM_VOICE_NOISE_LEVEL:
        soundNoiseLevel = value;
        break;
    case SYNTH_PARAM_LFO_DEPTH:
        lfo.setVolumen(value);
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
