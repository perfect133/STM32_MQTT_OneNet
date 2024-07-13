#include "Menu.h"
#include "Key.h"
#include "u8g2.h"
#include "picture.h"
#include <stdio.h>
#include "Delay.h"
#include "Serial.h"
#include "String.h"
#include "DHT11.h"
#include "AD.h"
#include "LED.h"

#include "stdio.h"
#include "onenet.h"
#include "esp8266.h"
//#define Str_Count 8
#define Menu_X 12
#define devPubTopic	"Sersor"
u16 Num = 0;
//u16 Hum_Dat = 0, Temp_Dat = 0;
char PubTopic_Str[65];
unsigned char *dataPtr = NULL;
u8 espReady = 0;
//FunctionalState Tim_flag = DISABLE;
struct Page GAME = {5, "Game", "game1:Serial", "game2:Dinosaur Rex", "game3:Stick Fight", "game4:Tetris"};
struct Page Serial = {5, "Serial", "BaudRate:115200", "WordLength:8", "StopBits:1", "ITConfig:ENABLE"};
struct Page SensorData = {5, "Sensor", "Temperature:   %", "Humidity:  .  'C", "Smog:  ", "Flame:  "}; // 12 9 5 6
struct Page SETTING = {4, "Setting", "UI_Speed:", "Sensor_Re:", "Early warning"};
struct Page Speed = {6, "Speed","LOW_SPEED","MID_SPEED","HIGH_SPEED","FAST_SPEED","SO_FAST_SPEED"};
struct Page Sersor_RE = {7, "Ser_Re","5 SEC","10 SEC","20 SEC","30 SEC","1 MIN","5 MIN"};
Speed_ENUM Speed_choose = so_fast_speed;

u8 Fire_warning = 0,Led_Flag = 0,Fan_Flag = 0;			//报警标记位

u16 Ser_Re = 10;
u8 Hum=0,Flame=0,Smog=0;								//传感器数据位
u16 Temp=0;

extern u8g2_t u8g2;
u16 display = 48;
u16 diaplay_trg = 1;
u8 circle_num;
u8 KeyNum;
static u8 Picture_Flag = 0; // 菜单标志位,0:"GAME",1:"MESSAGE",2:"SETTING"
const char words[][10] = {{"GAME\0"}, {"MESSAGE\0"}, {"SETTING\0"}};
u8 Menu_Key = 0;
char Box_x = 1;
char Box_y = 14;
char Box_w = 84;
char Box_h = 13;

char Box_x_trg;
char Box_y_trg;
char Box_w_trg;
char Box_h_trg;
u8 Box_Flag = 0;
void ui_run(char *a, char *a_trg, int b)
{
	if (*a < *a_trg)
	{
		*a += b;
		if (*a > *a_trg) // 大于预定值回正
			*a = *a_trg;
	}
	if (*a > *a_trg)
	{
		*a -= b;
		if (*a < *a_trg) // 小于预定值回正
			*a = *a_trg;
	}
}

void ui_right_one_Picture(int16_t *a, int b)
{
	uint8_t i = 0;
	if ((i <= 48))
	{
		*a += b;
		i += b;
	}
}

void ui_left_one_Picture(int16_t *a, int b)
{
	uint8_t i = 0;
	if ((i <= 48))
	{
		*a -= b;
		i += b;
	}
}

void Show_Menu_Config(void)
{
	u8g2_SetFontMode(&u8g2, 1);					  // 设置模式
	u8g2_SetFontDirection(&u8g2, 0);			  // 设置画笔
	u8g2_SetFont(&u8g2, u8g2_font_spleen6x12_mf); // 设置显示字体

	u8g2_DrawXBM(&u8g2, 44, 36, 40, 40, arrowhead);			// 箭头图标
	u8g2_DrawXBM(&u8g2, display, 16, 32, 32, game);			// 游戏图标
	u8g2_DrawXBM(&u8g2, display + 48, 16, 32, 32, wechat);	// 我的信息图标
	u8g2_DrawXBM(&u8g2, display + 96, 16, 32, 32, setting); // 设置图标
	u8g2_SendBuffer(&u8g2);									// u8g2发送字符
}

void Show_Menu() // 显示菜单
{
	KeyNum = AD_Key_GetNum();
	Menu_Key = KeyNum;

	if ((KeyNum == 1) && (display > -48))
	{
		Picture_Flag++;					// 菜单标志位"GAME","MESSAGE","SETTING"
		circle_num = 48 / Speed_choose; // ui速度
		while (circle_num)
		{
			u8g2_ClearBuffer(&u8g2); // u8g2清空刷新
			ui_left_one_Picture(&display, Speed_choose);
			u8g2_DrawXBM(&u8g2, 44, 36, 40, 40, arrowhead);
			u8g2_DrawXBM(&u8g2, display, 16, 32, 32, game);
			u8g2_DrawXBM(&u8g2, display + 48, 16, 32, 32, wechat);
			u8g2_DrawXBM(&u8g2, display + 96, 16, 32, 32, setting);

			u8g2_DrawStr(&u8g2, Menu_X, 10, "MENU:");
			u8g2_SendBuffer(&u8g2);

			circle_num--;
		}

		KeyNum = 0;
	}

	else if ((KeyNum == 2) && (display < 48))
	{
		Picture_Flag--;

		circle_num = 48 / Speed_choose;
		while (circle_num)
		{
			u8g2_ClearBuffer(&u8g2);
			ui_right_one_Picture(&display, Speed_choose);
			u8g2_DrawXBM(&u8g2, 44, 36, 40, 40, arrowhead);
			u8g2_DrawXBM(&u8g2, display, 16, 32, 32, game);
			u8g2_DrawXBM(&u8g2, display + 48, 16, 32, 32, wechat);
			u8g2_DrawXBM(&u8g2, display + 96, 16, 32, 32, setting);
			u8g2_DrawStr(&u8g2, Menu_X, 10, "MENU:");
			u8g2_SendBuffer(&u8g2);
			circle_num--;
		}
		KeyNum = 0;
	}
	else
		;

	switch (Picture_Flag)
	{
	case 0:
		u8g2_DrawStr(&u8g2, Menu_X, 10, "MENU:");
		u8g2_DrawStr(&u8g2, Menu_X + 30, 10, &words[0][0]); // 显示“GAME”
		u8g2_SendBuffer(&u8g2);
		Game_Menu();
		break;

	case 1:
		u8g2_DrawStr(&u8g2, Menu_X, 10, "MENU:");
		u8g2_DrawStr(&u8g2, Menu_X + 30, 10, &words[1][0]);
		u8g2_SendBuffer(&u8g2);
		DATA_Menu();
		break;

	case 2:
		u8g2_DrawStr(&u8g2, Menu_X, 10, "MENU:");
		u8g2_DrawStr(&u8g2, Menu_X + 30, 10, &words[2][0]);
		u8g2_SendBuffer(&u8g2);
		SET_Menu();
		break;
	}
}

void Page_To_Menu_Display(uint8_t Count_Menu, char **Menu_Str, uint8_t Count_Page, char **Page_Str) // 目录到目录显示
{
	char Game_Menu_Display = 10;
	char Game_Menu_Display_trg = 74;
	uint8_t Menu_Flag = 0;
	uint8_t Page_Flag = 0;

	while (Game_Menu_Display != Game_Menu_Display_trg)
	{
		Menu_Flag = 0;
		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2, Menu_X, Game_Menu_Display, Menu_Str[Menu_Flag++]); // Game_Menu_Display+16+62 Game图标
		u8g2_DrawLine(&u8g2, 1, Game_Menu_Display + 3, 128, Game_Menu_Display + 3);
		while (Menu_Flag < Count_Menu)
		{
			u8g2_DrawStr(&u8g2, 3, Game_Menu_Display + 2 + Menu_Flag * 12, Menu_Str[Menu_Flag]);
			Menu_Flag++;
		}
		ui_run(&Game_Menu_Display, &Game_Menu_Display_trg, Speed_choose);
		u8g2_SendBuffer(&u8g2); // u8g2字符串显示
	}

	Game_Menu_Display = 74;
	Game_Menu_Display_trg = 10;

	while (Game_Menu_Display != Game_Menu_Display_trg)
	{
		Page_Flag = 0;
		u8g2_ClearBuffer(&u8g2);
		ui_run(&Game_Menu_Display, &Game_Menu_Display_trg, Speed_choose);
		u8g2_DrawStr(&u8g2, Menu_X, Game_Menu_Display, Page_Str[Page_Flag++]); // 图像绘制
		while (Page_Flag < Count_Menu)
		{
			u8g2_DrawStr(&u8g2, 3, Game_Menu_Display + 2 + Page_Flag * 12, Page_Str[Page_Flag]);
			Page_Flag++;
		}
		u8g2_SendBuffer(&u8g2); //u8g2字符串显示
	}
}
void Menu_To_OnePage_Display(struct Page One_Page) // 菜单→一级目录显示
{
	char Game_Menu_Display = 10;
	char Game_Menu_Display_trg = 74;
	uint8_t Count;

	while (Game_Menu_Display != Game_Menu_Display_trg)
	{

		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2, Menu_X, Game_Menu_Display, "MENU:");				   
		u8g2_DrawXBM(&u8g2, 44, Game_Menu_Display + 26, 40, 40, arrowhead);		   // →图标
		u8g2_DrawXBM(&u8g2, display, Game_Menu_Display + 6, 32, 32, game);		   // 游戏图标
		u8g2_DrawXBM(&u8g2, display + 48, Game_Menu_Display + 6, 32, 32, wechat);  // 信息图标
		u8g2_DrawXBM(&u8g2, display + 96, Game_Menu_Display + 6, 32, 32, setting); // 设置图标
		ui_run(&Game_Menu_Display, &Game_Menu_Display_trg, Speed_choose);             
		u8g2_SendBuffer(&u8g2); // u8g2字符串显示
	}

	Game_Menu_Display = 74;
	Game_Menu_Display_trg = 10;

	while (Game_Menu_Display != Game_Menu_Display_trg)
	{
		Count = 0;
		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2, Menu_X, Game_Menu_Display, One_Page.Menu_Str[Count++]); // Game_Menu_Display+16+62Ϊ72����ʱ��Game����ʧ
		u8g2_DrawLine(&u8g2, 1, Game_Menu_Display + 3, 128, Game_Menu_Display + 3);
		while (Count < One_Page.Count)
		{
			u8g2_DrawStr(&u8g2, 3, Game_Menu_Display + 2 + Count * 12, One_Page.Menu_Str[Count]);
			Count++;
		}
		ui_run(&Game_Menu_Display, &Game_Menu_Display_trg, Speed_choose);
		u8g2_SendBuffer(&u8g2); // u8g2字符串显示
	}
}

void OnePage_To_Menu_Display(struct Page One_Page) // 一级目录→菜单显示
{
	char Game_Menu_Display = 10;
	char Game_Menu_Display_trg = 74;
	uint8_t Count;

	while (Game_Menu_Display != Game_Menu_Display_trg)
	{
		Count = 0;
		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2, Menu_X, Game_Menu_Display, One_Page.Menu_Str[Count++]); // Game_Menu_Display+16+62显示Game图标
		u8g2_DrawLine(&u8g2, 1, Game_Menu_Display + 3, 128, Game_Menu_Display + 3);
		while (Count < One_Page.Count)
		{
			u8g2_DrawStr(&u8g2, 3, Game_Menu_Display + 2 + Count * 12, One_Page.Menu_Str[Count]);
			Count++;
		}
		ui_run(&Game_Menu_Display, &Game_Menu_Display_trg, Speed_choose);
		u8g2_SendBuffer(&u8g2); // u8g2字符串显示
	}
	Game_Menu_Display = 74;
	Game_Menu_Display_trg = 10;

	while (Game_Menu_Display != Game_Menu_Display_trg)
	{
		u8g2_ClearBuffer(&u8g2);
		ui_run(&Game_Menu_Display, &Game_Menu_Display_trg, Speed_choose);
		u8g2_DrawStr(&u8g2, Menu_X, Game_Menu_Display, "MENU:");				   // Game_Menu_Display>=72显示MENU:
		u8g2_DrawXBM(&u8g2, 44, Game_Menu_Display + 26, 40, 40, arrowhead);		   // →图标
		u8g2_DrawXBM(&u8g2, display, Game_Menu_Display + 6, 32, 32, game);		   // 游戏图标
		u8g2_DrawXBM(&u8g2, display + 48, Game_Menu_Display + 6, 32, 32, wechat);  // 信息图标
		u8g2_DrawXBM(&u8g2, display + 96, Game_Menu_Display + 6, 32, 32, setting); // 设置图标
		u8g2_SendBuffer(&u8g2);													   // 图标载入
	}
}
void Game_Menu(void)				//GAME界面
{

	if (Menu_Key == 4) 
	{
		u8 Flag = 0;
		Menu_To_OnePage_Display(GAME);
		while (1)
		{
			Menu_Key = AD_Key_GetNum();
			u8g2_ClearBuffer(&u8g2);

			u8g2_DrawStr(&u8g2, Menu_X, 10, GAME.Menu_Str[0]);
			u8g2_DrawLine(&u8g2, 1, 13, 128, 13);
			u8g2_DrawStr(&u8g2, 3, 24, GAME.Menu_Str[1]);
			u8g2_DrawStr(&u8g2, 3, 36, GAME.Menu_Str[2]);
			u8g2_DrawStr(&u8g2, 3, 48, GAME.Menu_Str[3]);
			u8g2_DrawStr(&u8g2, 3, 60, GAME.Menu_Str[4]);

			ui_run(&Box_x, &Box_x_trg, 1);
			ui_run(&Box_y, &Box_y_trg, 1);
			ui_run(&Box_w, &Box_w_trg, 2);
			ui_run(&Box_h, &Box_h_trg, 1);

			u8g2_DrawFrame(&u8g2, Box_x, Box_y, Box_w, Box_h);
			u8g2_SendBuffer(&u8g2);

			if (Menu_Key == 4)
			{
				Box_Flag++;
				if (Box_Flag >= (GAME.Count-2))
					Box_Flag = (GAME.Count-2);
			}

			else if (Menu_Key == 3)
			{
				Box_Flag--;
				if (Box_Flag <= 0)
					Box_Flag = 0;
			}

			else if (Menu_Key == 2) 
			{
				u8g2_ClearBuffer(&u8g2);
				Box_Flag = 0;
				Box_x = 1;
				Box_y = 14;
				Box_w = 84;
				Box_h = 13;
				OnePage_To_Menu_Display(GAME);
				break;
			}
			switch (Box_Flag) 
			{
			case 0:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(GAME.Menu_Str[1]) + 1) * 6;
				Box_h_trg = 13;
				Serial_Menu();
				break;
			case 1:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(GAME.Menu_Str[2]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 2:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(GAME.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 3:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(GAME.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				break;
			}
		}
	}
	Menu_Key = 0;		
	Show_Menu_Config(); // 返回主菜单
	display = 48;
	Picture_Flag = 0;
}
//u8 Serial_ReFresh(void)
//{
//	char Str_Tim_Flag[8];
//	if(Tim_flag == DISABLE)
//		strcpy(Str_Tim_Flag,"DISABLE");
//	else
//		strcpy(Str_Tim_Flag,"ENABLE");
//	u8g2_DrawStr(&u8g2, (strlen(Serial.Menu_Str[3]) + 1) * 6, 60, Str_Tim_Flag);
//	return strlen(Str_Tim_Flag);
//}

void Serial_Menu(void)				//串口界面
{
	
	if (Menu_Key == 1) 
	{
		u8 Flag=0;
		
		Page_To_Menu_Display(GAME.Count, GAME.Menu_Str, Serial.Count, Serial.Menu_Str);
		while (1)
		{
			Menu_Key = AD_Key_GetNum();
			u8g2_ClearBuffer(&u8g2);

			u8g2_DrawStr(&u8g2, Menu_X, 10, Serial.Menu_Str[0]);
			u8g2_DrawLine(&u8g2, 1, 13, 128, 13);
			u8g2_DrawStr(&u8g2, 3, 24, Serial.Menu_Str[1]);
			u8g2_DrawStr(&u8g2, 3, 36, Serial.Menu_Str[2]);
			u8g2_DrawStr(&u8g2, 3, 48, Serial.Menu_Str[3]);
			u8g2_DrawStr(&u8g2, 3, 60, Serial.Menu_Str[4]);

			ui_run(&Box_x, &Box_x_trg, 1);
			ui_run(&Box_y, &Box_y_trg, 1);
			ui_run(&Box_w, &Box_w_trg, 2);
			ui_run(&Box_h, &Box_h_trg, 1);

			u8g2_DrawFrame(&u8g2, Box_x, Box_y, Box_w, Box_h);
			u8g2_SendBuffer(&u8g2);

			if (Menu_Key == 4)
			{
				Box_Flag++;
				if (Box_Flag >= (Serial.Count-2))
					Box_Flag = (Serial.Count-2);
				esp_ready();
			}

			else if (Menu_Key == 3)
			{
				Box_Flag--;
				if (Box_Flag <= 0)
					Box_Flag = 0;
			}

			else if (Menu_Key == 2)
			{
				u8g2_ClearBuffer(&u8g2);
				Box_Flag = 0;
				Box_x = 1;
				Box_y = 14;
				Box_w = 84;
				Box_h = 13;
				Page_To_Menu_Display(Serial.Count, Serial.Menu_Str, GAME.Count, GAME.Menu_Str);
				break;
			}
//			else if(Menu_Key == 1&& Box_Flag == 3)
//			{
//				Tim_flag = !Tim_flag;
//				TIM_Cmd(TIM2, Tim_flag);
//			}

			switch (Box_Flag) 
			{
			case 0:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Serial.Menu_Str[1]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 1:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Serial.Menu_Str[2]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 2:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Serial.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 3:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Serial.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				break;
			}
		}
	}
	Menu_Key = 0; 
	display = 48;
	Picture_Flag = 0;
}
u8 SET_Speed_ReFresh(void)			//ui速度转成字符串显示
{
	char Str_Speed[8];
	if(Speed_choose == low_speed)
		strcpy(Str_Speed,"LOW");
	else if(Speed_choose == mid_speed)
		strcpy(Str_Speed,"MID");
	else if(Speed_choose == high_speed)
		strcpy(Str_Speed,"HIGH");
	else if(Speed_choose == fast_speed)
		strcpy(Str_Speed,"FAST");
	else if(Speed_choose == so_fast_speed)
		strcpy(Str_Speed,"SO_FAST");
	u8g2_DrawStr(&u8g2, (strlen(SETTING.Menu_Str[1]) + 1) * 6, 24, Str_Speed);
	return strlen(Str_Speed);
}
u8 SET_Sec_ReFresh(void)			//刷新上传速度转成字符串显示
{
	char Str_Sec[8];
	if(Ser_Re == 5)
		strcpy(Str_Sec,"5 SEC");
	else if(Ser_Re == 10)
		strcpy(Str_Sec,"10 SEC");
	else if(Ser_Re == 20)
		strcpy(Str_Sec,"20 SEC");
	else if(Ser_Re == 30)
		strcpy(Str_Sec,"30 SEC");
	else if(Ser_Re == 60)
		strcpy(Str_Sec,"1 MIN");
	else if(Ser_Re == 300)
		strcpy(Str_Sec,"5 MIN");
	u8g2_DrawStr(&u8g2, (strlen(SETTING.Menu_Str[2]) + 1) * 6, 36, Str_Sec);
	return strlen(Str_Sec);
}

void SET_Menu(void)							//设置界面
{
	if (Menu_Key == 4) 
	{
		u8 Flag=0;
		u8 Len_Speed=0;
		u8 Len_Sec=0;
		Menu_To_OnePage_Display(SETTING);
		while (1)
		{
			Menu_Key = AD_Key_GetNum();

			u8g2_ClearBuffer(&u8g2);

			u8g2_DrawStr(&u8g2, Menu_X, 10, SETTING.Menu_Str[0]);
			u8g2_DrawLine(&u8g2, 1, 13, 128, 13);
			Len_Speed = SET_Speed_ReFresh();
			Len_Sec = SET_Sec_ReFresh();
			u8g2_DrawStr(&u8g2, 3, 24, SETTING.Menu_Str[1]);
			u8g2_DrawStr(&u8g2, 3, 36, SETTING.Menu_Str[2]);
			u8g2_DrawStr(&u8g2, 3, 48, SETTING.Menu_Str[3]);
//			u8g2_DrawStr(&u8g2, 3, 60, SETTING.Menu_Str[4]);	不满足四位选项时，应当选择性的显示内容

			ui_run(&Box_x, &Box_x_trg, 1);
			ui_run(&Box_y, &Box_y_trg, 1);
			ui_run(&Box_w, &Box_w_trg, 2);
			ui_run(&Box_h, &Box_h_trg, 1);

			u8g2_DrawFrame(&u8g2, Box_x, Box_y, Box_w, Box_h);
			u8g2_SendBuffer(&u8g2);

			if (Menu_Key == 4)
			{
				Box_Flag++;
				if (Box_Flag >= (SETTING.Count-2))
					Box_Flag = (SETTING.Count-2);
			}

			else if (Menu_Key == 3)
			{
				Box_Flag--;
				if (Box_Flag <= 0)
					Box_Flag = 0;
			}

			else if (Menu_Key == 2)
			{
				u8g2_ClearBuffer(&u8g2);
				Box_Flag = 0;
				Box_x = 1;
				Box_y = 14;
				Box_w = 84;
				Box_h = 13;
				OnePage_To_Menu_Display(SETTING);
				break;
			}

			switch (Box_Flag) 
			{
			case 0:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SETTING.Menu_Str[1]) + 1 + Len_Speed) * 6;
				Box_h_trg = 13;
				Speed_adjust_Page();
				break;
			case 1:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SETTING.Menu_Str[2]) + 1 + Len_Sec) * 6;
				Box_h_trg = 13;
				Sensor_Re_Page();
				break;
			case 2:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SETTING.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				break;
			}
		}
	}
	Menu_Key = 0; 
	Show_Menu_Config();
	display = -48;
	Picture_Flag = 2;
}

void Sersor_ReFresh()
{
	char strHum[3];
	char strTemp[5];
	char strSmog[3];
	char strFlame[3];
	sprintf(strHum, "%2d", Hum);
	sprintf(strTemp, "%2d.%1d ", Temp>>8, Temp & 0xFF);
	sprintf(strSmog, "%3d", Smog);
	sprintf(strFlame, "%3d", Flame);
	u8g2_DrawStr(&u8g2, (strlen(SensorData.Menu_Str[1]) - strlen(strHum) - 1) * 6, 24, strHum);

	u8g2_DrawStr(&u8g2, (strlen(SensorData.Menu_Str[2]) - strlen(strTemp) - 1) * 6, 36, strTemp);
	
	u8g2_DrawStr(&u8g2, (strlen(SensorData.Menu_Str[3]) - strlen(strSmog) + 1) * 6, 48, strSmog);
	
	u8g2_DrawStr(&u8g2, (strlen(SensorData.Menu_Str[4]) - strlen(strFlame) + 1) * 6, 60, strFlame);
}
void DATA_Menu(void)							//传感器数据界面
{
	if (Menu_Key == 4) // 摇杆下偏进入
	{
		u8 Flag=0;
		Menu_To_OnePage_Display(SensorData);
		while (1)
		{
			Menu_Key = AD_Key_GetNum();
			
			u8g2_ClearBuffer(&u8g2);

			u8g2_DrawStr(&u8g2, Menu_X, 10, SensorData.Menu_Str[0]);
			u8g2_DrawLine(&u8g2, 1, 13, 128, 13);
			
			u8g2_DrawStr(&u8g2, 3, 24, SensorData.Menu_Str[1]);
			u8g2_DrawStr(&u8g2, 3, 36, SensorData.Menu_Str[2]);
			u8g2_DrawStr(&u8g2, 3, 48, SensorData.Menu_Str[3]);
			u8g2_DrawStr(&u8g2, 3, 60, SensorData.Menu_Str[4]);
			
			
			
			Sersor_ReFresh();
			ui_run(&Box_x, &Box_x_trg, 1);
			ui_run(&Box_y, &Box_y_trg, 1);
			ui_run(&Box_w, &Box_w_trg, 2);
			ui_run(&Box_h, &Box_h_trg, 1);

			u8g2_DrawFrame(&u8g2, Box_x, Box_y, Box_w, Box_h);
			u8g2_SendBuffer(&u8g2);

			if (Menu_Key == 4)
			{
				Box_Flag++;
				if (Box_Flag >= (SensorData.Count-2))
					Box_Flag = (SensorData.Count-2);
			}

			else if (Menu_Key == 3)
			{
				Box_Flag--;
				if (Box_Flag <= 0)
					Box_Flag = 0;
			}

			else if (Menu_Key == 2)
			{
				u8g2_ClearBuffer(&u8g2);
				Box_Flag = 0;
				Box_x = 1;
				Box_y = 14;
				Box_w = 84;
				Box_h = 13;
				OnePage_To_Menu_Display(SensorData);
				break;
			}

			switch (Box_Flag)
			{
			case 0:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SensorData.Menu_Str[1]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 1:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SensorData.Menu_Str[2]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 2:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SensorData.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				break;
			case 3:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(SensorData.Menu_Str[4]) + 1) * 6;
				Box_h_trg = 13;
				break;
			}
		}
	}
	Menu_Key = 0; // 清空按键值
	Show_Menu_Config();
	display = 0;
	Picture_Flag = 1;
}
void Sensor_Re_Page(void)						//传感器刷新设置界面
{
	if (Menu_Key == 1) // 摇杆左偏进入
	{
		Box_Flag = 0;
		u8 Flag=0;
		u16 Ser_Re_Flag;
		Page_To_Menu_Display(SETTING.Count, SETTING.Menu_Str, Sersor_RE.Count, Sersor_RE.Menu_Str);
		while (1)
		{
			Menu_Key = AD_Key_GetNum();
			u8g2_ClearBuffer(&u8g2);

			u8g2_DrawStr(&u8g2, Menu_X, 10, Sersor_RE.Menu_Str[0]);
			u8g2_DrawLine(&u8g2, 1, 13, 128, 13);
			for(u8 i=1;i<5;i++)
			{
				u8g2_DrawStr(&u8g2, 3, 12+(12*i), Sersor_RE.Menu_Str[Flag+i]);
			}

			ui_run(&Box_x, &Box_x_trg, 1);
			ui_run(&Box_y, &Box_y_trg, 1);
			ui_run(&Box_w, &Box_w_trg, 4);
			ui_run(&Box_h, &Box_h_trg, 1);

			u8g2_DrawFrame(&u8g2, Box_x, Box_y, Box_w, Box_h);
			u8g2_SendBuffer(&u8g2);

			if (Menu_Key == 4)
			{
				Box_Flag++;
				if (Box_Flag >= (Sersor_RE.Count-2))
				{
					Box_Flag = (Sersor_RE.Count-2);
				}
				Flag=(Box_Flag - 3 - Flag)>0?Flag+1:Flag;
			}

			else if (Menu_Key == 3)
			{
				Box_Flag--;
				if (Box_Flag <= 0)
				{
					Box_Flag = 0;
				}
				Flag=(Box_Flag +1 - Flag)>0?Flag:Flag-1;
			}

			else if (Menu_Key == 2)
			{
				u8g2_ClearBuffer(&u8g2);
				Box_Flag = 0;
				Box_x = 1;
				Box_y = 14;
				Box_w = 84;
				Box_h = 13;
				Page_To_Menu_Display(Sersor_RE.Count, Sersor_RE.Menu_Str, SETTING.Count, SETTING.Menu_Str);
				break;
			}
			
			else if	(Menu_Key == 1)
			{
				Ser_Re = Ser_Re_Flag;
			}

			switch (Box_Flag) // 矩形框标志位
			{
			case 0:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Sersor_RE.Menu_Str[1]) + 1) * 6;
				Box_h_trg = 13;
				Ser_Re_Flag = 5;
				break;
			case 1:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Sersor_RE.Menu_Str[2]) + 1) * 6;
				Box_h_trg = 13;
				Ser_Re_Flag = 10;
				break;
			case 2:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Sersor_RE.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				Ser_Re_Flag = 20;
				break;
			case 3:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Sersor_RE.Menu_Str[4]) + 1) * 6;
				Box_h_trg = 13;
				Ser_Re_Flag = 30;
				break;
			case 4:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Sersor_RE.Menu_Str[5]) + 1) * 6;
				Box_h_trg = 13;
				Ser_Re_Flag = 60;
				break;
			case 5:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Sersor_RE.Menu_Str[6]) + 1) * 6;
				Box_h_trg = 13;
				Ser_Re_Flag = 300;
				break;
			}
		}
	}
	Menu_Key = 0; // 清空按键值
	display = -48;
	Picture_Flag = 2;
}

void Speed_adjust_Page(void)		//超四级选项页面，ui速度设置界面
{
	if (Menu_Key == 1) // 摇杆左偏进入
	{
		u8 Flag=0;
		Speed_ENUM Speed_Flag;
		Page_To_Menu_Display(SETTING.Count, SETTING.Menu_Str, Speed.Count, Speed.Menu_Str);
		while (1)
		{
			Menu_Key = AD_Key_GetNum();			//接收摇杆数据
			u8g2_ClearBuffer(&u8g2);

			u8g2_DrawStr(&u8g2, Menu_X, 10, Speed.Menu_Str[0]);
			u8g2_DrawLine(&u8g2, 1, 13, 128, 13);
			for(u8 i=1;i<5;i++)
			{
				u8g2_DrawStr(&u8g2, 3, 12+(12*i), Speed.Menu_Str[Flag+i]);
			}

			ui_run(&Box_x, &Box_x_trg, 1);
			ui_run(&Box_y, &Box_y_trg, 1);
			ui_run(&Box_w, &Box_w_trg, 4);
			ui_run(&Box_h, &Box_h_trg, 1);

			u8g2_DrawFrame(&u8g2, Box_x, Box_y, Box_w, Box_h);
			u8g2_SendBuffer(&u8g2);

			if (Menu_Key == 4)
			{
				Box_Flag++;
				if (Box_Flag >= (Speed.Count-2))
				{
					Box_Flag = (Speed.Count-2);
				}
				Flag=(Box_Flag - 3 - Flag)>0?Flag+1:Flag;
			}

			else if (Menu_Key == 3)
			{
				Box_Flag--;
				if (Box_Flag <= 0)
				{
					Box_Flag = 0;
				}
				Flag=(Box_Flag +1 - Flag)>0?Flag:Flag-1;
			}

			else if (Menu_Key == 2)
			{
				u8g2_ClearBuffer(&u8g2);
				Box_Flag = 0;
				Box_x = 1;
				Box_y = 14;
				Box_w = 84;
				Box_h = 13;
				Page_To_Menu_Display(Speed.Count, Speed.Menu_Str, SETTING.Count, SETTING.Menu_Str);
				break;
			}
			
			else if	(Menu_Key == 1)
			{
				Speed_choose = Speed_Flag;
			}

			switch (Box_Flag) // 矩形框标志位，速度调节
			{
			case 0:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Speed.Menu_Str[1]) + 1) * 6;
				Box_h_trg = 13;
				Speed_Flag = low_speed;
				break;
			case 1:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Speed.Menu_Str[2]) + 1) * 6;
				Box_h_trg = 13;
				Speed_Flag = mid_speed;
				break;
			case 2:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Speed.Menu_Str[3]) + 1) * 6;
				Box_h_trg = 13;
				Speed_Flag = high_speed;
				break;
			case 3:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Speed.Menu_Str[4]) + 1) * 6;
				Box_h_trg = 13;
				Speed_Flag = fast_speed;
				break;
			case 4:
				Box_x_trg = 1;
				Box_y_trg = 14+(Box_Flag-Flag)*12;
				Box_w_trg = (strlen(Speed.Menu_Str[5]) + 1) * 6;
				Box_h_trg = 13;
				Speed_Flag = so_fast_speed;
				break;
			
			}
		}
	}
	Menu_Key = 0; // 清空按键值
	display = -48;
	Picture_Flag = 2;
}
void esp_ready(void)					//开启数据接收
{
	espReady = 1;
	//espReady = !espReady;				//数据接收与关闭
}
void TIM2_IRQHandler(void)				//中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		Num++;
		if(Num>=Ser_Re)
		{
			
			DHT11_Read_Data(&Hum,&Temp);
			Smog = AD_GetValue(ADC_Channel_2);
			Flame = AD_GetValue(ADC_Channel_3);
			Led_Flag = LED1_ReadData();
			Fan_Flag = LED2_ReadData();
			if(Flame >= 180|| Smog >= 180)
			{
				Fire_warning = 1;
			}
			else
			{
				Fire_warning = 0;
			}
			Num=0;
			sprintf(PubTopic_Str,"{\"Hum\":%d,\"Temp\":%d.%d,\"Smog\":%d,\"Flame\":%d,\"Led\":%d,\"Fan\":%d}",Hum,Temp>>8,Temp&0xff,Smog,Flame,Led_Flag,Fan_Flag);
			
			OneNet_Publish(devPubTopic, PubTopic_Str);
			
			ESP8266_Clear();
			
		}
		if(espReady)
		{
			dataPtr = ESP8266_GetIPD(3);
			if(dataPtr != NULL)
				OneNet_RevPro(dataPtr);
		}
		if(Fire_warning == 1)
		{
			Beep_Turn();
		}
		else
		{
			Beep_OFF();
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}
