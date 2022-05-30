/*
 * DATA_processing.c
 *
 *  Created on: 30 may. 2022
 *      Author: a-mon
 */


float getSampleFrequency(uint8_t data){
	return (float) (data >> 1)/10;
}

float getAngle(uint16_t data){
	return (float) (data >> 1)/64;
}

float getDiferenciaAngles(Data data){
	float angle_dif = getAngle(data.angleInicial) - getAngle(data.angleFinal);
	if (angle_dif < 0) angle_dif += 360;
	return angle_dif;
}

uint16_t getChecksum(Data data){
	uint16_t checksum = 0x8140;
	checksum ^= data.angleFinal;
	for (int i =0; i < 2*data.datasize; i+2){
		checksum ^= (uint16_t) (data.data[i]<<8|data[i]);
	}
	checksum ^= (uint16_t) (data.datasize <<8 | data.datasize= 0x01);
	checksum ^= data.angleInicial;
	return checksum;
}
float getAngleMostra(uint16_t angleInicial, uint16_t numMostra, float incrementAngle){
	return incrementAngle*numMostra +angleInicial;
}

Mesura getMesura(uint16_t data, uint16_t numMostra,float increment){
	Mesura out;

	out.angle = getAngleMostra(data.angleInicial,numMostra, increment);
	out.distancia = (float) (data.data[numMostra+1]<<8)|data.data[numMostra];

	return out;
}
