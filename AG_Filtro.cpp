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
    w[2] = 0.0f;
}

