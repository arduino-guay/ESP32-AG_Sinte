
#include "AG_Param.h"

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

void AG_Param::Init(AG_Oscilador* _lfo)
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
    lfo = _lfo;
    lfo->setWfOsc(&seno);
    wfOsc1 = &seno;
    wfOsc2 = &seno;
}

void AG_Param::setWaveFormOsc1(uint8_t selWaveForm)
{
    wfOsc1 = tablaOndas[selWaveForm];
}

void AG_Param::setWaveFormOsc2(uint8_t selWaveForm)
{
    wfOsc2 = tablaOndas[selWaveForm];
}

void AG_Param::setWaveFormLFO(uint8_t selWaveForm)
{
    lfo->setWfOsc(tablaOndas[selWaveForm]);
}

void AG_Param::setCicloPulso(float ciclo)
{
    pulso.setCiclo(ciclo);
}

void AG_Param::setSaltosSeno(uint8_t saltosSeno)
{
    senoDiscreto.setSaltos(saltosSeno);
}


float AG_Param::synth_Log(float value, float maxSeg)
{
    return pow(value, 5) * maxSeg * (SAMPLE_RATE / TICS_ADSR_VOZ);
}

void AG_Param::setTipoFiltroGeneral(uint8_t tipo)
{
    tipoFiltroGen = static_cast<AG_Filtro::TipoFiltro>(tipo);
}

void AG_Param::setTipoFiltroVoz(uint8_t tipo)
{
    tipoFiltroVoz = static_cast<AG_Filtro::TipoFiltro>(tipo);
}

void AG_Param::setDestinoLFO(float mod, float frec, float cutoff)
{
    coefLFOMod = 1.5 * mod;
    coefLFOFrec = frec * 2;
    coefLFOCutoff = cutoff;
}

void AG_Param::setUnison(uint8_t _voces, uint8_t _centsDetuneUnison)
{
    vocesUnison = _voces;
    centsDetuneUnison = _centsDetuneUnison;
}

void AG_Param::setUltimoValor(float valor, boolean forzado)
{
    if (!forzado)
    {
        ultimoValor = valor;
    }
}

void AG_Param::setPortamento(float _portamento)
{
    portamento = synth_Log(_portamento, 1);
    setUltimoValor(portamento * TICS_ADSR_VOZ / SAMPLE_RATE, false);
}

void AG_Param::setModWheel(uint8_t ch, float _wheel)
{
    wheel = _wheel;
    lfo->setVolumen(volLFO + wheel); 
    setUltimoValor(volLFO + wheel, false);
}

void AG_Param::setParam(uint8_t slider, float value, boolean forzado)
{
    int octavas;
    float ratio;
    switch (slider)
    {
    case SYNTH_PARAM_VOL_ENV_ATTACK:
        adsrVolumen.a = synth_Log(value, 5);
        setUltimoValor(adsrVolumen.a / SAMPLE_RATE, forzado);
        break;
    case SYNTH_PARAM_VOL_ENV_DECAY:
        adsrVolumen.d = synth_Log(value, 5);
        setUltimoValor(adsrVolumen.d / SAMPLE_RATE, forzado);
        break;
    case SYNTH_PARAM_VOL_ENV_SUSTAIN:
        adsrVolumen.s = value;
        setUltimoValor(adsrVolumen.s, forzado);
        break;
    case SYNTH_PARAM_VOL_ENV_RELEASE:
        adsrVolumen.r = synth_Log(value, 3);
        setUltimoValor(adsrVolumen.r / SAMPLE_RATE, forzado);
        break;
    case SYNTH_PARAM_FIL_ENV_ATTACK:
        adsrFiltro.a = synth_Log(value, 5);
        setUltimoValor(adsrFiltro.a / SAMPLE_RATE, forzado);
        // Serial.printf("Attack %f\n", adsrFiltro.a);
        break;
    case SYNTH_PARAM_FIL_ENV_DECAY:
        adsrFiltro.d = synth_Log(value, 5);
        setUltimoValor(adsrFiltro.d / SAMPLE_RATE, forzado);
        break;
    case SYNTH_PARAM_FIL_ENV_SUSTAIN:
        adsrFiltro.s = value;
        setUltimoValor(adsrFiltro.s, forzado);
        // Serial.printf("Cutoff %f\n", cutOffGen);
        break;
    case SYNTH_PARAM_FIL_ENV_RELEASE:
        adsrFiltro.r = synth_Log(value, 3);
        setUltimoValor(adsrFiltro.s / SAMPLE_RATE, forzado);
        break;
    case SYNTH_PARAM_OFFSET_OSC2:
        volOsc2 = value;
        setUltimoValor(volOsc2, forzado);
        break;
    case SYNTH_PARAM_PITCH_OSC2:
        // Movemos entre +/- 5 octavas
        if (fm)
        {
            ratio = pow(2, (value * 10) - 5);
            incNotaOsc2 = log2(ratio) * 12;
            setUltimoValor(ratio, forzado);
        }
        else
        {
            incNotaOsc2 = ((uint8_t)(value * 10) - 5) * 12;
            setUltimoValor(incNotaOsc2, forzado);
        }
        // Serial.printf("Ratio %f\n", ratio);
        break;
    case SYNTH_PARAM_MAIN_FILT_CUTOFF:
        cutOffGen = (value * value * value) / 2;
        // Serial.printf("Cutoff %f\n", cutOffGen);
        setUltimoValor(cutOffGen * MAX_FREQ_CUTOFF, forzado);
        break;
    case SYNTH_PARAM_MAIN_FILT_RESO:
        resoGen = 0.01f + 20 * value * value * value;
        // Serial.printf("Resonancia %f\n", 0.5f + 10 * value * value * value);
        // fistroGlobal.debug();
        setUltimoValor(resoGen, forzado);
        break;
    case SYNTH_PARAM_VOICE_FILT_RESO:
        resoFiltVoz = 0.01f + 20 * value * value * value;
        setUltimoValor(resoFiltVoz, forzado);
        break;
    case SYNTH_PARAM_VOICE_NOISE_LEVEL:
        // soundNoiseLevel = value;
        fm = value > 0.5;
        setUltimoValor(fm ? 1 : 0, forzado);
        break;
    case SYNTH_PARAM_LFO_DEPTH:
        volLFO = value;
        lfo->setVolumen(volLFO + wheel);
        setUltimoValor(volLFO, forzado);
        break;
    case SYNTH_PARAM_LFO_RATE:
        freqLFO = value * 10;
        lfo->setFrecuencia(freqLFO);
        setUltimoValor(freqLFO, forzado);
        break;
    case SYNTH_PARAM_VOLUMEN:
        volumenGen = pow(5, value - 1) - 0.2;
        // setUltimoValor(volumenGen,forzado);
        break;
    default:
        /* not connected */
        break;
    }
}
