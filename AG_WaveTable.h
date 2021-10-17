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
   virtual IRAM_ATTR float getValor(uint32_t i) {}
};

class AG_WTSeno : public AG_WaveTable
{
public:
   float IRAM_ATTR getValor(uint32_t i) { return AG_Util::seno(i); }
};

class AG_WTLFR : public AG_WaveTable
{
public:
   float IRAM_ATTR getValor(uint32_t i) { return AG_Util::LFR(i); }
};

class AG_WTSenoDiscreto : public AG_WaveTable
{
public:
   AG_WTSenoDiscreto() { AG_Util::setSaltosSenoDiscreto(3); }
   void setSaltos(uint32_t _saltos)
   {
      if (_saltos < 2)
      {
         _saltos = 2;
      }
      _saltos++;
      AG_Util::setSaltosSenoDiscreto(_saltos);
   }

   float IRAM_ATTR getValor(uint32_t i) { return AG_Util::senoDiscreto(i); }
};

class AG_WTSierra : public AG_WaveTable
{
public:
   float getValor(uint32_t i);
};

class AG_WTCuadrada : public AG_WaveTable
{
public:
   float getValor(uint32_t i);
};

class AG_WTPulso : public AG_WaveTable
{
public:
   AG_WTPulso() { ciclo = 0.25; }
   float getValor(uint32_t i);
   void setCiclo(float _ciclo) { ciclo = _ciclo; }

private:
   float ciclo; // 1 -- 100
};

class AG_WTTriangular : public AG_WaveTable
{
public:
   float getValor(uint32_t i);
};

class AG_WTRuidoBlanco : public AG_WaveTable
{
public:
   float getValor(uint32_t i);
};

class AG_WTAKWF1 : public AG_WaveTable
{
public:
   float IRAM_ATTR getValor(uint32_t i) { return AG_Util::akwf1(i); }
};

class AG_WTAKWF2 : public AG_WaveTable
{
public:
   float IRAM_ATTR getValor(uint32_t i) { return AG_Util::akwf2(i); }
};


class AG_WTOff : public AG_WaveTable
{
public:
   float getValor(uint32_t i);
};


#endif //AG_WAVETABLE_H

