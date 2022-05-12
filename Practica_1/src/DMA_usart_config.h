#ifndef _USARTCONFPRACT2_
#define _USARTCONFPRACT2_

#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"

#define DMA_REGISTERBOUNDARY_ADDRESS     ((uint32_t)0x40011000) /*IGUAL NO ESTÁ BIEN. SACADO DE	PAG 89 DEL DATASHEET*/


void configUsart(int dataAmount);

#endif
