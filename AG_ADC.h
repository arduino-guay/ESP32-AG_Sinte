/*
	AG_ADC - CD74HC4067 16-Channel Multiplexer interface for Arduino
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


#ifndef AGUAY_ADC_H
#define AGUAY_ADC_H

#include <Arduino.h>
#include "AG_Sintetizador.h"

#define MAX_ENTRADAS 16
#define BITS_RESOLUCION_ADC 9

#define S0 22
#define S1 12
#define S2 13
#define S3 21

#define ADC_PIN 15
#define ADC_VOLUMEN 35

#define RETARDO_ACTIVACION 1


class AG_ADC {
    
	public:
		AG_ADC(AG_Sintetizador* _sinte) { sinte = _sinte; }
		void comenzar();
		void procesa(boolean refrescar);

   private:
		float valorFloat(uint16_t valorInt);
		uint16_t maxValorADC;
		uint16_t antValores[MAX_ENTRADAS];
    AG_Sintetizador* sinte;
};


#endif //AGUAY_ADC_H

