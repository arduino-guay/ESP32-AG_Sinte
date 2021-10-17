/*
	AG_Midi - Midi Interface for  AG_Sinte  
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

#include <MIDI.h>
#include "AG_Midi.h"

#define NORM127MUL  0.007874f

MIDI_CREATE_INSTANCE(HardwareSerial, Serial,     midiUSB);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2,    midiSerie);

AG_Sintetizador* AG_Midi::sinte;
AG_Arpegiador* AG_Midi::arp;
uint8_t AG_Midi::modo;

void AG_Midi::Process()
{
    midiUSB.read();
    midiSerie.read();
}

void AG_Midi::Setup()
{
    midiUSB.setHandleNoteOn(AG_Midi::NoteOn);
    midiUSB.setHandleNoteOff(AG_Midi::NoteOff);
    midiUSB.setHandleControlChange(AG_Midi::ControlChange);
    midiUSB.setHandlePitchBend(AG_Midi::PitchBend);
    midiUSB.begin(MIDI_CHANNEL_OMNI);    
    midiSerie.setHandleNoteOn(AG_Midi::NoteOn);
    midiSerie.setHandleNoteOff(AG_Midi::NoteOff);
    midiSerie.setHandleControlChange(AG_Midi::ControlChange);
    midiSerie.setHandlePitchBend(AG_Midi::PitchBend);
    midiSerie.begin(MIDI_CHANNEL_OMNI);    
}
