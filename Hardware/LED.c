#include "stm32f10x.h"                  // Device header
#define LED1_Pin GPIO_Pin_5
#define LED1_GPIO GPIOA

#define LED2_Pin GPIO_Pin_4
#define LED2_GPIO GPIOA
void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED1_Pin|LED2_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(LED1_GPIO, LED1_Pin);
	GPIO_ResetBits(LED2_GPIO, LED2_Pin);
}
void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure_Beep;
	GPIO_InitStructure_Beep.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure_Beep.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure_Beep.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure_Beep);
}
void Beep_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_13) == 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	}
}
void Beep_ON(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
}

void Beep_OFF(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_13);
}
void LED1_ON(void)
{
	GPIO_ResetBits(LED1_GPIO, LED1_Pin);
}

void LED1_OFF(void)
{
	GPIO_SetBits(LED1_GPIO, LED1_Pin);
}

void LED1_Turn(void)
{
	if (GPIO_ReadOutputDataBit(LED1_GPIO, LED1_Pin) == 0)
	{
		GPIO_SetBits(LED1_GPIO, LED1_Pin);
	}
	else
	{
		GPIO_ResetBits(LED1_GPIO, LED1_Pin);
	}
}
u8 LED1_ReadData(void)
{
	if (GPIO_ReadOutputDataBit(LED1_GPIO, LED1_Pin) == 0)
		return 0;
	else
		return 1;
}
u8 LED2_ReadData(void)
{
	if (GPIO_ReadOutputDataBit(LED2_GPIO, LED2_Pin) == 0)
		return 0;
	else
		return 1;
}
void LED2_ON(void)
{
	GPIO_ResetBits(LED2_GPIO, LED2_Pin);
}

void LED2_OFF(void)
{
	GPIO_SetBits(LED2_GPIO, LED2_Pin);
}

void LED2_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, LED2_Pin) == 0)
	{
		GPIO_SetBits(GPIOA, LED2_Pin);
	}
	else
	{
		GPIO_ResetBits(GPIOA, LED2_Pin);
	}
}
