/*
 * DATA_processing.h
 *
 *
 *      Author: alejandro.monux, josemaria.fernandez
 */
#ifndef DATA_PROCESSING_H_
#define DATA_PROCESSING_H_
#define MAX_DATASIZE 255

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"
#include <stm32f429i_discovery_lcd.h>
#include <stm32f429i_discovery_sdram.h>
#include "stm32f4xx_fmc.h"
#include <stdlib.h>
#include <math.h>
typedef struct{
	float angle;
	float distancia;
	char dibuixaVermell;
}Mesura;
#define TRUECOS(angle) (cos((angle/180)*M_PI))
#define TRUESIN(angle) (sin((angle/180)*M_PI))

typedef struct{
	uint8_t used;
	uint8_t datasize;
	uint8_t type;
	uint16_t angleInicial;
	uint16_t angleFinal;
	uint16_t checksum;
	uint8_t data[2*MAX_DATASIZE]; //2 Bytes per mostra
}Data;

float getSampleFrequency(uint8_t data);
float getAngle(uint16_t data);
float getDiferenciaAngles(Data data);
float getIncrementAngles(Data data);
uint16_t getChecksum(Data data);
float getAngleMostra(uint16_t angleInicial, uint16_t numMostra, float incrementAngle);
Mesura getMesura(Data data, uint16_t numMostra,float increment);

#endif /* DATA_PROCESSING_H_ */
