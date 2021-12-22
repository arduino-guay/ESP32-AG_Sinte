#include  "AG_WaveTable.h"

float IRAM_ATTR AG_WTSierra::getValor(uint32_t i) 
{
     return (2.0f * ((float)i) / ((float)WAVEFORM_CNT)) - 1.0f; 
}

float IRAM_ATTR AG_WTCuadrada::getValor(uint32_t i) 
{ 
    return (i > (WAVEFORM_CNT / 2)) ? 1 : -1; 
}

float IRAM_ATTR AG_WTPulso::getValor(uint32_t i) 
{ 
    return (i > (WAVEFORM_CNT * ciclo)) ? 1 : -1; 
}

float IRAM_ATTR AG_WTTriangular::getValor(uint32_t i) 
{ 
    return ((i > (WAVEFORM_CNT / 2)) ? (((4.0f * (float)i) / ((float)WAVEFORM_CNT)) - 1.0f) : (3.0f - ((4.0f * (float)i) / ((float)WAVEFORM_CNT)))) - 2.0f; 
}

float IRAM_ATTR AG_WTRuidoBlanco::getValor(uint32_t i) 
{ 
    return (random(10000) / 5000.0) - 1.0f; 
}

float IRAM_ATTR AG_WTOff::getValor(uint32_t i) 
{ 
    return 0; 
}


