/*
 * DMA_usart_config.h
 *
 *
 *      Author: alejandro.monux, josemaria.fernandez
 */
#ifndef _USARTCONFPRACT2_
#define _USARTCONFPRACT2_

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include <stm32f429i_discovery_lcd.h>
#include <stm32f429i_discovery_sdram.h>
#include "stm32f4xx_fmc.h"
#include <stdlib.h>
#include <DATA_processing.h>

#define DMA_REGISTERBOUNDARY_ADDRESS     ((uint32_t)0x40011000) /*SACADO DE	PAG 89 DEL DATASHEET*/
__IO uint8_t UsartIncomingThingies[512]; //Cosicas
__IO uint8_t UsartIncomingThingies2[512];
#define MAX_CUASIZE 512
unsigned int anteriorFinal;
unsigned int anteriorBuffer;
/*Arrays donde guardaremos la info*/
Data * dataArray;
unsigned int totalSize;
unsigned int pendingData;
uint8_t lastDataRead[2*MAX_DATASIZE+6];
uint8_t quantesDades;
float sample_frequency;
char USART_Attention;
/*pseudo-Cola buena para saber cuál printar ahora*/

unsigned int posicio;
unsigned char cua[MAX_CUASIZE]; //Num arbitrari
unsigned int indexCua;

void configUsart(int dataAmount);
void gestionaUsart();
Data desencua();

#endif
