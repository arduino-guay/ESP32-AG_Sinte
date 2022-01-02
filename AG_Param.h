/*
    AG_aram - Parameter Interface for  AG_Sinte
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

#ifndef AG_PARAM_H
#define AG_PARAM_H

/*
 * Valores de los indices para Synth_SetParam
 */
#define SYNTH_PARAM_VOL_ENV_ATTACK 5
#define SYNTH_PARAM_VOL_ENV_DECAY 6
#define SYNTH_PARAM_VOL_ENV_SUSTAIN 7
#define SYNTH_PARAM_VOL_ENV_RELEASE 8

#define SYNTH_PARAM_FIL_ENV_ATTACK 12
#define SYNTH_PARAM_FIL_ENV_DECAY 13
#define SYNTH_PARAM_FIL_ENV_SUSTAIN 14
#define SYNTH_PARAM_FIL_ENV_RELEASE 15
#define SYNTH_PARAM_MAIN_FILT_CUTOFF 10
#define SYNTH_PARAM_MAIN_FILT_RESO 11

#define SYNTH_PARAM_VOICE_FILT_RESO 9
#define SYNTH_PARAM_NOISE_LEVEL 4
#define SYNTH_PARAM_LFO_DEPTH 1
#define SYNTH_PARAM_LFO_RATE 0

#define SYNTH_PARAM_PITCH_OSC2 2
#define SYNTH_PARAM_OFFSET_OSC2 3

#define SYNTH_PARAM_VOLUMEN 20

#define SYNTH_PARAM_WAVEFORM_1 100
#define SYNTH_PARAM_WAVEFORM_2 101
#define SYNTH_PARAM_VOICE_NOISE_LEVEL 4

#define TICS_ADSR_VOZ 4
#define TICS_ADSR_FILTRO 32

#include "ADSR.h"
#include "AG_WaveTable.h"
#include "AG_Filtro.h"
#include "AG_Oscilador.h"

class AG_Param
{
private:
    AG_WaveTable *wfOsc1;
    AG_WaveTable *wfOsc2;
    adsrParam adsrFiltro;
    adsrParam adsrVolumen;
    float volumenGen = 0.05;

    // Param Osc2
    int incNotaOsc2 = 0;
    float volOsc2 = 0;

    // Param Unison
    uint8_t vocesUnison = 2;
    uint8_t centsDetuneUnison = 10;

    // param LFO
    float coefLFOMod = 1.0f;
    float coefLFOFrec = 0;
    float coefLFOCutoff = 0;
    float volLFO = 0;
    float freqLFO = 0;

    float pitchBendValue = 0;
    float wheel = 0;

    // Filtros
    float resoFiltVoz;
    float cutOffGen;
    float resoGen;
    AG_Filtro::TipoFiltro tipoFiltroVoz;
    AG_Filtro::TipoFiltro tipoFiltroGen;

    float portamento = 0.0f;

    float synth_Log(float value, float maxSeg);
    void setUltimoValor(float valor, boolean forzado);

    float ultimoValor = 0;
    boolean fm = false;
    AG_Oscilador *lfo;

public:
    void Init(AG_Oscilador *_lfo);
    void setPitchBend(uint8_t ch, float bend) { pitchBendValue = bend; }
    void setModWheel(uint8_t ch, float _wheel);
    void setWaveFormOsc1(uint8_t selWaveForm);
    void setWaveFormOsc2(uint8_t selWaveForm);
    AG_WaveTable *getWaveFormOsc1() { return wfOsc1; }
    AG_WaveTable *getWaveFormOsc2() { return wfOsc2; }
    void setWaveFormLFO(uint8_t selWaveForm);
    void setCicloPulso(float ciclo); 
    void setSaltosSeno(uint8_t saltosSeno);
    void setParam(uint8_t slider, float value, boolean forzado = false);
    void setTipoFiltroGeneral(uint8_t tipo);
    void setTipoFiltroVoz(uint8_t tipo);
    void setDestinoLFO(float mod, float frec, float cutoff);
    void setUnison(uint8_t _voces, uint8_t _centsDetune);
    void setAKWF1(uint8_t valor);
    void setAKWF2(uint8_t valor);
    void setPortamento(float _portamento);
    float getUltimoValor() { return ultimoValor; }

    uint8_t getVocesUnison() { return vocesUnison; }
    uint8_t getCentsDetune() { return centsDetuneUnison; }
    int getIncNotaOsc2() { return incNotaOsc2; }
    float getVolOsc2() { return volOsc2; }
    float getPortamento() { return portamento; }
    float getCutOffGen() { return cutOffGen; }
    float getRresoGen() { return resoGen; }
    float getResoFiltVoz() { return resoFiltVoz; }
    AG_Filtro::TipoFiltro getTipoFiltroGen() { return tipoFiltroGen; }
    AG_Filtro::TipoFiltro getTipoFiltroVoz() { return tipoFiltroVoz; }
    float getPitchBendValue() { return pitchBendValue; }
    float getCoefLFOFrec() { return coefLFOFrec; }
    float getCoefLFOCutoff() { return coefLFOCutoff; }
    float getCoefLFOMod() { return coefLFOMod; } 
    float getVolumenGen() { return volumenGen; }
    adsrParam* getADSRFiltro() { return &adsrFiltro; }
    adsrParam* getADSRVolumen() { return &adsrVolumen; }

    boolean getFM() { return fm; }
};

#endif
