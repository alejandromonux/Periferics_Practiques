/*
 * File:     main.c
 * Authors:  Alejandro Moñux
 *           José María Fernández López
 *
 *
 * */

/* Includes */
//#include "stm32f4xx.h"
//#include "stm32f429i_discovery.h"
#include <DMA_usart_config.h>

#define MAXINTVALUE 	4294967295
#define TIME_MAGNITUTE_DENOMINATOR 1000000
#define COMANDA_INIT 	0xA5
#define STARTSCAN_VALUE 0x60
#define STARTSCAN_MACRO {0xA5,0x60}
/* variables */
static char interrupts;
char wheelsOn;
unsigned int numOverflows[2]; //Used for the time calculation.
unsigned int numOverflowsOLD[2]; //Used for the time calculation.
int periodMS[2];
int calcDebug[2][10];
int calcDebugCounter[2];
int duty_cycle1 = 50; //valor entre 0 y 100%
int duty_cycle2 = 50; //valor entre 0 y 100%
int cycle = 0; //No se me ocurre como hacerlo ahora mismo
char state1 = 0;
char state2 = 0;
int velValue = 1000;
int waitTo = 0;
int counter = 0;
int subclock = 0;
char startPulsado=0;
/* Private function prototypes */
/* Private functions */

/*
 *
 *TIMER
 *CONFIG
 *
 */
void TIM_INT_Init(){
    // Enable clock for TIM2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    // TIM2 initialization overflow every 1ms
    // TIM2 by default has clock of 84MHz
    // Here, we must set value of prescaler and period,
    // so update event is 0.5Hz or 500ms
    // Update Event (Hz) = timer_clock / ((TIM_Prescaler + 1) *  (TIM_Period + 1))
    // Update Event (Hz) = 42MHz / ((99+ 1) * (419+ 1)) = 1000 Hz
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 89;
    TIM_TimeBaseInitStruct.TIM_Period = 999;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;

    // TIM2 initialize
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    // Enable TIM2 interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    // Start TIM2
    TIM_Cmd(TIM2, ENABLE);

    // Nested vectored interrupt settings
    // TIM2 interrupt is most important (PreemptionPriority and
    // SubPriority = 0)
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);



	//Timer per comptar els microsegons entre els flancs de pujada
	  // default clock 84MHz
	  // Update Event = timer_clock / ((Prescaler + 1) *  (Period + 1))
		// Update Event (Hz) = 90MHz / ((2+ 1) * (14+ 1)) = 1 MHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct_T5;
	TIM_TimeBaseInitStruct_T5.TIM_Prescaler = 89;
	TIM_TimeBaseInitStruct_T5.TIM_Period = 0xFFFE;
	TIM_TimeBaseInitStruct_T5.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct_T5.TIM_CounterMode = TIM_CounterMode_Up;

	// TIM initialize
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseInitStruct_T5);
	// Enable TIM interrupt
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	// Start TIM
	TIM_Cmd(TIM5, ENABLE);

	//interrupt settings
	NVIC_InitTypeDef NVIC_InitStruct_T5;
	NVIC_InitStruct_T5.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStruct_T5.NVIC_IRQChannelPreemptionPriority = 0X00;
	NVIC_InitStruct_T5.NVIC_IRQChannelSubPriority = 0X02;
	NVIC_InitStruct_T5.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct_T5);

}

void TIM5_IRQHandler(){
	numOverflows[0]++;
	numOverflows[1]++;
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
}


void TIM2_IRQHandler() //RSI Timer2
{

	if(interrupts ==200){
		STM_EVAL_LEDToggle(LED3);
		interrupts = 0;
	}else{
		interrupts++;
	}
	//DUTY CYCLE PWM
	if (wheelsOn == 1){
		duty_cycle1 = 100;
		duty_cycle2 = 100;
	}else{
		duty_cycle1 = 50;
		duty_cycle2 = 50;
	}
    if ((GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_7))&&(startPulsado==0)){
    	//Enviar por la USART
    	USART_SendData(USART1, COMANDA_INIT);
    	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)){}
    	USART_SendData(USART1, STARTSCAN_VALUE);
		STM_EVAL_LEDOn(LED4);
		configUsart(0);
    	startPulsado = 1;
    }

	// Netejem la flag
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    //}
}


/***
 * intIndex: 0 si EXTI1, 1 si EXTI2
 */

int getRevs(int intIndex){
	uint32_t value = TIM_GetCounter(TIM5);
	if (periodMS[intIndex] == -1){
		periodMS[intIndex] = value;
		return -1;
	}else{
		// controlar el overflow
		if (value < periodMS[intIndex]){
			periodMS[intIndex] = MAXINTVALUE*(numOverflows[intIndex]-numOverflowsOLD[intIndex]) - periodMS[intIndex] + value;
		}else{
			periodMS[intIndex] = value - periodMS[intIndex];
		}
		float auxiliar = (1/(16*(periodMS[intIndex]/(float)TIME_MAGNITUTE_DENOMINATOR)));
		int output = (int)auxiliar; //(int)(auxiliar*1000); //Calculem revolucions
		//Lo anterior lo hemos puesto sin el *1000 porque ya sale un num lï¿½gico, digamos. Antes la resoluciï¿½n era de ms y ahora es de us
		calcDebug[intIndex][calcDebugCounter[intIndex]] = output;
		periodMS[intIndex] = value;
		numOverflowsOLD[intIndex]=numOverflows[intIndex];
		return output;
	}
}

void EXTI1_IRQHandler()
{
	int valueRoda1 = getRevs(0);
	// Netejem la flag
    EXTI_ClearITPendingBit(EXTI_Line1);
}
void EXTI2_IRQHandler()
{
	int valueRoda2 = getRevs(1);
	// Netejem la flag
    EXTI_ClearITPendingBit(EXTI_Line2);
}

void INIT_IO_PRACTICA_1(){
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  //INPUTS
      GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2|GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  //Configurem el interrupt dels sensors.

	  /*******************
	   * SENSOR DEL PD01 *
	   *******************/
	  EXTI_InitTypeDef EXTI_InitStruct;
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);
	  EXTI_InitStruct.EXTI_Line = EXTI_Line1;
	  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_Init(&EXTI_InitStruct);

 	  /* Add IRQ vector to NVIC */
	  //Prioritat a 1 al vector
	  NVIC_InitTypeDef NVIC_InitStruct;
	  NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
	  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	  NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStruct);
	  /*******************
	   * SENSOR DEL PD02 *
	   *******************/
	  EXTI_InitTypeDef EXTI_InitStruct2;
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);
	  EXTI_InitStruct2.EXTI_Line = EXTI_Line2;
	  EXTI_InitStruct2.EXTI_LineCmd = ENABLE;
	  EXTI_InitStruct2.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStruct2.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_Init(&EXTI_InitStruct2);

 	  /* Add IRQ vector to NVIC */
	  //Prioritat a 1 al vector
	  NVIC_InitTypeDef NVIC_InitStruct2;
	  NVIC_InitStruct2.NVIC_IRQChannel = EXTI2_IRQn;
	  NVIC_InitStruct2.NVIC_IRQChannelPreemptionPriority = 0x01;
	  NVIC_InitStruct2.NVIC_IRQChannelSubPriority = 0x01;
	  NVIC_InitStruct2.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStruct2);

}

void INIT_USART(void){
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);


	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

	USART_InitStructure.USART_BaudRate = /*128000*/ 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

int main(void)
{
	waitTo = 1000000/velValue;
	interrupts = 0;
	wheelsOn = 0;
	periodMS[0]=-1;
	periodMS[1]=-1;
	numOverflows[0] = 0;
	numOverflows[1] = 0;
	numOverflowsOLD[0] = 0;
	numOverflowsOLD[1] = 0;


  INIT_IO_PRACTICA_1();
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  INIT_USART();

  //Configurar el botï¿½n
  STM_EVAL_PBInit(BUTTON_USER,BUTTON_MODE_GPIO);
  //Configurar el timer
  TIM_INT_Init();
  /* Infinite loop */
  while (1)
  {
	  if (STM_EVAL_PBGetState(BUTTON_USER)==1){
		  wheelsOn = 1-wheelsOn;
		  while(STM_EVAL_PBGetState(BUTTON_USER)==1){}
	  }
	  /*
	  if(USART_GetFlagStatus(USART1,USART_IT_RXNE)==SET)
	{
		uint16_t ucTemp = USART_ReceiveData(USART1);
		USART_SendData(USART1,ucTemp);
		STM_EVAL_LEDToggle(LED4);
	}*/

	 /*Codi de que si comencem a rebre llegim les tres primeres i despres tirem la config de la DMA*/
  }
}

/*
 * Callback used by stm324xg_eval_i2c_ee.c.
 * Refer to stm324xg_eval_i2c_ee.h for more info.
 */
uint32_t sEE_TIMEOUT_UserCallback(void)
{
  /* TODO, implement your code here */
  while (1)
  {
  }
}
