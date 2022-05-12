#include <DMA_usart_config.h>

void configUsart(int dataAmount){
	DMA_InitTypeDef DMA_InitStructure;
	/*Per el double buffer mode config*/

	uint32_t Memory1BaseAddr = 0; /*Esto no sé si está bien pero si no lo tocamos*/
	uint32_t DMA_CurrentMemory = 0;

	//Primer encenem el clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 ,ENABLE);

	/* CONFIGURACIÓN PARA EL RX (STREAM 5) */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART1_DR_ADDRESS;
	  DMA_InitStructure.DMA_Memory0BaseAddr = NULL; /*Com fem servir el Double Buffer, no ho utilitzem*/
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	  DMA_InitStructure.DMA_BufferSize = 1;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	  DMA_Init(DMA1_Stream5, &DMA_InitStructure);
	  DMA_DoubleBufferModeConfig(DMA1_Stream5,Memory1BaseAddr,DMA_CurrentMemory);
	  DMA_DoubleBufferModeCmd(DMA1_Stream5,ENABLE);
	  DMA_Cmd(DMA1_Stream5, ENABLE);

	/* CONFIGURACIÓN PARA EL TX (STREAM 7) */
}
