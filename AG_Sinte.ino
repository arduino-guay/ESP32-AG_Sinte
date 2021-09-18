/*
	AG_Sinte - Sintetizador analógico, estilo de los 80 basado en ESP-32
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

#include "config.h"

TaskHandle_t  Core0TaskHnd ;
/*
 * required include files
 */
#include <arduino.h>
#include <WiFi.h>
#include "AG_Sintetizador.h"
#include "AG_ADC.h"
#include "AG_menu.h"
#include "AG_Midi.h"

AG_Sintetizador El_Sinte;
AG_ADC adc = AG_ADC(&El_Sinte);
AG_Midi midi = AG_Midi(&El_Sinte);

uint32_t inicio;
uint32_t acum;

void setup()
{
    /*
     * this code runs once
     */
    Serial.begin(115200);
    delay(500);
    Menu_setup(&El_Sinte);
    
    Delay_Init();

    //Serial.printf("Initialize Synth Module\n");
    El_Sinte.Init();
    //Serial.printf("Initialize I2S Module\n");

 
    setup_i2s();
    //Serial.printf("Initialize Midi Module\n");
    
    //Serial.printf("Turn off Wifi/Bluetooth\n");
    WiFi.mode(WIFI_OFF);

    /*
    Serial.printf("ESP.getFreeHeap() %d\n", ESP.getFreeHeap());
    Serial.printf("ESP.getMinFreeHeap() %d\n", ESP.getMinFreeHeap());
    Serial.printf("ESP.getHeapSize() %d\n", ESP.getHeapSize());
    Serial.printf("ESP.getMaxAllocHeap() %d\n", ESP.getMaxAllocHeap());

    Serial.printf("Firmware started successfully\n");
    */

    //Synth_NoteOn(0, 64, 1.0f);
    //Synth_ModulationWheel(0,0.5 );

    xTaskCreatePinnedToCore(Core0Task, "Core0Task", 8000, NULL, 999, &Core0TaskHnd, 0);

    adc.comenzar();

    midi.Setup();
    Serial.begin(115200);
 }

float fl_sample, fr_sample;

void Core0TaskLoop()
{
    static uint32_t loop_count_u100 = 0;

    loop_count_u100++;
    if (loop_count_u100 % 100 == 0)    
    {
      adc.procesa(loop_count_u100 % 1000 == 0);
    }

    Menu_process();    
    //Serial.println(fl_sample*300);
}

void Core0Task(void *parameter)
{
    while (true)
    {
        Core0TaskLoop();

        /* this seems necessary to trigger the watchdog */
        delay(1);
        yield();
    }
}

/*
 * our main loop
 * - all is done in a blocking context
 * - do not block the loop otherwise you will get problems with your audio
 */

float valor (uint16_t analog) {
  return ((float) analog/4096);
}

void loop()
{
    static uint32_t loop_cnt_1hz;
    static uint32_t loop_count;

    loop_count++;

    if (i2s_write_stereo_samples(&fl_sample, &fr_sample))
    {
        /* nothing for here */
    }
    inicio = micros();
    El_Sinte.Process(&fl_sample, &fr_sample);
    acum += micros()-inicio;

    if ( loop_count == 10000 ) {
      //Serial.printf("Proceso %d ns\n", acum/10);
      loop_count = 0;
      acum = 0;
    }
    /*
     * process delay line
     */
    Delay_Process(&fl_sample, &fr_sample);

    /*
     * Midi does not required to be checked after every processed sample
     * - we divide our operation by 8
     */
    if (loop_count % 8 == 0)
    {
        midi.Process();
    }
}


