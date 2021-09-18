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

#include "AG_ADC.h"

void AG_ADC::comenzar () {
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    analogReadResolution(BITS_RESOLUCION_ADC);
    maxValorADC = (1 << BITS_RESOLUCION_ADC)-1;
    for (uint8_t i = 0; i < MAX_ENTRADAS; i++) 
    {
        antValores[i] = 9999;
    }
}

float AG_ADC::valorFloat (uint16_t valorInt) {
    return (float)(maxValorADC - valorInt) / maxValorADC;
}

void AG_ADC::procesa (boolean refrescar) {
    sinte->setParam(SYNTH_PARAM_VOLUMEN, valorFloat(analogRead(ADC_VOLUMEN)));
    for (uint8_t i = 0; i < MAX_ENTRADAS; i++)
    {
        digitalWrite (S0, (i & 0x01) );   
        digitalWrite (S1, (i & 0x02) >> 1 );   
        digitalWrite (S2, (i & 0x04) >> 2 );   
        digitalWrite (S3, (i & 0x08) >> 3 );   
        delay(RETARDO_ACTIVACION);
        uint16_t valor = analogRead(ADC_PIN);
        if ( (abs(antValores[i] - valor) > 5) || refrescar ) {
            sinte->setParam(i, valorFloat(valor));
            antValores[i] = valor;
        }
        delay(RETARDO_ACTIVACION);
    }
}








