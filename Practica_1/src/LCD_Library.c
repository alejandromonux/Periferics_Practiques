/*
 * LCD_Library.c
 *
 *  Created on: 21 may. 2022
 *      Author: a-mon
 */
#include "LCD_Library.h"

/*
 * Inicialitza el LCD, SDRAM i tot allò que faci falta
 */
void LCD_initialize(){
	LCD_Init();
	LTDC_Cmd(ENABLE);
	LCD_DisplayOn();
	LCD_LayerInit();
	LTDC_LayerPixelFormat(LTDC_Layer1, LTDC_Pixelformat_ARGB1555);
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
 * Nota: Columna i fila van al revés
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt SetPixel (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval )
{
	//ARGB1555
	uint16_t color = ((alfa) << 15) | ((Rval) << 10) | ((Gval) << 5) | (Bval);
	if (col >= N_COL || fila >= N_FIL)
		return (ERROR);
	frame_buffer[(N_COL-col) + fila*LCD_PIXEL_WIDTH]= color;
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
RetSt DibuixaLiniaHoritzontal (uint16_t fila, uint16_t col_inici, uint16_t col_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){
	for(int i = col_inici; i<=col_fi; i++){
		SetPixel(fila,i,alfa,Rval,Gval,Bval);
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
		SetPixel(i,col,alfa,Rval,Gval,Bval);
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
	for(float i = 0; i<2*pi;i = i+pi/1000){
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

RetSt DibuixaEstructura(){
	#define RADI_BASE 24
	DibuixaCircumferencia(120,120,RADI_BASE,  0,25,20,24);
	DibuixaCircumferencia(120,120,RADI_BASE*2,0,25,20,24);
	DibuixaCircumferencia(120,120,RADI_BASE*3,0,25,20,24);
	DibuixaCircumferencia(120,120,RADI_BASE*4,0,25,20,24);

	//Grid General
	DibuixaLiniaVertical(120, 0,N_FIL,0,12,3,5);
	DibuixaLiniaVertical(240, 0,N_FIL,0,12,3,5);
	DibuixaLiniaHoritzontal(120, 0,240,0,12,3,5);
	//Cuadre
	PintaRecuadreEstructura(GREEN_R,GREEN_G,GREEN_B);
	return OK;
}

RetSt PintaRecuadreEstructura(uint8_t Rval, uint8_t Gval, uint8_t Bval){
	#define VE 250//Horitzontal Top
	#define VD 310//Horitzontal Bottom
	#define HT 20//Vertical Esquerra
	#define HB 60//Vertical Dreta

	DibuixaLiniaVertical(VE, HT,HB,0,12,3,5);
	DibuixaLiniaVertical(VD, HT,HB,0,12,3,5);
	DibuixaLiniaHoritzontal(HT, VE,VD,0,12,3,5);
	DibuixaLiniaHoritzontal(HB, VE,VD,0,12,3,5);

	//Emplenem
	for(int i=HT+1;i<HB;i++){
		for(int j=VE+1;j<VD;j++){
			SetPixel(i,j,0,Rval,Gval,Bval);
		}
	}
	return OK;
}

