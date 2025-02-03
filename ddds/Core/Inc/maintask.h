#ifndef __MAINTASK_H__
#define __MAINTASK_H__

#include "OLED.h"
#include "DHT11.h"
#include <stdio.h>
#include <string.h>
#include "esp.h"
#include "Flappy_Bird.h"
#include "my_rtc.h"
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#define LONG_PRESS_TIME 2000

void beforeall(void);
void mode1(void);
void mode2(void);
void setperiod(void);
void mode3(void);
void setpwm1(void);
void mode4(void);
void mode5(void);
void Display_Temperature_Humidity(void);
void setdht(void);
void mode6(void);
void mode7(void);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void wifiswitch(void);
void init(void);
void mode8(void);
void mode8_pre(void);
void setrgb(void);
void setvlight(void);
void switchmode(void);
void entermode(uint8_t x);
uint8_t Button_LongPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t Button_ShortPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void displayrtc(void);

#endif
