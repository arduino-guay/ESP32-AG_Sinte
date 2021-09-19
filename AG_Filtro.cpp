/*
	AG_Filtro  
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

#include "AG_Filtro.h"
#include "AG_Util.h"

AG_Filtro::AG_Filtro(void)
{
    aNorm[0] = 0.0f;
    aNorm[1] = 0.0f;
    bNorm[0] = 1.0f;
    bNorm[1] = 0.0f;
    bNorm[2] = 0.0f;
    fSeno = (1ULL << 31) - 1;
    fCoseno = (1ULL << 30) - 1;
    tipo = LOWPASS;
}

void AG_Filtro::Reset()
{
    w[0] = 0.0f;
    w[1] = 0.0f;
}

void IRAM_ATTR AG_Filtro::CalculateCoeff()
{
    float cosOmega, omega, sinOmega, alpha, a[3], b[3];

    if (cutOff >= 1.0f)
    {
        omega = 1.0f;
    }
    else if (cutOff < 0.0025f)
    {
        omega = 0.0025f;
    }
    else
    {
        omega = cutOff;
    }

    uint32_t tmp1 = (uint32_t)(fSeno * omega); 
    cosOmega = AG_Util::seno(WAVEFORM_I( tmp1 + fCoseno ));
    sinOmega = AG_Util::seno(WAVEFORM_I( tmp1 ));

    switch (tipo)
    {
        case LOWPASS:
            alpha = sinOmega / (2.0 * resonance);
            b[1] = 1 - cosOmega;
            b[0] = b[1] / 2;
            b[2] = b[0];
            a[0] = 1 + alpha;
            a[1] = -2 * cosOmega;
            a[2] = 1 - alpha;
            break;
        
        case HIGHPASS:
            alpha = sinOmega / (2.0 * resonance);
            b[0] = (1 + cosOmega) / 2;
            b[1] = -(1 + cosOmega);
            b[2] = b[0];
            a[0] = 1 + alpha;
            a[1] = -2 * cosOmega;
            a[2] = 1 - alpha;
            break;
    }

    float factor = 1.0f / a[0];

    aNorm[0] = a[1] * factor;
    aNorm[1] = a[2] * factor;

    bNorm[0] = b[0] * factor;
    bNorm[1] = b[1] * factor;
    bNorm[2] = b[2] * factor;
}

float IRAM_ATTR AG_Filtro::Process(float const signal)
{
    if ( apagado ) 
    {
      return signal;
    }
    const float out = bNorm[0] * signal + w[0];
    w[0] = bNorm[1] * signal - aNorm[0] * out + w[1];
    w[1] = bNorm[2] * signal - aNorm[1] * out;
    return out;
}

void AG_Filtro::debug()
{
  Serial.printf("Apagado %s - Cutoff: %f - Reso: %f - aNorm: %f,%f - bNorm: %f,%f,%f - w: %f,%f\n", apagado? "Si":"No" , cutOff, resonance, aNorm[0], aNorm[1], bNorm[0], bNorm[1], bNorm[2], w[0], w[1]); 
}

