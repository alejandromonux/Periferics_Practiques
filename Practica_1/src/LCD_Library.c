/*
 * LCD_Library.c
 *
 *  Created on: 21 may. 2022
 *      Author: a-mon
 */
#include "LCD_Library.h"
/*
 * Inicialitza el LCD, SDRAM i tot all� que faci falta
 */
void LCD_initialize(){
	LCD_Init();
	LCD_LayerInit();
	//LCD_SPIConfig(); /YA LO HACE LCD_INIT??
	//SDRAM_Init(); //YA LO HACE LCD_INIT??
	//FMC_SDRAMWriteProtectionConfig(FMC_Bank2_SDRAM,DISABLE); //COMENTADO PORQUE LA FUNCI�N DE WRITE YA LO HACE SOLO.
/*
 * 	LCD_SetLayer(LCD_FOREGROUND_LAYER);
	LCD_DrawRect(120,120,20,20);
 */
}

/*
 * Dibuixa a [col,fila] el p�xel amb el valor [alfa, Rval, Gbal, Bval]
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt SetPixel (uint16_t col, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){}

/*
 * Retorna el valor del pixel a [col, fila].
 *
 * Returns: 32 bits de info del p�xel. 16 HLb tenen 0xFFFF, els 16 LLb tenen info de ARGB.
 */
uint32_t GetPixel (uint16_t col, uint16_t fila){}

/*
 * Dibuixa una l�nia entre [col_inici, fila] fins a [col_fi, fila] amb els valors ARGB entrats.
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt DibuixaLiniaHoritzontal (uint16_t col_inici, uint16_t col_fi, uint16_t fila, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){}

/*
 * Dibuixa una l�nia entre [col, fila_inici] fins a [col, fila_fi] amb els valors ARGB entrats.
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt DibuixaLiniaVertical (uint16_t col, uint16_t fila_inici, uint16_t fila_fi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){}

/*
 * Dibuixa una circumfer�ncia amb centre [col, fila] i radi [radi]
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt DibuixaCircumferencia (uint16_t ccol, uint16_t cfila, uint16_t radi, uint8_t alfa, uint8_t Rval, uint8_t Gval, uint8_t Bval ){}

/*
 * Esborra els p�xels del color seleccionat de la pantalla
 *
 * Returns: OK si funciona, NO_OK si no.
 */
RetSt EsborraPantalla (uint8_t Rval, uint8_t Gval, uint8_t Bval ){}


