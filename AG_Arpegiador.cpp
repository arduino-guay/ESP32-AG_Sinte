#include "AG_Arpegiador.h"
#include <Arduino.h>

void AG_Arpegiador::_calculaParametros()
{
    milisBPM = 60000 / tipoNota / BPM;
}

void AG_Arpegiador::_siguienteNota()
{
    notaActual = (*itNotaActual) + (octavaActual * 12);
    itNotaActual++;
    if (itNotaActual == notasATocar.end())
    {
        if (octavaAsc)
        {
            octavaActual++;
            if (octavaActual >= octavas)
            {
                octavaActual = 0;
            }
        }
        else
        {
            octavaActual--;
            if (octavaActual < 0)
            {
                octavaActual = octavas - 1;
            }
        }
        if (modo == aleatorio)
        {
            _crearListaNotas();
        }
        itNotaActual = notasATocar.begin();
    }
}

void AG_Arpegiador::_crearListaNotas()
{
    notasATocar.clear();
    itNotaActual = notasATocar.begin();
    std::list<uint8_t> notasAsc;
    notasAsc.assign(notasPulsadas.begin(), notasPulsadas.end());
    notasAsc.sort();
    switch (modo)
    {
    case arriba:
        notasATocar.assign(notasAsc.begin(), notasAsc.end());
        break;
    case abajo:
        notasATocar.assign(notasAsc.rbegin(), notasAsc.rend());
        break;
    case arribaAbajoEx:
        notasATocar.assign(notasAsc.begin(), notasAsc.end());
        if (notasAsc.size() > 2)
        {
            notasAsc.reverse();
            notasAsc.pop_front();
            notasAsc.pop_back();
            notasATocar.insert(notasATocar.end(), notasAsc.begin(), notasAsc.end());
        }
        break;
    case arribaAbajoIn:
        notasATocar.assign(notasAsc.begin(), notasAsc.end());
        notasATocar.insert(notasATocar.end(), notasAsc.rbegin(), notasAsc.rend());
        break;
    case comoSeHaTocado:
        notasATocar.assign(notasPulsadas.begin(), notasPulsadas.end());
        break;
    case aleatorio:
        uint8_t notas[10];
        uint8_t i = 0;
        for (std::list<uint8_t>::iterator itNota = notasPulsadas.begin(); itNota != notasPulsadas.end(); ++itNota)
        {
            notas[i++] = *itNota;
        }
        for (uint8_t t = 0; t < notasPulsadas.size(); t++)
        {
            notasATocar.push_back(notas[random(notasPulsadas.size())]);
        }
        break;
    }
    itNotaActual = notasATocar.begin();
}

void AG_Arpegiador::_pulsarNota()
{
    notaSonando = notaActual;
    sinte->NoteOn(1, notaActual, velocidad / 100.0);
}

void AG_Arpegiador::_apagarNota()
{
    sinte->NoteOff(1, notaSonando);
    notaSonando = 0;
}

AG_Arpegiador::AG_Arpegiador(AG_Sintetizador *_sinte)
{
    sinte = _sinte;
    velocidad = 20;
    milisUltAcorde = millis();
    setBPM(120);
    setPorcentajeNota(50);
    setModo(comoSeHaTocado);
    setOcatavas(2);
    setOctavaAsc(true);
    setTipoNota(8);
    setMantener(false);
}

void AG_Arpegiador::noteOn(uint8_t nota)
{
    if (!encendido)
    {
        return;
    }
    if (mantener && ((millis() - milisUltAcorde) > MILIS_NUEVOACORDE))
    {
        _apagarNota();
        notasPulsadas.clear();
    }
    notasPulsadas.push_back(nota);
    _crearListaNotas();
    milisUltAcorde = millis();
}

void AG_Arpegiador::noteOff(uint8_t nota)
{
    if (!encendido)
    {
        return;
    }
    if (!mantener)
    {
        notasPulsadas.remove(nota);
        _crearListaNotas();
    }
}

void AG_Arpegiador::Process()
{
    if (!encendido)
    {
        return;
    }
    if (notaSonando)
    {
        if (millis() - milisUltEvento > milisNota)
        {
            _apagarNota();
        }
    }
    else if (millis() - milisUltEvento > milisBPM)
    {
        if (notasATocar.size() > 0)
        {
            _siguienteNota();
            _pulsarNota();
            milisUltEvento = millis();
        }
    }
}

void AG_Arpegiador::setModo(ModoArpegio _modo)
{
    if (_modo != modo)
    {
        modo = _modo;
        _crearListaNotas();
    }
}

void AG_Arpegiador::setMantener(bool _mantener)
{
    if (mantener != _mantener)
    {
        mantener = _mantener;
        if (!mantener)
        {
            notasPulsadas.clear();
        }
    }
}
