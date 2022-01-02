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
#include "AG_Util.h"
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
int osc1, osc2, lfoWT, akwf1=0, akwf2 =1;
float cicloPulso = 0.25;
uint8_t saltosSeno = 3;
uint8_t tipoFiltroGlobal = 0;
uint8_t tipoFiltroVoz = 0;
extern float delayToMix;
extern float delayFeedback;
uint8_t _poliVoces = 1;
uint8_t _centsDetune = 10;
uint8_t _portamento = 0;
float ultimoValor = 0;

// Arpegiador
uint8_t _arpModo = 0;
uint8_t _arpMantener = 0;
uint8_t _arpTipo = 0;
AG_Arpegiador::ModoArpegio modos[6] = { 
    AG_Arpegiador::ModoArpegio::arriba, 
    AG_Arpegiador::ModoArpegio::abajo, 
    AG_Arpegiador::ModoArpegio::arribaAbajoIn, 
    AG_Arpegiador::ModoArpegio::arribaAbajoEx, 
    AG_Arpegiador::ModoArpegio::comoSeHaTocado, 
    AG_Arpegiador::ModoArpegio::aleatorio 
};
uint8_t _arpBPM = 60;
uint8_t _arpPorc = 70;
uint8_t _arpTipoNota = 4;
uint8_t _arpVolumen = 70;
uint8_t _arpOctavas = 1;

float _coefLFOMod = 1;
float _coefLFOFrec = 0;
float _coefLFOCutoff = 0;
AG_Param* _param;
AG_Arpegiador* _arpe;
AG_Midi* _midi;

SELECT(osc1,menuOsc1,"Osc1",doNothing,noEvent,noStyle
  ,VALUE("Seno",0,doNothing,noEvent)
  ,VALUE("Cuadrada",1,doNothing,noEvent)
  ,VALUE("Sierra",2,doNothing,noEvent)
  ,VALUE("Pulso",3,doNothing,noEvent)
  ,VALUE("Triangular",4,doNothing,noEvent)
  ,VALUE("Seno discreto",5,doNothing,noEvent)
  ,VALUE("Ruido",6,doNothing,noEvent)
  ,VALUE("AKWF1",8,doNothing,noEvent)
  ,VALUE("AKWF2",9,doNothing,noEvent)
);

SELECT(osc2,menuOsc2,"Osc2",doNothing,noEvent,noStyle
  ,VALUE("Seno",0,doNothing,noEvent)
  ,VALUE("Cuadrada",1,doNothing,noEvent)
  ,VALUE("Sierra",2,doNothing,noEvent)
  ,VALUE("Pulso",3,doNothing,noEvent)
  ,VALUE("Triangular",4,doNothing,noEvent)
  ,VALUE("Seno discreto",5,doNothing,noEvent)
  ,VALUE("Ruido",6,doNothing,noEvent)
  ,VALUE("AKWF1",8,doNothing,noEvent)
  ,VALUE("AKWF2",9,doNothing,noEvent)
);

SELECT(akwf1,menuAkwf1,"AKWF1",doNothing,noEvent,noStyle
  ,VALUE("Piano-1",0,doNothing,noEvent)
  ,VALUE("Piano-2",1,doNothing,noEvent)
  ,VALUE("Guitarra-1",2,doNothing,noEvent)
  ,VALUE("Guitarra-2",3,doNothing,noEvent)
  ,VALUE("Sax-1",4,doNothing,noEvent)
  ,VALUE("Sax-2",5,doNothing,noEvent)
  ,VALUE("Violin-1",6,doNothing,noEvent)
  ,VALUE("Violin-2",7,doNothing,noEvent)
  ,VALUE("Oboe-1",8,doNothing,noEvent)
  ,VALUE("Oboe-2",9,doNothing,noEvent)
  ,VALUE("Theremin-1",10,doNothing,noEvent)
  ,VALUE("Theremin-2",11,doNothing,noEvent)
);

SELECT(akwf2,menuAkwf2,"AKWF2",doNothing,noEvent,noStyle
  ,VALUE("Piano-1",0,doNothing,noEvent)
  ,VALUE("Piano-2",1,doNothing,noEvent)
  ,VALUE("Guitarra-1",2,doNothing,noEvent)
  ,VALUE("Guitarra-2",3,doNothing,noEvent)
  ,VALUE("Sax-1",4,doNothing,noEvent)
  ,VALUE("Sax-2",5,doNothing,noEvent)
  ,VALUE("Violin-1",6,doNothing,noEvent)
  ,VALUE("Violin-2",7,doNothing,noEvent)
  ,VALUE("Oboe-1",8,doNothing,noEvent)
  ,VALUE("Oboe-2",9,doNothing,noEvent)
  ,VALUE("Theremin-1",10,doNothing,noEvent)
  ,VALUE("Theremin-2",11,doNothing,noEvent)
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

MENU(menuFiltros,"Filtros",doNothing,noEvent,wrapStyle
  ,SUBMENU(menuFG)
  ,SUBMENU(menuFV)
  ,EXIT("< Volver")
);  

MENU(menuUnison,"Unison",doNothing,noEvent,wrapStyle
  ,FIELD(_poliVoces,"Voces unison","",1,5,1,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_centsDetune,"Cents detune","",0,50,10,5, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("< Volver")
);  

MENU(menuOpciones,"Opciones",doNothing,noEvent,wrapStyle
  ,FIELD(cicloPulso,"Ciclo Pulso","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(saltosSeno,"Saltos Seno","",0,20,1,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(delayToMix,"Delay Mix","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(delayFeedback,"Delay Feedback","",0,1,0.1,0.01, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_portamento,"Portamento","",0,100,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(menuAkwf1)
  ,SUBMENU(menuAkwf2)
  ,EXIT("< Volver")
);  

SELECT(_arpModo,menuArpgModo,"Modo",doNothing,noEvent,noStyle
  ,VALUE("Teclado",0,doNothing,noEvent)
  ,VALUE("Arpegiador",1,doNothing,noEvent)
  ,VALUE("Mezcla",2,doNothing,noEvent)
);

TOGGLE(_arpMantener,menuArpgMant,"Mantener: ",doNothing,noEvent,wrapStyle
    ,VALUE("SI",HIGH,doNothing,noEvent)
    ,VALUE("NO",LOW,doNothing,noEvent)
);

SELECT(_arpTipo,menuArpgTipo,"Tipo",doNothing,noEvent,noStyle
  ,VALUE("Asc",0,doNothing,noEvent)
  ,VALUE("Dec",1,doNothing,noEvent)
  ,VALUE("Asc/Des Inc",2,doNothing,noEvent)
  ,VALUE("Asc/Des Exc",3,doNothing,noEvent)
  ,VALUE("Tal cual",4,doNothing,noEvent)
  ,VALUE("Aleatorio",5,doNothing,noEvent)
);

SELECT(_arpTipoNota,menuArpgTipoNota,"Tipo",doNothing,noEvent,noStyle
  ,VALUE("1/4",1,doNothing,noEvent)
  ,VALUE("1/8",2,doNothing,noEvent)
  ,VALUE("1/16",4,doNothing,noEvent)
  ,VALUE("1/32",8,doNothing,noEvent)
);

MENU(menuArpegiador,"Arpegiador",doNothing,noEvent,wrapStyle
  ,SUBMENU(menuArpgModo) 
  ,SUBMENU(menuArpgMant) 
  ,SUBMENU(menuArpgTipo) 
  ,FIELD(_arpBPM,"BPM","",60,240,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(menuArpgTipoNota) 
  ,FIELD(_arpPorc,"% Nota","",0,100,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_arpVolumen,"Volumen","",0,100,10,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,FIELD(_arpOctavas,"Octavas","",1,4,1,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,EXIT("< Volver")
);  

MENU(mainMenu,"Menu principal",doNothing,noEvent,wrapStyle
  ,FIELD(ultimoValor,"Valor:","",0,10000,1,1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
  ,SUBMENU(menuOsc1)
  ,SUBMENU(menuOsc2)
  ,SUBMENU(menuLfo)
  ,SUBMENU(menuArpegiador)
  ,SUBMENU(menuFiltros)
  ,SUBMENU(menuUnison)
  ,SUBMENU(menuOpciones)
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

void Menu_setup(AG_Param* param, AG_Arpegiador* arpe, AG_Midi* midi)
{

  _param = param;
  _arpe = arpe;
  _midi = midi;
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
  ultimoValor = _param->getUltimoValor();
  nav.poll();
  AG_Util::setAKWF1(akwf1);
  AG_Util::setAKWF2(akwf2);
  _param->setWaveFormOsc1(osc1);
  _param->setWaveFormOsc2(osc2);
  _param->setWaveFormLFO(lfoWT);
  _param->setCicloPulso(cicloPulso);
  _param->setSaltosSeno(saltosSeno);
  _param->setTipoFiltroGeneral(tipoFiltroGlobal);
  _param->setTipoFiltroVoz(tipoFiltroVoz);
  _param->setDestinoLFO(_coefLFOMod, _coefLFOFrec, _coefLFOCutoff);
  _param->setUnison(_poliVoces, _centsDetune);
  _arpe->setMantener(_arpMantener);
  _arpe->setModo(modos[_arpTipo]);
  _arpe->setBPM(_arpBPM);
  _arpe->setPorcentajeNota(_arpPorc);
  _arpe->setTipoNota(_arpTipoNota);
  _arpe->setVelocidad(_arpVolumen);
  _arpe->setOcatavas(_arpOctavas);
  _midi->setModo(_arpModo);
  //_param->setPortamento(_portamento/100);
  if ( micros() - _utime > 1000 ) {
      _utime = micros();
       clickEncoder.service();
  }
}



