/*
	AG_Menu - Menu and control of LCD for AG_Sinte  
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

#include "AG_Menu.h"
#include "AG_Sintetizador.h"
#include <SPI.h>
#include <Ucglib.h>
#include <menu.h>
#include <ClickEncoder.h>
#include <menuIO/chainStream.h>
#include <menuIO/clickEncoderIn.h>
#include <menuIO/UCGLibOut.h>
#include <menuIO/serialOut.h>

using namespace Menu;
// Declare pins for rotary encoder
#define encA 33
#define encB 32
#define encBtn 14
#define encSteps 4

ClickEncoder clickEncoder = ClickEncoder(encA, encB, encBtn, encSteps);
ClickEncoderStream encStream(clickEncoder, 1);
MENU_INPUTS(in,&encStream);

#define UC_Width 320
#define UC_Height 240

//font size plus margins
#define fontX 17
#define fontY 26

Ucglib_ILI9341_18x240x320_HWSPI ucg(2, 5, 4);

unsigned long _utime;
int osc1,osc2, lfoWT=0;
float cicloPulso = 0.25;
uint8_t saltosSeno = 3;
uint8_t tipoFiltroGlobal = 0;
uint8_t tipoFiltroVoz = 0;
extern float delayToMix;
extern float delayFeedback;
uint8_t _poliVoces = 1;
uint8_t _centsDetune = 10;

float _coefLFOMod = 1;
float _coefLFOFrec = 0;
float _coefLFOCutoff = 0;
AG_Sintetizador* _sinte;

SELECT(osc1,menuOsc1,"Osc1",doNothing,noEvent,noStyle
  ,VALUE("Seno",0,doNothing,noEvent)
  ,VALUE("Cuadrada",1,doNothing,noEvent)
  ,VALUE("Sierra",2,doNothing,noEvent)
  ,VALUE("Pulso",3,doNothing,noEvent)
  ,VALUE("Triangular",4,doNothing,noEvent)
  ,VALUE("Seno discreto",5,doNothing,noEvent)
  ,VALUE("Ruido",6,doNothing,noEvent)
  ,VALUE("OFF",8,doNothing,noEvent)
);

SELECT(osc2,menuOsc2,"Osc2",doNothing,noEvent,noStyle
  ,VALUE("Seno",0,doNothing,noEvent)
  ,VALUE("Cuadrada",1,doNothing,noEvent)
  ,VALUE("Sierra",2,doNothing,noEvent)
  ,VALUE("Pulso",3,doNothing,noEvent)
  ,VALUE("Triangular",4,doNothing,noEvent)
  ,VALUE("Seno discreto",5,doNothing,noEvent)
  ,VALUE("Ruido",6,doNothing,noEvent)
  ,VALUE("OFF",8,doNothing,noEvent)
);


SELECT(lfoWT,menuLfoWT,"Onda",doNothing,noEvent,noStyle
  ,VALUE("Seno",0,doNothing,noEvent)
  ,VALUE("Cuadrada",1,doNothing,noEvent)
  ,VALUE("Sierra",2,doNothing,noEvent)
  ,VALUE("Pulso",3,doNothing,noEvent)
  ,VALUE("Triangular",4,doNothing,noEvent)
  ,VALUE("Seno discreto",5,doNothing,noEvent)
  ,VALUE("LFR",7,doNothing,noEvent)
  ,VALUE("Ruido",6,doNothing,noEvent)
  ,VALUE("OFF",8,doNothing,noEvent)
);

MENU(menuLfo,"LFO",doNothing,noEvent,wrapStyle
  ,SUBMENU(menuLfoWT)
  ,FIELD(_coefLFOMod,"Modulacion","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_coefLFOFrec,"Frecuencia","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_coefLFOCutoff,"Cutoff","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("< Volver")
);  

SELECT(tipoFiltroGlobal,menuFG,"Filtro Gen",doNothing,noEvent,noStyle
  ,VALUE("LOWPASS",0,doNothing,noEvent)
  ,VALUE("HIGHPASS",1,doNothing,noEvent)
);

SELECT(tipoFiltroVoz,menuFV,"Filtro Voz",doNothing,noEvent,noStyle
  ,VALUE("LOWPASS",0,doNothing,noEvent)
  ,VALUE("HIGHPASS",1,doNothing,noEvent)
);

MENU(menuOpciones,"Opciones",doNothing,noEvent,wrapStyle
  ,FIELD(cicloPulso,"Ciclo Pulso","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(saltosSeno,"Saltos Seno","",0,20,1,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(delayToMix,"Delay Mix","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(delayFeedback,"Delay Feedback","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("< Volver")
);  

MENU(mainMenu,"Menu principal",doNothing,noEvent,wrapStyle
  ,SUBMENU(menuOsc1)
  ,SUBMENU(menuOsc2)
  ,SUBMENU(menuLfo)
  ,SUBMENU(menuFG)
  ,SUBMENU(menuFV)
  ,SUBMENU(menuOpciones)
  ,FIELD(_poliVoces,"Voces unison","",1,5,1,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_centsDetune,"Cents detune","",0,50,10,5, Menu::doNothing, Menu::noEvent, Menu::noStyle)
);

//define colors
#define BLACK {0,0,0}
#define BLUE {0,0,255}
#define GRAY {128,128,128}
#define WHITE {255,255,255}
#define YELLOW {255,255,0}
#define RED {255,0,0}

const colorDef<rgb> my_colors[6] {
  {{GRAY,GRAY},{BLUE,GRAY,GRAY}},//bgColor
  {{WHITE,WHITE},{WHITE,WHITE,WHITE}},//fgColor
  {{WHITE,BLACK},{YELLOW,YELLOW,RED}},//valColor
  {{WHITE,BLACK},{WHITE,YELLOW,YELLOW}},//unitColor
  {{WHITE,GRAY},{BLACK,BLUE,WHITE}},//cursorColor
  {{WHITE,YELLOW},{BLUE,BLACK,BLACK}},//titleColor
};

#define offsetX 0
#define offsetY 0

MENU_OUTPUTS(out,MAX_DEPTH
  ,UCG_OUT(ucg,my_colors,fontX,fontY,offsetX,offsetY,{0,0,UC_Width/fontX,UC_Height/fontY})
  ,NONE
  //,SERIAL_OUT(Serial2)
);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

void Menu_setup(AG_Sintetizador* sinte)
{

  _sinte = sinte;
  clickEncoder.setAccelerationEnabled(false);
  clickEncoder.setDoubleClickEnabled(false); // Disable doubleclicks makes the response faster.  See: https://github.com/soligen2010/encoder/issues/6
 
  ucg.begin(UCG_FONT_MODE_TRANSPARENT);
  ucg.setFont(ucg_font_courB18_tf);//choose fized width font (monometric)
  ucg.clearScreen();
  ucg.setRotate270();
  ucg.setColor(255,255,255);
  ucg.setFontPosBottom();
  _utime = micros();
  pinMode(encBtn, INPUT_PULLUP);
  pinMode(encA, INPUT_PULLUP);
  pinMode(encB, INPUT_PULLUP);
}

void Menu_process(void)
{
  nav.poll();
  _sinte->setWaveFormOsc1(osc1);
  _sinte->setWaveFormOsc2(osc2);
  _sinte->setWaveFormLFO(lfoWT);
  _sinte->setCicloPulso(cicloPulso);
  _sinte->setSaltosSeno(saltosSeno);
  _sinte->setTipoFiltroGeneral(tipoFiltroGlobal);
  _sinte->setTipoFiltroVoz(tipoFiltroVoz);
  _sinte->setDestinoLFO(_coefLFOMod, _coefLFOFrec, _coefLFOCutoff);
  _sinte->setUnison(_poliVoces, _centsDetune);
  if ( micros() - _utime > 1000 ) {
      _utime = micros();
       clickEncoder.service();
  }
}


