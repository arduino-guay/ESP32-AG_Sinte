/*
	AG_Wavetable - Wavetable class for AG_Sinte  
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

#ifndef AG_WAVETABLE_H
#define AG_WAVETABLE_H

#include "AG_Util.h"

class AG_WaveTable
{
public:
   inline virtual float getValor(uint32_t i) {}
};

class AG_WTSeno : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return AG_Util::seno(i); }
};

class AG_WTLFR : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return AG_Util::LFR(i); }
};

class AG_WTSenoDiscreto : public AG_WaveTable
{
public:
   AG_WTSenoDiscreto() { AG_Util::setSaltosSenoDiscreto(3); }
   inline void setSaltos(uint32_t _saltos)
   {
      if (_saltos < 2)
      {
         _saltos = 2;
      }
      _saltos++;
      AG_Util::setSaltosSenoDiscreto(_saltos);
   }

   inline float getValor(uint32_t i) { return AG_Util::senoDiscreto(i); }
};

class AG_WTSierra : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return (2.0f * ((float)i) / ((float)WAVEFORM_CNT)) - 1.0f; }
};

class AG_WTCuadrada : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return (i > (WAVEFORM_CNT / 2)) ? 1 : -1; }
};

class AG_WTPulso : public AG_WaveTable
{
public:
   inline AG_WTPulso() { ciclo = 0.25; }
   inline float getValor(uint32_t i) { return (i > (WAVEFORM_CNT * ciclo)) ? 1 : -1; }
   inline void setCiclo(float _ciclo) { ciclo = _ciclo; }

private:
   float ciclo; // 1 -- 100
};

class AG_WTTriangular : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return ((i > (WAVEFORM_CNT / 2)) ? (((4.0f * (float)i) / ((float)WAVEFORM_CNT)) - 1.0f) : (3.0f - ((4.0f * (float)i) / ((float)WAVEFORM_CNT)))) - 2.0f; }
};

class AG_WTRuidoBlanco : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return (random(1024) / 512.0f) - 1.0f; }
};

class AG_WTRuidoRosa : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i)
   {
      float tmp = (random(1024) / 512.0f) - 1.0f;
      float res = (tmp + vAnt1 + vAnt2) / 3;
      vAnt2 = vAnt1;
      vAnt1 = tmp;
      return res;
   }

private:
   float vAnt1, vAnt2 = 0;
};

class AG_WTOff : public AG_WaveTable
{
public:
   inline float getValor(uint32_t i) { return 0; }
};


#endif //AG_WAVETABLE_H

