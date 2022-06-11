#include <DMA_usart_config.h>
uint8_t hemMiratInici=0;
uint8_t tipus=0;

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


	/*Interrupción del RX*/
	//interrupt settings
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0X00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0X00;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	/*Enable de la DMA con USART*/
	DMA_ITConfig(DMA2_Stream5,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA2_Stream5,ENABLE);

	/* CONFIGURACIÓN PARA EL TX (STREAM 7) */

	/*Configurem la memòria dinàmica*/
	dataArray = (Data *) malloc(sizeof(Data));
	dataArray[0].used=0;
	pendingData = 0;
	totalSize = 1;
	anteriorFinal=0;
	anteriorBuffer=0;
}

Data desencua(){
	posicio = (posicio+1)%MAX_CUASIZE;
	pendingData--;
	return dataArray[cua[posicio/*++*/]];
}

char isMostraPlenaDeVerdad(){
	int bufferActual = anteriorBuffer;
	uint8_t hemMiratNumDades=0;
	int16_t numDades= 0;
	uint8_t * buffer = (bufferActual==0)?UsartIncomingThingies:UsartIncomingThingies2;
	int i = 0;
	quantesDades=255;
	hemMiratInici=0;

	for(i =anteriorFinal; numDades-6/*-6 que son los extras de angulos y checksum*/<2*quantesDades;i++){
		//Mirem si no ens hem passat del tamany del buffer (igual al tamany de la cua
		if(i>MAX_CUASIZE-1){
			bufferActual = 1-bufferActual;
			i=0;
			buffer = (bufferActual==0)?UsartIncomingThingies:UsartIncomingThingies2;
		}

		//Mirem si tenim mostra sencera
		if((buffer[i+1]==0x40)&&(buffer[i]==0x81)){
			if((!hemMiratInici)){
				hemMiratInici=1;
				buffer[i+1]= 0x69;
				buffer[i]= 0x69;
			}else{
				return 0; //FALSE
			}
		}


		if(hemMiratNumDades<6){
			if(hemMiratNumDades==4) tipus = buffer[i];
			quantesDades= buffer[i];
			hemMiratNumDades++;
		}else{
			//Anem emplenant l'array
			lastDataRead[numDades]=buffer[i];
			numDades++;
		}
	}

	if (hemMiratInici==0){
		return 0;
	}

	montaDadaIEncua();
	anteriorFinal=i;
	anteriorBuffer=bufferActual;
	if((i<MAX_CUASIZE-1)&&(i+1<MAX_CUASIZE-1)){
		if((buffer[i+1]==0x40)&&(buffer[i]==0x81)){
			isMostraPlenaDeVerdad();
		}
	}
	//Comprovem bé que no llegim res de brutícia per lo de canviar de buffer o que hagi saltat el interrupt amb només el 0x40 guardat
	return 1;
}

void montaDadaIEncua(){
	uint16_t indexArray = 0;

	//Encuament
	unsigned int posicioAEncuar = 0;
	if (pendingData == totalSize){
				//Realloc
				dataArray = (Data *) realloc(dataArray, sizeof(Data)*(++totalSize));
				posicioAEncuar=totalSize-1;
	}else{
		for(posicioAEncuar = 0; posicioAEncuar<totalSize;){
			/*Mirar si nos da para los datos actuales o no*/
			if(dataArray[posicioAEncuar].used==0)break;
			posicioAEncuar++;
		}
	}

	dataArray[posicioAEncuar].used=1;
	dataArray[posicioAEncuar].datasize=quantesDades;
	dataArray[posicioAEncuar].type = tipus;
	dataArray[posicioAEncuar].angleFinal=(lastDataRead[1]<<8)|lastDataRead[0];
	dataArray[posicioAEncuar].angleInicial=(lastDataRead[3]<<8)|lastDataRead[2];
	dataArray[posicioAEncuar].checksum=(lastDataRead[5]<<8)|lastDataRead[4];
	for(int i = 6;indexArray < 2*quantesDades;i++){
		dataArray[posicioAEncuar].data[indexArray] = lastDataRead[i];
		indexArray++;
	}
	cua[indexCua]=posicioAEncuar;
	indexCua = (indexCua+1)%MAX_CUASIZE;
	pendingData++;

}

//Es possible que arribin vàries mostres abans de que aquesta funció es faci per primer cop si arriben mostres però no
//Plenen ningún dels dos buffers
char isMostraPlena(){
	//TODO: Potser podem guardar aquí en un array auxiliar la mostra i passarla al emplena i encua sense tant embolic
	int bufferActual = anteriorBuffer;
	hemMiratInici=0;
	uint8_t hemMiratNumDades=0;
	quantesDades=255; //FIXME:Mirar que no pueda ser 0 por l ode pillar valores anteriores al [6]
	int16_t numDades= 0;
	uint8_t * buffer = (bufferActual==0)?UsartIncomingThingies:UsartIncomingThingies2;
	uint8_t inicial = 0;
	int i = 0;
	for(i =anteriorFinal; numDades-6/*-6 que son los extras de angulos y checksum*/<2*quantesDades;i++){
		//Mirem si no ens hem passat del tamany del buffer (igual al tamany de la cua
		if(i>MAX_CUASIZE-1){
			bufferActual = 1-bufferActual;
			i=0;
			buffer = (bufferActual==0)?UsartIncomingThingies:UsartIncomingThingies2;
		}

		//Mirem si tenim mostra sencera (
		//Realment si canvia de buffer no passa res perque son adreçes contígues. PERÒ potser mirar de com fer per si no ho fossin perque si es del 2 al 1 cagada
		//Mirem més tard que no hagi saltat el interrupt amb només el 0x40 guardat [linia 102]
		if((buffer[i+1]==0x40)&&(buffer[i]==0x81) ){
			if((!hemMiratInici)){
				hemMiratInici=1;
			}else if (numDades==quantesDades){
				//MAI ENTRARÀ PER LA CONDICIÓ DEL FOR??
				/*if(quantesDades!=255){
					lastDataRead[numDades]='\0'; //AIXÍ SABEM FINS ON LLEGIR
				}
				return 1; //TRUE
				*/
			}else{
				return 0; //FALSE
			}
		}


		if(hemMiratNumDades<6){
			//Apuntem quantes Mostres
			if((hemMiratNumDades==4)&&((buffer[i]&0x0001)==1)&&(sample_frequency == 0)){
				//Això es per a que quan surti la mostra inicial no la guardem i només agafem la frecuència de mostres
				uint8_t valor = buffer[i];
				sample_frequency = getSampleFrequency(valor);
				hemMiratInici=0;
				numDades = 0; //Lo reiniciamos para que cuando entremos después de la inicial
				//FIXME: Con esto no miramos el CRC de la inicial, mal?
				i=anteriorFinal+13; //13 y no 14 porque luego hace el +1 y pasa a 14 que es ya la primera trama
				hemMiratNumDades=0;
			}else{
				quantesDades= buffer[i];
				hemMiratNumDades++;
			}
		}else{
			//Anem emplenant l'array
			lastDataRead[numDades]=buffer[i];
			if(numDades==492){
				anteriorFinal=14;
			}

			numDades++;
		}
	}
	//Comprovem bé que no llegim res de brutícia per lo de canviar de buffer o que hagi saltat el interrupt amb només el 0x40 guardat
	anteriorFinal=i;
	anteriorBuffer=bufferActual;
	if (hemMiratInici==0){
		return 0;
	}/*else if(quantesDades!=255){
		lastDataRead[numDades]='\0'; //AIXÍ SABEM FINS ON LLEGIR
	}*/
	return 1;
}

void emplenaIEncua(Data * array, unsigned int index){
	array[index].used=1;
	array[index].datasize=quantesDades;
	array[index].angleFinal=(lastDataRead[1]<<8)|lastDataRead[0];
	array[index].angleInicial=(lastDataRead[3]<<8)|lastDataRead[2];
	array[index].checksum=(lastDataRead[5]<<8)|lastDataRead[4];
	uint16_t indexArray = 0;
	for(int i = 6;indexArray < 2*quantesDades;i++){
		//if(lastDataRead[i]!='\0'){
			array[index].data[indexArray] = lastDataRead[i];
			indexArray++;
		/*}else{
			break;
		}*/
	}
	cua[indexCua]=index;
	indexCua = (indexCua+1)%MAX_CUASIZE;
}
void gestionaUsart(){
	char isMostraFull = isMostraPlena();
	if(isMostraFull){
		if (pendingData == totalSize){
			//Realloc
			dataArray = (Data *) realloc(dataArray, sizeof(Data)*(++totalSize));
			emplenaIEncua(dataArray, totalSize-1);
		}else{
			for(unsigned int i = 0; i<totalSize;){
				/*Mirar si nos da para los datos actuales o no*/
				if(dataArray[i].used==1){
					i++;
				}else{
					//Ficar dades i assignar posicio de la cua
					emplenaIEncua(dataArray,i);
				}
			}
		}
		pendingData++;
	}
}

void DMA2_Stream5_IRQHandler()
{
	STM_EVAL_LEDToggle(LED4);
	/*MIRAR SI LA MUESTRA ESTÁ LLENA*/
	/*
	*/
	//USART_Attention=1;
	isMostraPlenaDeVerdad();
	// Netejem la flag
	DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_TCIF5);
	DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
}
