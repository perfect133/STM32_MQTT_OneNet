#include "stm32f10x.h"
#include "Delay.h"

#define DHT_Pin GPIO_Pin_12
#define DHT_GPIO GPIOB

//DHT11输出模式配置
void DHT11_IO_OUT()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(DHT_GPIO,&GPIO_InitStructure);	
}
 
//DHT11输入模式配置
void DHT11_IO_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT_Pin;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	 //上拉输入模式
	GPIO_Init(DHT_GPIO,&GPIO_InitStructure);	
}
void Drv_Dht11_Start(void)
{
	DHT11_IO_OUT(); // GPIO输出模式

	GPIO_SetBits(DHT_GPIO, DHT_Pin); // 拉高
	Delay_us(30);

	GPIO_ResetBits(DHT_GPIO, DHT_Pin); // 拉低
	Delay_ms(20);					   // 等待20ms

	GPIO_SetBits(DHT_GPIO, DHT_Pin); // 拉高等待
	Delay_us(30);

	 
}
u8 DHT11_Check() 	   
{   
	u8 retry=0;
	DHT11_IO_IN();//配置io口为输入模式 
    while (GPIO_ReadInputDataBit(DHT_GPIO, DHT_Pin)&&retry<100)//高电平循环，低电平跳出
	{
		retry++;
		Delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!GPIO_ReadInputDataBit(DHT_GPIO, DHT_Pin)&&retry<100)//DHT11拉低后会再次拉高40~50us
	{
		retry++;
		Delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

u8 DHT11_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(GPIO_ReadInputDataBit(DHT_GPIO, DHT_Pin)&&retry<100)//等待变为低电平 12-14us 开始
	{
		retry++;
		Delay_us(1);
	}
	retry=0;
	while(!GPIO_ReadInputDataBit(DHT_GPIO, DHT_Pin)&&retry<100)//等待变高电平	 26-28us表示0,116-118us表示1
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);//等待40us
	if(GPIO_ReadInputDataBit(DHT_GPIO, DHT_Pin))return 1;
	else return 0;		   
}
u8 DHT11_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_Read_Bit();
    }						    
    return dat;
}
void DHT11_Read_Data(u8 *humi,u16 *temp)    
{        
 	u8 buf[5];
	u8 i;
	Drv_Dht11_Start();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=(buf[2]<<8)|buf[3];
		}
		else
		{
			buf[0]=buf[1]=buf[2]=buf[3]=buf[4]=0;
		}
		
	}    
}

