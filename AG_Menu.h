/*
	AG_Menu - Menu and control of LCD for AG_Sinte  
	Copyright (C) 2021, J. Daniel Herrera Gomariz

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

#ifndef AGUAY_MENU_H
#define AGUAY_MENU_H

#define UC_Width 320
#define UC_Height 240

//font size plus margins
#define fontX 17
#define fontY 26

#define MAX_DEPTH 3

//define colors
#define BLACK {0,0,0}
#define BLUE {0,0,255}
#define GRAY {128,128,128}
#define WHITE {255,255,255}
#define YELLOW {255,255,0}
#define RED {255,0,0}

#include "AG_Sintetizador.h"
#include "AG_Arpegiador.h"
#include "AG_Midi.h"

void Menu_setup(AG_Sintetizador* sinte, AG_Arpegiador* arpe, AG_Midi* midi);
void Menu_process(void);

#endif

