#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "AD.h"
uint16_t AD0, AD1;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t AD_Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	AD0 = AD_GetValue(ADC_Channel_0);
	AD1 = AD_GetValue(ADC_Channel_1);
	if(AD0>3600)
	{
		Delay_ms(50);
		while(AD_GetValue(ADC_Channel_0)>3600);
		Delay_ms(20);
		KeyNum = 1;
	}else if(AD0<400)
	{
		Delay_ms(50);
		while(AD_GetValue(ADC_Channel_0)<400);
		Delay_ms(20);
		KeyNum = 2;
	}else if(AD1>3600)
	{
		Delay_ms(50);
		while(AD_GetValue(ADC_Channel_1)>3600);
		Delay_ms(20);
		KeyNum = 3;
	}else if(AD1<400)
	{
		Delay_ms(50);
		while(AD_GetValue(ADC_Channel_1)<400);
		Delay_ms(20);
		KeyNum = 4;
	}
	return KeyNum;
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0);
		Delay_ms(20);
		KeyNum = 2;
	}
	
	return KeyNum;
}

