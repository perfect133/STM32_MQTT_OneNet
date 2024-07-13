#include "u8g2.h"
#ifndef __My_IIC_H
#define __My_IIC_H

void IIC_Init(void);
uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
#endif
