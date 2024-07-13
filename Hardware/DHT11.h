#ifndef __DHT11_H
#define __DHT11_H
#include "stm32f10x.h"                  // Device header
 
void DHT11_IO_IN(void);
void DHT11_IO_OUT(void);
void DHT11_Read_Data(u8 *humi,u16 *temp);

 
#endif
