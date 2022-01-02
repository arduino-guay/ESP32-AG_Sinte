#include "AG_Oscilador.h"

float IRAM_ATTR AG_Oscilador::getSiguienteValor()
{
    samplePos += (uint32_t)(pitchMultiplier * ((float)(addVal + addCents)));
    valor = waveForm->getValor(WAVEFORM_I(samplePos));
    return valor * volumen;
}

float IRAM_ATTR AG_Oscilador::getValor()
{
    return valor * volumen;
}

void IRAM_ATTR AG_Oscilador::setNotaMidi(uint8_t nota)
{
    if (nota + incNota < MIDI_NOTE_CNT)
    {
        addVal = AG_Util::saltosMidi(nota + incNota);
    }
    else
    {
        addVal = AG_Util::saltosMidi(nota);
    }
}

void IRAM_ATTR AG_Oscilador::setNotaMidi(uint8_t nota, uint32_t _samplePos, uint8_t cents)
{
    //if (nota + incNota < MIDI_NOTE_CNT)
    if (1)
    {
        addVal = AG_Util::saltosMidi(nota + incNota);
        addCents = AG_Util::incCents(nota + incNota, cents);
    }
    else
    {
        addVal = AG_Util::saltosMidi(nota);
        addCents = AG_Util::incCents(incNota, cents);
    }
    samplePos = _samplePos;
}

void IRAM_ATTR AG_Oscilador::setFrecuencia(float fHz)
{
    addVal = (uint32_t)(fHz * ((float)(1ULL << 32ULL) / ((float)SAMPLE_RATE)));
}

void IRAM_ATTR AG_Oscilador::setFrecuencia(uint8_t notaDesde, uint8_t notaHasta, float porc)
{
    float f1, f2;
    if (notaHasta + incNota < MIDI_NOTE_CNT)
    {
        f1 = AG_Util::getFrecuencia(notaDesde + incNota);
        f2 = AG_Util::getFrecuencia(notaHasta + incNota);
    }
    else
    {
        f1 = AG_Util::getFrecuencia(notaDesde);
        f2 = AG_Util::getFrecuencia(notaHasta);
    }
    setFrecuencia(f1 + porc * (f2 - f1));
}
