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

/*Colur RED*/
#define RED_A 1
#define RED_R 255
#define RED_G 0
#define RED_B 0
/*Colur GREEN*/
#define GREEN_A 1
#define GREEN_R 0
#define GREEN_G 255
#define GREEN_B 0
/*Colur BLUE*/
#define BLUE_A 1
#define BLUE_R 0
#define BLUE_B 255
/*Colur PURPLE*/
#define PURPLE_A 1
#define PURPLE_R 136
#define PURPLE_G 50
#define PURPLE_B 217

#define N_COL	1024
#define N_FIL 768
uint16_t *frame_buffer;



void LCD_initialize();
RetSt SetPixel (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
uint32_t GetPixel (uint16_t col, uint16_t fila);
RetSt DibuixaLiniaHoritzontal (uint16_t col_inici, uint16_t col_fi, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaLiniaVertical (uint16_t col, uint16_t fila_inici, uint16_t fila_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt DibuixaCircumferencia (uint16_t ccol, uint16_t cfila, uint16_t radi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval );
RetSt EsborraPantalla (uint8_t Rval, uint8_t Gval, uint8_t Bval );

#endif /* LCD_LIBRARY_H_ */
