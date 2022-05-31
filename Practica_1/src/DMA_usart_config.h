#ifndef _USARTCONFPRACT2_
#define _USARTCONFPRACT2_

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include <stm32f429i_discovery_lcd.h>
#include <stm32f429i_discovery_sdram.h>
#include "stm32f4xx_fmc.h"
#include <stdlib.h>
#include <DATA_processing.h>

#define DMA_REGISTERBOUNDARY_ADDRESS     ((uint32_t)0x40011000) /*IGUAL NO ESTÁ BIEN. SACADO DE	PAG 89 DEL DATASHEET*/
__IO uint8_t UsartIncomingThingies[512]; //Cosicas
__IO uint8_t UsartIncomingThingies2[512];
unsigned int anteriorFinal;
unsigned int anteriorBuffer;
/*Arrays donde guardaremos la info*/
Data * dataArray;
unsigned int totalSize;
unsigned int pendingData;
uint8_t lastDataRead[MAX_DATASIZE];
uint8_t quantesDades;
float sample_frequency;
/*pseudo-Cola buena para saber cuál printar ahora*/
#define MAX_CUASIZE 512
unsigned int posicio;
unsigned char cua[MAX_CUASIZE]; //Num arbitrari
unsigned int indexCua; //Pot ser que el index doni la volta i avanci la posició??


void configUsart(int dataAmount);
Data desencua();

#endif
