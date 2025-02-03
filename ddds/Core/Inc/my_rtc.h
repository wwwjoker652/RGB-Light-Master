#ifndef INC_MY_RTC_H_
#define INC_MY_RTC_H_
#include "stm32f1xx_hal.h"
#include "rtc.h"
#include "time.h"

HAL_StatusTypeDef MY_RTC_SetTime(struct tm *time);
struct tm *MY_RTC_GetTime(void);
void MY_RTC_Init(void);

#endif /* INC_MY_RTC_H_ */
