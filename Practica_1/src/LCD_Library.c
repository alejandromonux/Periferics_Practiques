/*
 * LCD_Library.c
 *
 *  Created on: 21 may. 2022
 *      Author: a-mon
 */
#include "LCD_Library.h"
#include <math.h>;
/*
 * Inicialitza el LCD, SDRAM i tot allò que faci falta
 */
void LCD_initialize(){
	LCD_Init();
	LTDC_Cmd(ENABLE);
	LCD_DisplayOn();
	LCD_LayerInit();
	LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB4444);
	LTDC_ReloadConfig(LTDC_VBReload);
	LCD_SetLayer(LCD_BACKGROUND_LAYER);
	LCD_Clear(LCD_COLOR_WHITE); //TODO: Cambiarlo por el esborra_LCD


	//LCD_SPIConfig(); /YA LO HACE LCD_INIT??
	//SDRAM_Init(); //YA LO HACE LCD_INIT??
	//FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM,DISABLE); //COMENTADO PORQUE LA FUNCIÓN DE WRITE YA LO HACE SOLO.
/*
 * 	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_DrawRect(120,120,20,20);
 */
}

/*
 * Dibuixa a [col,fila] el píxel amb el valor [alfa, Rval, Gbal, Bval]
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt SetPixel (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval )
{
	uint16_t color = ((alfa) << 16) | ((Rval) << 8) | ((Gval) << 4) | (Bval);
	if (col >= N_COL || fila >= N_FIL)
		return (ERROR);
	frame_buffer[col + fila*LCD_PIXEL_WIDTH]= color;
	return (OK);
}

/*
 * Retorna el valor del pixel a [col, fila].
 *
 * Returns: 32 bits de info del píxel. 16 HLb tenen 0xFFFF, els 16 LLb tenen info de ARGB.
 */
uint32_t GetPixel (uint16_t col, uint16_t fila){
	return frame_buffer[col + fila*LCD_PIXEL_WIDTH];
}

/*
 * Dibuixa una línia entre [col_inici, fila] fins a [col_fi, fila] amb els valors ARGB entrats.
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt DibuixaLiniaHoritzontal (uint16_t col_inici, uint16_t col_fi, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){
	for(int i = col_inici; i<=col_fi; i++){
		SetPixel(i,fila,alfa,Rval,Gval,Bval);
	}
	return(OK);
}

/*
 * Dibuixa una línia entre [col, fila_inici] fins a [col, fila_fi] amb els valors ARGB entrats.
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt DibuixaLiniaVertical (uint16_t col, uint16_t fila_inici, uint16_t fila_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){
	for(int i = fila_inici; i<=fila_fi; i++){
		SetPixel(col,i,alfa,Rval,Gval,Bval);
	}
	return(OK);
}
/*
 * Dibuixa una circumferència amb centre [col, fila] i radi [radi]
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt DibuixaCircumferencia (uint16_t ccol, uint16_t cfila, uint16_t radi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){
	int x, y;
	float pi = 3.1415;
	for(int i = 0; i<2*pi;i = i+pi/50){
		x = radi*cos(i)+ccol;
		y = radi*sin(i)+cfila;
		SetPixel(x,y,alfa,Rval,Gval,Bval);
	}
}

/*
 * Esborra els píxels del color seleccionat de la pantalla
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt EsborraPantalla (uint8_t Rval, uint8_t Gval, uint8_t Bval ){
	  for(int i = 0; i < N_COL;i++){
		  for(int j = 0; j < N_FIL ;j++){
			  	  SetPixel(i,j,0,Rval,Gval,Bval);
		  	  }
	  }
}

