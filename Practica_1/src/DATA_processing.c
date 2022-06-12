/*
 * DATA_processing.c
 *
 *  Created on: 30 may. 2022
 *      Author: a-mon
 */
#include <DATA_processing.h>

float getSampleFrequency(uint8_t data){
	return (float) (data >> 1)/10;
}

float getAngle(uint16_t data){
	return (float) (data >> 1)/64;
}

float getIncrementAngles(Data data){
	float angle_dif = getDiferenciaAngles(data);
	return angle_dif/(data.datasize-1);
}

float getDiferenciaAngles(Data data){
	float angle_dif = getAngle(data.angleFinal) - getAngle(data.angleInicial);
	if (angle_dif < 0) angle_dif += 360;
	return angle_dif;
}

uint16_t getChecksum(Data data){
	uint16_t checksum = 0xAA55;
	checksum ^= data.angleFinal;
	for (int i =0; i < 2*data.datasize; i++/*i+=2*/){
		checksum ^= (uint16_t) (data.data[i+1]<<8|data.data[i]);
		i++;
	}
	checksum ^= (uint16_t) ((data.datasize <<8) | data.type);
	checksum ^= data.angleInicial;
	return checksum;
}
float getAngleMostra(uint16_t angleInicial, uint16_t numMostra, float incrementAngle){
	return incrementAngle*numMostra +angleInicial;
}

Mesura getMesura(Data data, uint16_t numMostra,float increment){
	Mesura out;

	out.angle = getAngleMostra(data.angleInicial,numMostra/2, increment);
	out.distancia = ((float)((data.data[numMostra+1]<<8)|data.data[numMostra])/65535)*240;

	return out;
}
