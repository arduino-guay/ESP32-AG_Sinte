/*
  AG_Arpegiador - Arpegiador for  AG_Sinte
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

#ifndef AG_ARPEGIADOR_H
#define AG_ARPEGIADOR_H
#include "AG_Sintetizador.h"
#include <Arduino.h>
#include <iterator>
#include <list>

#define MILIS_NUEVOACORDE 500

class AG_Arpegiador
{
public:
  enum ModoArpegio
  {
    arriba = 0,
    abajo,
    arribaAbajoIn,
    arribaAbajoEx,
    comoSeHaTocado,
    aleatorio
  };

  AG_Arpegiador(AG_Sintetizador *_sinte);
  void noteOn(uint8_t nota);
  void noteOff(uint8_t nota);
  void Process();

  void setBPM(uint16_t _BPM)
  {
    BPM = _BPM;
    _calculaParametros();
  }
  void setPorcentajeNota(uint8_t porc) { milisNota = porc * milisBPM / 100; }
  void setOcatavas(uint8_t _octavas)
  {
    if (octavas != _octavas)
    {
      octavas = _octavas;
      octavaActual = octavaAsc ? 0 : octavas - 1;
    }
  }
  void setModo(ModoArpegio modo);
  void setOctavaAsc(bool _octavaAsc)
  {
    if (octavaAsc != _octavaAsc)
    {
      octavaAsc = _octavaAsc;
      octavaActual = octavaAsc ? 0 : octavas - 1;
    }
  }
  void setTipoNota(uint8_t _tipoNota)
  {
    tipoNota = _tipoNota;
    _calculaParametros();
  }
  void setMantener(bool _mantener);
  void setEncendido(bool _encendido)
  {
    encendido = _encendido;
    if ( !encendido ) 
    {
      notasATocar.clear();
    }
  }
  void setVelocidad(uint8_t _velocidad) { velocidad = _velocidad; }

private:
  AG_Sintetizador *sinte;
  void _pulsarNota();
  void _apagarNota();
  void _siguienteNota();
  void _crearListaNotas();
  void _calculaParametros();
  std::list<uint8_t> notasPulsadas;
  std::list<uint8_t> notasATocar;
  uint8_t velocidad;
  std::list<uint8_t>::iterator itNotaActual;
  std::list<uint8_t> vPatronRitmo;
  std::list<uint8_t>::iterator itPasoActual;
  uint8_t octavas = 1;
  short octavaActual = 0;
  bool octavaAsc = true;
  ModoArpegio modo;
  uint8_t notaActual;
  uint8_t notaSonando;
  uint16_t BPM;
  uint8_t tipoNota = 2;
  uint32_t milisUltAcorde;
  uint32_t milisBPM;
  uint32_t milisNota;
  uint32_t milisUltEvento;
  bool mantener;
  bool encendido;
};

#endif
