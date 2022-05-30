#include <DMA_usart_config.h>

void configUsart(int dataAmount){
	DMA_InitTypeDef DMA_InitStructure;
	/*Per el double buffer mode config*/
	uint32_t Memory1BaseAddr = 0; /*Esto no s� si est� bien pero si no lo tocamos*/
	//Primer encenem el clock
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 ,ENABLE);

	/* CONFIGURACI�N PARA EL RX (STREAM 5) */
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&UsartIncomingThingies[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = 512;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
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
	DMA_DoubleBufferModeConfig(DMA2_Stream5,(uint32_t)&UsartIncomingThingies2[0],DMA_Memory_0);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	DMA_DoubleBufferModeCmd(DMA2_Stream5,ENABLE);


	/*Interrupci�n del RX*/
	//interrupt settings
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0X00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0X01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	/*Enable de la DMA con USART*/
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA2_Stream5,ENABLE);

	/* CONFIGURACI�N PARA EL TX (STREAM 7) */

	/*Configurem la mem�ria din�mica*/
	data = (Data *) malloc(sizeof(Data));
	pendingData = 0;
	totalSize = 1;
	anteriorFinal=0;
	anteriorBuffer=0;
}


//Es possible que arribin v�ries mostres abans de que aquesta funci� es faci per primer cop si arriben mostres per� no
//Plenen ning�n dels dos buffers
char isMostraPlena(){
	//TODO: Potser podem guardar aqu� en un array auxiliar la mostra i passarla al emplena i encua sense tant embolic
	int bufferActual = anteriorBuffer;
	uint8_t hemMiratInici=0;
	uint8_t hemMiratNumDades=0;
	quantesDades=255; //FIXME:Mirar que no pueda ser 0 por l ode pillar valores anteriores al [6]
	uint8_t numDades= 0;
	uint8_t * buffer = (bufferActual==0)?UsartIncomingThingies:UsartIncomingThingies2;
	int i = 0;
	for(i =anteriorFinal; numDades!=quantesDades;i++){
		//Mirem si no ens hem passat del tamany del buffer (igual al tamany de la cua
		if(i>MAX_CUASIZE-1){
			bufferActual = 1-bufferActual;
			i=0;
			buffer = (bufferActual==0)?UsartIncomingThingies:UsartIncomingThingies2;
		}

		//Mirem si tenim mostra sencera (
		//Realment si canvia de buffer no passa res perque son adre�es cont�gues. PER� potser mirar de com fer per si no ho fossin perque si es del 2 al 1 cagada
		//Mirem m�s tard que no hagi saltat el interrupt amb nom�s el 0x40 guardat [linia 102]
		if((buffer[i]==0x40)&&(buffer[i+1]==0x81) ){
			if((!hemMiratInici)){
				hemMiratInici=1;
			}else if (numDades==quantesDades){
				//MAI ENTRAR� PER LA CONDICI� DEL FOR??
				/*if(quantesDades!=255){
					lastDataRead[numDades]='\0'; //AIX� SABEM FINS ON LLEGIR
				}
				return 1; //TRUE
				*/
			}else{
				return 0; //FALSE
			}
		}


		if(hemMiratNumDades<6){
			//Apuntem quantes Mostres
			quantesDades= buffer[i];
			hemMiratNumDades++;
		}else{
			//Anem emplenant l'array
			lastDataRead[numDades++]=buffer[i];
		}
	}
	//Comprovem b� que no llegim res de brut�cia per lo de canviar de buffer o que hagi saltat el interrupt amb nom�s el 0x40 guardat
	if (hemMiratIinici==0){
		return 0;
	}else if(quantesDades!=255){
		lastDataRead[numDades]='\0'; //AIX� SABEM FINS ON LLEGIR
	}
	anteriorFinal=i;
	return 1;
}
void emplenaIEncua(Data * array, unsigned int index){
	array[index]->used=1;
	array[index]->datasize=quantesDades;
	array[index]->angleInicial=(lastDataRead[1]<<8)|lastDataRead[0];
	array[index]->angleFinal=(lastDataRead[2]<<8)|lastDataRead[1];
	array[index]->checksum=(lastDataRead[4]<<8)|lastDataRead[3];
	for(int i = 0;MAX_DATASIZE;i++){
		if(lastDataRead[i]!='\0'){
			array[index]->data[i] = lastDataRead[i];
			i++;
		}else{
			break;
		}
	}
	cua[indexCua]=index;
	indexCua = (indexCua+1)%MAX_CUASIZE;
}
Data desencua(){
	posicio = (posicio+1)%MAX_CUASIZE;
	pendingData--;
	return dataArray[cua[posicio++]];
}



void DMA2_Stream5_IRQHandler()
{
	STM_EVAL_LEDToggle(LED4);
	/*MIRAR SI LA MUESTRA EST� LLENA*/
	if(isMostraPlena()){
		if (pendingData == totalSize){
			//Realloc
			dataArray = (Data *) realloc(dataArray, sizeof(Data)*(++totalSize));
			emplenaIencua(dataArray, totalSize-1);
		}else{
			for(unsigned int i = 0; i<totalSize;){
				/*Mirar si nos da para los datos actuales o no*/
				if(dataArray[i].used==1){
					i++;
				}else{
					//Ficar dades i assignar posicio de la cua
					emplenaIencua(dataArray,i);
				}
			}
		}
		pendingData++;
	}

	// Netejem la flag
	DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);
	DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
}
