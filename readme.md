# Sintetizador analógico, estilo de los 80 basado en ESP32
### Descripción
Sintetizador monofónico totálmente autónomo , inicialmente basado en el gran trabajo de Marcel-licence ([https://github.com/marcel-licence/esp32_basic_synth](https://github.com/marcel-licence/esp32_basic_synth)).
El código ha sido refactorizado (todavía no esta al 100%) , separando los distintos elementos en clases C++ para facilitar su mantenimiento y expansión.
Algunos elementos han sido modificados/cambiados, teniendo en cuenta que el objetivo buscado era un sintetizador autónomo, al estilo de los sintetizadores analógicos de los 80.

### Características

- Testado con módulo ESP32 DOIT DEV KIT (pero fácilmente adaptable a otros módulos ESP32)
- Salida monofónica (misma señal por ambos canales)
- Hasta 11 voces con dos osciladores cada una
- Modo "unison" con número de voces por nota seleccionable por menú así como el "detuning" entre voces.
- Dos osciladores con formas de onda senoidal, cuadrada, sierra, triangular, pulso, senoidal escalonada y ruido blanco
- El segundo oscilador permite seleccionar volumen y variar en +/- 5 octavas su nota
- Filtro general con Cutoff y Resonancia variables (tipo seleccionable por menú).
- ADRS de volumen y filtro con tramos exponenciales ([Nigel Redmon](http://www.earlevel.com/main/category/digital-audio/oscillators/envelope-generators/?orderby=date&order=ASC))
- Generador de ruido blanco independiente
- LFO con velocidad y profundidad variables y destino seleccionable mediante menú (volumen, frecuencia y cutoff del filtro general), así como la forma de onda (mismas que los osciladores, mas una señal aleatoria de baja frecuencia)
- Recibe señales midi a través de una entrada estandar, asi como a través del puerto usb del módulo ESP32 (para esto es necesario utilizar un puente MIDI->Serial en el PC ([Hairless MIDI](https://projectgus.github.io/hairless-midiserial/)))
- Límea de retardo configurable desde el menú


### Dependencias
Aparte de tener instado en el IDE las librerías necesarias para la tarjeta ESP32 se utilizan algunas librerías:

- [Arduino MIDI library](https://github.com/FortySevenEffects/arduino_midi_library)
- [Arduino Menu](https://github.com/neu-rah/ArduinoMenu)
- [ClickEncoder](https://github.com/0xPIT/encoder)
- [Ucglib](https://github.com/olikraus/ucglib)

### Componentes
- ESP32 DOIT DEV KIT
- LCD 2,4" ILI9341 320x240 pixels
- 17 potenciómetros (1K ~ 20K)
- Encoder rotatorio
- Módulo muliplexor 16 canales (CD74HC4067)
- Módulo DAC I2s (en mi caso un Max98357   con amplificador incluido de 3W)
- Optoacoplador para entrada MIDI (en mi caso 4N28) 

### Conexiones
|  ESP32 GPIO |  DESTINO | ELEMENTO  |
| ------------ | ------------ | ------------ | 
|	22	|	S0	|	MUX ADC	|
|	12	|	S1	|		|
|	13	|	S2	|		|
|	21	|	S3	|		|
|	15	|	ADC	|		|
| ------------ | ------------ | ------------ | 
|	5	|	CS	|	TFT	|
|	4	|	RESET	|		|
|	2	|	DC	|		|
|	23	|	MOSI	|		|
|	18	|	SCK	|		|
| ------------ | ------------ | ------------ | 
|	14	|	EncBtn	|	Encoder	|
|	33	|	EncA	|		|
|	32	|	EncB	|		|
| ------------ | ------------ | ------------ | 
|	26	|	Din	|	I2S DAC	|
|	25	|	Bclk	|		|
|	27	|	LCR	|		|
| ------------ | ------------ | ------------ | 
|	16	|	RX2	|	MIDI IN	|
| ------------ | ------------ | ------------ | 
|	35	|	Volumen	| VOL GRAL|

### Etiquetas
\#Esp32, \#Arduino, \#Sintetizador, \#Synthesizer, \#Música, #Music, \#Sonido, \#Sound

### Vídeos
Próximamente


### Fotos
![Frontal ](https://github.com/arduino-guay/AG_Sinte/blob/main/fotos/AG_Sinte-ESP32-Frontal.jpg "Frontal ")
![Trasera frontal](https://github.com/arduino-guay/AG_Sinte/blob/main/fotos/AG_Sinte-ESP32-Frontal-Bck.jpg "Trasera frontal")
![Placa](https://github.com/arduino-guay/AG_Sinte/blob/main/fotos/AG_Sinte-ESP32-Placa.jpg "Placa")
![Multiplexor](https://github.com/arduino-guay/AG_Sinte/blob/main/fotos/AG_Sinte-ESP32-MultiplexorADC.jpg "Multiplexor")![Conjuto](https://github.com/arduino-guay/AG_Sinte/blob/main/fotos/AG_Sinte-ESP32-Abierto.jpg "Conjuto")