#ifndef _USARTCONFPRACT2_
#define _USARTCONFPRACT2_

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"



#define DMA_REGISTERBOUNDARY_ADDRESS     ((uint32_t)0x40011000) /*IGUAL NO ESTÁ BIEN. SACADO DE	PAG 89 DEL DATASHEET*/
__IO uint8_t UsartIncomingThingies[512]; //Cosicas
__IO uint8_t UsartIncomingThingies2[512];
unsigned int anteriorFinal;
unsigned int anteriorBuffer;
/*Arrays donde guardaremos la info*/
#define MAX_DATASIZE 255
typedef struct{
	uint8_t used;
	uint8_t datasize;
	uint16_t angleInicial;
	uint16_t angleFinal;
	uint16_t checksum;
	uint16_t data[MAX_DATASIZE]; //2 Bytes per mostra
}Data;
Data * dataArray;
unsigned int totalSize;
unsigned int pendingData;
uint8_t lastDataRead[MAX_DATASIZE];
uint8_t quantesDades;
/*pseudo-Cola buena para saber cuál printar ahora*/
#define MAX_CUASIZE 512
unsigned int posicio;
unsigned char cua[MAX_CUASIZE]; //Num arbitrari
unsigned int indexCua; //Pot ser que el index doni la volta i avanci la posició??


void configUsart(int dataAmount);

#endif
