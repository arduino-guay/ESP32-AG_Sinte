/*
 * config.h
 *
 * Put all your project settings here (defines, numbers, etc.)
 * configurations which are requiring knowledge of types etc.
 * shall be placed in z_config.ino (will be included at the end)
 *
 *  Created on: 12.05.2021
 *      Author: Marcel Licence
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* this will force using const velocity for all notes, remove this to get dynamic velocity */
#define MIDI_USE_CONST_VELOCITY

#ifdef I2S_NODAC
  #define I2S_NODAC_OUT_PIN   22  /* noisy sound without DAC, add capacitor in series! */
#endif

/*
 * DIN MIDI Pinout
 */
#define RXD2 16 /* U2RRXD */
#define TXD2 17

#define SAMPLE_RATE 44000
#define SAMPLE_SIZE_16BIT

#endif /* CONFIG_H_ */

