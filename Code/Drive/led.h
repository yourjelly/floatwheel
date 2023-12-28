#ifndef __LED_H
#define __LED_H

#include "hk32f030m.h"

#define  LED1_ON     GPIOA->BRR  = GPIO_Pin_1;
#define  LED1_OFF    GPIOA->BSRR = GPIO_Pin_1;
#define  LED1_FILP   GPIOA->ODR ^= GPIO_Pin_1;

#define  LED_B_ON    GPIOC->BRR  = GPIO_Pin_7; // Light On
#define  LED_B_OFF   GPIOC->BSRR = GPIO_Pin_7; // Light Off

#define  LED_F_ON    GPIOC->BRR  = GPIO_Pin_6; // Light On
#define  LED_F_OFF   GPIOC->BSRR = GPIO_Pin_6; // Light Off

extern uint16_t LED_Counter;
//extern uint16_t LED_Filp_Time;

void LED_Init(void);
//void LED1_Filp_Time(uint16_t filp_time);

#endif


