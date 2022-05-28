/*
 * LCD_Library.h
 *
 *  Created on: 21 may. 2022
 *      Author: a-mon
 */

#ifndef LCD_LIBRARY_H_
#define LCD_LIBRARY_H_
#include <DMA_usart_config.h>
#include <stm32f429i_discovery_lcd.h>
#include <stm32f429i_discovery_sdram.h>
#include "stm32f4xx_fmc.h"

typedef enum {
	NO_OK = 0,
	OK = !NO_OK
} RetSt;

#define ALPHA_GOOD 0
/*Colur RED*/
#define RED_R 0x1FF
#define RED_G 0
#define RED_B 0
/*Colur GREEN*/
#define GREEN_R 0
#define GREEN_G 0x1FF
#define GREEN_B 0
/*Colur BLUE*/
#define BLUE_R 0
#define BLUE_G 0
#define BLUE_B 0x1FF

#define N_COL 240
#define N_FIL 320
uint16_t *frame_buffer;



void LCD_initialize();
RetSt SetPixel (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
uint32_t GetPixel (uint16_t col, uint16_t fila);
RetSt DibuixaLiniaHoritzontal ( uint16_t fila,uint16_t col_inici, uint16_t col_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaLiniaVertical (uint16_t col, uint16_t fila_inici, uint16_t fila_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaCircumferencia (uint16_t ccol, uint16_t cfila, uint16_t radi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt EsborraPantalla (uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaEstructura();
RetSt PintaRecuadreEstructura(uint8_t Rval, uint8_t Gval, uint8_t Bval);
#endif /* LCD_LIBRARY_H_ */
