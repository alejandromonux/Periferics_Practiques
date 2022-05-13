#include <DMA_usart_config.h>

void configUsart(int dataAmount){
	DMA_InitTypeDef DMA_InitStructure;
	/*Per el double buffer mode config*/
	uint32_t Memory1BaseAddr = 0; /*Esto no sé si está bien pero si no lo tocamos*/
	//Primer encenem el clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 ,ENABLE);

	/* CONFIGURACIÓN PARA EL RX (STREAM 5) */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&UsartIncomingThingies[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA2_Stream5, &DMA_InitStructure);
	DMA_DoubleBufferModeConfig(DMA2_Stream5,(uint32_t)&UsartIncomingThingies[2],DMA_Memory_0);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	DMA_DoubleBufferModeCmd(DMA2_Stream5,ENABLE);
	DMA_Cmd(DMA2_Stream5,ENABLE);
	/*Interrupción del RX*/
	//interrupt settings
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0X00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0X01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	/*Enable de la DMA con USART*/

	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);

	/* CONFIGURACIÓN PARA EL TX (STREAM 7) */

	/*Tornem a encendre la DMA*/

}

void DMA2_Stream5_IRQHandler()
{
	STM_EVAL_LEDToggle(LED4);
	// Netejem la flag
	DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF0);
	DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF0);
}
