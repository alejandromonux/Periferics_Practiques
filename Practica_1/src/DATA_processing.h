/*
 * DATA_processing.h
 *
 *  Created on: 30 may. 2022
 *      Author: a-mon
 */

#ifndef DATA_PROCESSING_H_
#define DATA_PROCESSING_H_
#include <DMA_usart_config.h>

typedef struct{
	float angle;
	float distancia;
}Mesura;

float getSampleFrequency(uint8_t data);
float getAngle(uint16_t data);
float getDiferenciaAngles(Data data);
uint16_t getChecksum(Data data);
float getAngleMostra(uint16_t numMostra);
Mesura getMesura(uint16_t data, uint16_t numMostra, float angleIncrement);

#endif /* DATA_PROCESSING_H_ */
