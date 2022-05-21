/*
 * File:     main.c
 * Authors:  Alejandro Mo�ux
 *           Jos� Mar�a Fern�ndez L�pez
 *
 *
 * */

/* Includes */
//#include "stm32f4xx.h"
//#include "stm32f429i_discovery.h"
#include <DMA_usart_config.h>
#include <math.h>
#include <stm32f429i_discovery_lcd.h>
#include <stm32f429i_discovery_sdram.h>

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
int cycle = 0; //No se me ocurre como hacerlo ahora mismo
char state1 = 0;
char state2 = 0;
int velValue = 1000;
int waitTo = 0;
int period = 49;
int initPeriodVelocity = 340;
float periodScaler1 = 1; //PB7												//Esto es lo que se updatea -> 1 = max Freq -> n = posterior freq. en 0 no funca obviamente
float periodScaler2 = 1; //PE5
int periodVelocity1 = 0;
int periodVelocity2 = 0;
int duty_cycle1 = 0; //valor entre 0 y 100%    //PC9
int duty_cycle2 = 0; //valor entre 0 y 100%    //PC8
float dc1mult = 1; //multiplicador % del DC para cuando se activa el switch
float dc2mult = 1;
int counter = 0;
int subclock = 0;
char startPulsado=0;
char stop = 0;
float currentScaler1 = 0;
float currentScaler2 = 0;
float savedperiodScaler1 = 0;
float savedperiodScaler2 = 0;
/* Private function prototypes */
/* Private functions */

/*
 *
 *TIMER
 *CONFIG
 *
 */

void Velocity_Init(){
	////Timer 4 Generation////
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //PB7
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct_T4;

	TIM_TimeBaseInitStruct_T4.TIM_Prescaler = 2068;
	TIM_TimeBaseInitStruct_T4.TIM_Period = periodVelocity1;
	TIM_TimeBaseInitStruct_T4.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct_T4.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct_T4);



	TIM_Cmd(TIM4, ENABLE);
	////////
	////Timer 9 Generation////
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE); //PE5
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct_T9;

	TIM_TimeBaseInitStruct_T9.TIM_Prescaler = 4136;
	TIM_TimeBaseInitStruct_T9.TIM_Period = periodVelocity2;
	TIM_TimeBaseInitStruct_T9.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct_T9.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM9, &TIM_TimeBaseInitStruct_T9);



	TIM_Cmd(TIM9, ENABLE);
	////////

	////OC Timer 4////
	TIM_OCInitTypeDef TIM_OCBaseInitStruct_T4;
	TIM_OCBaseInitStruct_T4.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCBaseInitStruct_T4.TIM_Pulse = periodVelocity1/2;
	TIM_OCBaseInitStruct_T4.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCBaseInitStruct_T4.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC2Init(TIM4, &TIM_OCBaseInitStruct_T4);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	////////
	////OC Timer 9/////
	TIM_OCInitTypeDef TIM_OCBaseInitStruct_T9;
	TIM_OCBaseInitStruct_T9.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCBaseInitStruct_T9.TIM_Pulse = periodVelocity2/2;
	TIM_OCBaseInitStruct_T9.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCBaseInitStruct_T9.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM9, &TIM_OCBaseInitStruct_T9);
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
	////////


}

void PWM_Init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct_T3;
	TIM_OCInitTypeDef TIM_OCBaseInitStruct_T3;

	TIM_TimeBaseInitStruct_T3.TIM_Prescaler = 89;
	TIM_TimeBaseInitStruct_T3.TIM_Period = 49;
	TIM_TimeBaseInitStruct_T3.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct_T3.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct_T3);

	TIM_Cmd(TIM3, ENABLE);

  	TIM_OCBaseInitStruct_T3.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCBaseInitStruct_T3.TIM_Pulse = duty_cycle1;
	TIM_OCBaseInitStruct_T3.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCBaseInitStruct_T3.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM3, &TIM_OCBaseInitStruct_T3);
	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); //PC9
	TIM_OCBaseInitStruct_T3.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCBaseInitStruct_T3.TIM_Pulse = duty_cycle2;
	TIM_OCBaseInitStruct_T3.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCBaseInitStruct_T3.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC4Init(TIM3, &TIM_OCBaseInitStruct_T3);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
}
void TIM_INT_Init()
{
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
    TIM_TimeBaseInitStruct.TIM_Period = 499;
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
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x010;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x010;
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
		duty_cycle1 = period*dc1mult;
		duty_cycle2 = period*dc2mult;
		PWM_Init();
		stop = 0;
	}else{
		duty_cycle1 = period/2;
		duty_cycle2 = period/2;
		PWM_Init();
		stop = 1;
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
			int overflowDiff = 0;
			if(numOverflows[intIndex]-numOverflowsOLD[intIndex] < 0){
				overflowDiff = 0;
			}else{
				overflowDiff=numOverflows[intIndex]-numOverflowsOLD[intIndex];
			}
			periodMS[intIndex] = MAXINTVALUE*(overflowDiff) - periodMS[intIndex] + value;
		}else{
			periodMS[intIndex] = value - periodMS[intIndex];
		}
		float auxiliar = (1/(16*(periodMS[intIndex]/(float)TIME_MAGNITUTE_DENOMINATOR)));
		int output = ceil(auxiliar); //(int)(auxiliar*1000); //Calculem revolucions
		//Lo anterior lo hemos puesto sin el *1000 porque ya sale un num l�gico, digamos. Antes la resoluci�n era de ms y ahora es de us
		calcDebug[intIndex][calcDebugCounter[intIndex]] = output;
		periodMS[intIndex] = value;
		numOverflowsOLD[intIndex]=numOverflows[intIndex];
		calcDebugCounter[intIndex] = (calcDebugCounter[intIndex]+1)%11;
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
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  //INPUTS
      GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2|GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;
	  GPIO_Init(GPIOD, &GPIO_InitStructure);

	  GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM3);
	  GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);

	  GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_Init(GPIOE, &GPIO_InitStructure);



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

	USART_InitStructure.USART_BaudRate = 128000 /*9600*/;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

void initLCD(void){
	LCD_Init();
}

int main(void)
{
	currentScaler1 = periodScaler1;
	currentScaler2 = periodScaler2;
	periodVelocity1 = initPeriodVelocity*periodScaler1;
	periodVelocity2 = initPeriodVelocity*periodScaler2;
	duty_cycle1 = period; //valor entre 0 y 100%
	duty_cycle2 = period; //valor entre 0 y 100%
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

  //Configurar el bot�n
  STM_EVAL_PBInit(BUTTON_USER,BUTTON_MODE_GPIO);
  //Configurar el timer
  TIM_INT_Init();
  PWM_Init();
  Velocity_Init();
  /* Infinite loop */
  while (1)
  {
	  if(periodScaler1!=currentScaler1 || periodScaler2!=currentScaler2){
		  periodVelocity1 = initPeriodVelocity*periodScaler1;
		  periodVelocity2 = initPeriodVelocity*periodScaler2;
		  Velocity_Init();
		  currentScaler1 = periodScaler1;
		  currentScaler2 = periodScaler2;

	  }
	  if (STM_EVAL_PBGetState(BUTTON_USER)==1){
		  wheelsOn = 1-wheelsOn;
		  while(STM_EVAL_PBGetState(BUTTON_USER)==1){}
	  }
	  if(stop == 1){
	            if(savedperiodScaler1 == 0){
	                savedperiodScaler1 = periodScaler1;
	                savedperiodScaler2 = periodScaler2;
	            }
	            periodScaler1 = 0;
	            periodScaler2 = 0;
	        }else{
	            if(savedperiodScaler1 != 0){
	                periodScaler1 = savedperiodScaler1;
	                periodScaler2 = savedperiodScaler2;
	                savedperiodScaler1 = 0;
	                savedperiodScaler2 = 0;
	            }
	        }

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
