#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"

#define LOW_SPEED 1
#define MID_SPEED 3
#define HIGH_SPEED 4
#define FAST_SPEED 6
#define SO_FAST_SPEED 8

typedef enum
{
	low_speed = LOW_SPEED,
	mid_speed = MID_SPEED,
	high_speed = HIGH_SPEED,
	fast_speed = FAST_SPEED,
	so_fast_speed = SO_FAST_SPEED
} Speed_ENUM;
struct Page
{
	uint8_t Count;
	char *Menu_Str[8];
};

void ui_run(char *a, char *a_trg, int b);
void ui_right_one_Picture(int16_t *a, int b);
void ui_left_one_Picture(int16_t *a, int b);

void Show_Menu(void);
void Game_Menu(void);
void Show_Menu_Config(void);
void Serial_Menu(void);
void DATA_Menu(void);
void SET_Menu(void);

void Page_To_Menu_Display(uint8_t Count_Menu, char **Menu_Str, uint8_t Count_Page, char **Page_Str);
void OnePage_To_Menu_Display(struct Page One_Page);
void Menu_To_OnePage_Display(struct Page One_Page);

void Speed_adjust_Page(void);
void Sensor_Re_Page(void);
void esp_ready(void);


#endif
