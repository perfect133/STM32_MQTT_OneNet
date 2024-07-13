#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "AD.h"
#include "Key.h"
#include "u8g2.h"
#include "esp8266.h"
#include "onenet.h"
#include "Menu.h"
#include "usart.h"
#include "Timer.h"
#include "LED.h"

uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
u8g2_t u8g2;
uint8_t display_flag=0;					//屏幕显示标志位

void u8g2_Config()
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c, u8g2_gpio_and_delay_stm32);  // 初始化 u8g2 结构体
	u8g2_InitDisplay(&u8g2); // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
	u8g2_SetPowerSave(&u8g2, 0); // 打开显示器
	u8g2_ClearBuffer(&u8g2);
}

void draw()											//u8g2图形库自带界面
{
    u8g2_SetFontMode(&u8g2, 1); /*字体模式选择*/
    u8g2_SetFontDirection(&u8g2, 0); /*字体方向选择*/
    u8g2_SetFont(&u8g2, u8g2_font_inb24_mf); /*字库选择*/
    u8g2_DrawStr(&u8g2, 0, 20, "U");
    
    u8g2_SetFontDirection(&u8g2, 1);
    u8g2_SetFont(&u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(&u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(&u8g2, 0);
    u8g2_SetFont(&u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(&u8g2, 51,30,"g");
    u8g2_DrawStr(&u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(&u8g2, 2, 35, 47);
    u8g2_DrawHLine(&u8g2, 3, 36, 47);
    u8g2_DrawVLine(&u8g2, 45, 32, 12);
    u8g2_DrawVLine(&u8g2, 46, 33, 12);
  
    u8g2_SetFont(&u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(&u8g2, 1,54,"github.com/olikraus/u8g2");
}
void OLED_Display()
{
	
	if(Key_GetNum()==1)
	{
		display_flag=!display_flag;
		u8g2_SetPowerSave(&u8g2, display_flag);
	}
}
void Hardware_Init(void)
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置									//systick初始化
	
	Usart1_Init(115200);		//串口1，驱动ESP8266用
	
	u8g2_DrawStr(&u8g2,1,10, " Hardware init OK\r\n");
	
}
void ALL_Init()
{
	I2C_Config();
	u8g2_Config();
	Show_Menu_Config();
	LED_Init();
	Beep_Init();
	AD_Init();
	Key_Init();
    
	const char *topics[] = {"Switch"};
	u8g2_ClearBuffer(&u8g2);
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266
	u8g2_SendBuffer(&u8g2); 
	while(OneNet_DevLink())			//接入OneNET
		Delay_ms(500);
//	LED1_ON();					//测试是否上电
	
	OneNet_Subscribe(topics, 1);
	
	u8g2_ClearBuffer(&u8g2); 
	do
	{
		draw();
	} while (u8g2_NextPage(&u8g2));
	u8g2_SendBuffer(&u8g2); 
	Delay_ms(1000);
	u8g2_ClearBuffer(&u8g2); 
	
	
	Timer_Init();
	LED2_OFF();
//	LED1_Turn();				//测试
}
int main(void)
{
	ALL_Init();
    
	while(1)
	{
		OLED_Display();					//OLED菜单显示
		Show_Menu();					//按键检测显示屏幕
		
    }
}

