/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern char RxBuffer[256];
extern uint8_t Uart2_Rx_Cnt;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
void init(void);
uint8_t Button_LongPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void wifiswitch(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Sound_Pin GPIO_PIN_2
#define Sound_GPIO_Port GPIOA
#define Voltage_light_Pin GPIO_PIN_3
#define Voltage_light_GPIO_Port GPIOA
#define Red_light_Pin GPIO_PIN_6
#define Red_light_GPIO_Port GPIOA
#define Green_light_Pin GPIO_PIN_0
#define Green_light_GPIO_Port GPIOB
#define Blue_light_Pin GPIO_PIN_1
#define Blue_light_GPIO_Port GPIOB
#define DHT11_Pin GPIO_PIN_8
#define DHT11_GPIO_Port GPIOA
#define esp8266_TX_Pin GPIO_PIN_9
#define esp8266_TX_GPIO_Port GPIOA
#define esp8266_RX_Pin GPIO_PIN_10
#define esp8266_RX_GPIO_Port GPIOA
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
