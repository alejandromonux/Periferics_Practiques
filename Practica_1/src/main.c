/*
******************************************************************************
File:     main.c
Authors:  Alejandro Moñux
          José María Fernández López

******************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f429i_discovery.h"

/* variables */
static char interrupts;
char wheelsOn;
unsigned int miliseconds; //Used for the time calculation
int periodMS[2];
int duty_cycle1 = 50; //valor entre 0 y 100%
int duty_cycle2 = 50; //valor entre 0 y 100%
int cycle = 0;
char state1 = 0;
char state2 = 0;
int velValue = 1000;
int waitTo = 0;
int counter = 0;
int subclock = 0;


/*
 *
 *TIMER
 *CONFIG
 *
 */
void TIM_INT_Init()
{
    // Enable clock for TIM2
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);


    // default clock 84MHz
    // Update Event = timer_clock / ((Prescaler + 1) *  (Period + 1))
    // Update Event = 84MHz / ((299+ 1) * (279+ 1)) = 1000 Hz
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseInitStruct.TIM_Prescaler = 299;
    TIM_TimeBaseInitStruct.TIM_Period = 279;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;

    // TIM initialize
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
    // Enable TIM interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    // Start TIM
    TIM_Cmd(TIM2, ENABLE);

    //interrupt settings
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);




	// Enable clock for TIM4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

  // default clock 84MHz
  // Update Event = timer_clock / ((Prescaler + 1) *  (Period + 1))
	// Update Event (Hz) = 84MHz / ((299+ 1) * (279+ 1)) = 1000 Hz
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct_T3;
	TIM_TimeBaseInitStruct_T3.TIM_Prescaler = 0;
	TIM_TimeBaseInitStruct_T3.TIM_Period = 42;
	TIM_TimeBaseInitStruct_T3.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct_T3.TIM_CounterMode = TIM_CounterMode_Up;

	// TIM initialize
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct_T3);
	// Enable TIM interrupt
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	// Start TIM
	TIM_Cmd(TIM3, ENABLE);

	//interrupt settings
	NVIC_InitTypeDef NVIC_InitStruct_T3;
	NVIC_InitStruct_T3.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct_T3.NVIC_IRQChannelPreemptionPriority = 0X01;
	NVIC_InitStruct_T3.NVIC_IRQChannelSubPriority = 0X01;
	NVIC_InitStruct_T3.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct_T3);
}

void TIM3_IRQHandler(){
	if(cycle < duty_cycle1 && state1 == 0){
	    GPIO_SetBits(GPIOD, GPIO_Pin_3);
	    state1 = 1;
    }else if(cycle >= duty_cycle1 && state1 == 1){
	    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
	    state1 = 0;
    }
	if(cycle < duty_cycle2 && state2 == 0){
		GPIO_SetBits(GPIOD, GPIO_Pin_4);
		state2 = 1;
	}else if(cycle >= duty_cycle2 && state2 ==1){
		GPIO_ResetBits(GPIOD, GPIO_Pin_4);
		state2 = 0;
	}
    cycle++;
    if(cycle >100){
	    cycle = 0;
    }

    if(subclock>500){ //Counter de 1ms
    	subclock = 0;
    	if(waitTo == counter){
    		GPIO_ToggleBits(GPIOD, GPIO_Pin_5);
    		GPIO_ToggleBits(GPIOD, GPIO_Pin_6);
    		counter = 0;
    	}
    	counter++;

    }
    subclock++;

    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

void TIM2_IRQHandler() //RSI Timer2
{
	miliseconds++;

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
	//ALGO M�S QUE TENEMOS QUE PENSAR A�N
	// Netejem la flag
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    //}
}


/***
 * intIndex: 0 si EXTI1, 1 si EXTI2
 */

#define MAXINTVALUE 4294967295
int getRevs(int intIndex){
	if (periodMS[intIndex] == -1){
		periodMS[intIndex] = miliseconds;
		return -1;
	}else{
		//controla el overflow
		if (miliseconds < periodMS[intIndex]){
			periodMS[intIndex] = MAXINTVALUE - periodMS[intIndex] + miliseconds;
		}else{
			periodMS[intIndex] = miliseconds - periodMS[intIndex];
		}
		float auxiliar = (1/(16*(periodMS[intIndex]/(float)1000)));
		int output = (int)(auxiliar*1000); //Calculem revolucions
		periodMS[intIndex] = miliseconds;
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
	  //Outputs
	  GPIO_InitTypeDef GPIO_InitStructure;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //FIXME: Si no va poner pullup
	  GPIO_Init(GPIOD, &GPIO_InitStructure);
	  //INPUTS
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2;
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


	  //Prioritat a 1 al vector
	  NVIC_InitTypeDef NVIC_InitStruct;
	  NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
	  NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x04;
	  NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x04;
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


	  //Prioritat a 1 al vector
	  NVIC_InitTypeDef NVIC_InitStruct2;
	  NVIC_InitStruct2.NVIC_IRQChannel = EXTI2_IRQn;
	  NVIC_InitStruct2.NVIC_IRQChannelPreemptionPriority = 0x05;
	  NVIC_InitStruct2.NVIC_IRQChannelSubPriority = 0x05;
	  NVIC_InitStruct2.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStruct2);
}

int main(void)
{
	waitTo = 1000000/velValue;
	interrupts = 0;
	wheelsOn = 0;
	miliseconds = 0;
	periodMS[0]=-1;
	periodMS[1]=-1;

  INIT_IO_PRACTICA_1();
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  //Configurar el bot�n
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
