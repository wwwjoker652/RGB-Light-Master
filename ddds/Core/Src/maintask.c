#include "maintask.h"
#define RXBUFFERSIZE  256

uint32_t ptime1 = 1000;
uint32_t ptime2 = 500;
uint32_t pwmtime1 = 10;
uint32_t light = 50;
uint32_t tempr = 30;
uint32_t R = 125;
uint32_t G = 125;
uint32_t B = 125;
uint32_t R1 = 0;
uint32_t G1 = 0;
uint32_t B1 = 0;
uint32_t vlight = 1863;
uint8_t Button_LongPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t Button_ShortPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t temperature = 0;
uint8_t humidity = 0;
uint8_t stat = 1;
uint8_t choice = 1;
uint32_t pg = 1;
uint8_t count = 0;
extern uint8_t Data[5];
extern DMA_HandleTypeDef hdma_usart1_rx;
uint8_t receive[50];
char RxBuffer[RXBUFFERSIZE]; 
uint8_t aRxBuffer;
uint8_t Uart2_Rx_Cnt = 0;	
char my_order[15]={0};
char receive_flag=0;
char weeks[7][10] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
uint8_t modes[11] = {0,1,1,1,1,1,1,1,1,1,1};
struct tm setting;
uint8_t is_running = 0;
uint32_t ms_counter = 0;

void beforeall(void){

	OLED_Init();
	OLED_Clear();
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	HAL_ADC_Start(&hadc2);
	MY_RTC_Init();
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1);
	esp8266_start_trans();
	OLED_Clear();
}

void init(void){
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	while (1)
  {
		displayrtc();
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<wifi>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				wifiswitch();
			}
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			switchmode();
		}
	}
}

void switchmode(void){
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	OLED_Clear();
	while(1){
		if(pg){
			OLED_ShowString(0, 0, "Mode1", 16, modes[0]);
			OLED_ShowString(0, 20, "Mode2", 16, modes[1]);
			OLED_ShowString(0, 40, "Mode3", 16, modes[2]);
			OLED_ShowString(80, 0, "Mode4", 16, modes[3]);
			OLED_ShowString(80, 20, "Mode5", 16, modes[4]);
			OLED_ShowString(80, 40, "Mode6", 16, modes[5]);
			OLED_Refresh();
			if (Button_ShortPress(GPIOA, GPIO_PIN_11)){
				modes[choice - 1] = 1;
				choice++;
				modes[choice - 1] = 0;
			}
			if (Button_ShortPress(GPIOA, GPIO_PIN_12)){
				if(choice == 1){
					choice = 2;
					count++;
				}
				modes[choice - 1] = 1;
				choice--;
				modes[choice - 1] = 0;
			}
			if(count){
				count = 0;
				modes[0] = 1;
				modes[10] = 0;
				choice = 11;
			}
			if (Button_LongPress(GPIOA, GPIO_PIN_15)){
				entermode(choice);
			}
			if(choice > 6){
				pg = !pg;
				OLED_Clear();
			}
		}else{
			OLED_ShowString(0, 0, "Mode7", 16, modes[6]);
			OLED_ShowString(0, 20, "Mode8", 16, modes[7]);
			OLED_ShowString(0, 40, "Wifi", 16, modes[8]);
			OLED_ShowString(80, 0, "Game", 16, modes[9]);
			OLED_ShowString(80, 20, "Timer", 16, modes[10]);
			OLED_Refresh();
			if (Button_ShortPress(GPIOA, GPIO_PIN_11)){
				if(choice == 11){
					choice = 10;
					count++;
				}
				modes[choice - 1] = 1;
				choice++;
				modes[choice - 1] = 0;
			}
			if (Button_ShortPress(GPIOA, GPIO_PIN_12)){
				modes[choice - 1] = 1;
				choice--;
				modes[choice - 1] = 0;
			}
			if(count){
				count = 0;
				choice = 1;
				modes[10] = 1;
				modes[0] = 0;
			}
			if (Button_LongPress(GPIOA, GPIO_PIN_15)){
				entermode(choice);
			}
			if(choice < 7){
				pg = !pg;
				OLED_Clear();
			}
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			init();
		}
	}
}

void entermode(uint8_t x){
	if(x == 1){
		mode1();
	}else if(x == 2){
		mode2();
	}else if(x == 3){
		mode3();
	}else if(x == 4){
		mode4();
	}else if(x == 5){
		mode5();
	}else if(x == 6){
		mode6();
	}else if(x == 7){
		mode7();
	}else if(x == 8){
		mode8_pre();
	}else if(x == 9){
		wifiswitch();
	}else if(x == 10){
		Flappy_Bird_Init();
	}else if(x == 11){
		timer();
	}
}

uint8_t Button_LongPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET) 
    {
        HAL_Delay(1000); 
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
        {
            return 1; 
        }
    }
    return 0;
}

uint8_t Button_ShortPress(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET) 
    {
        HAL_Delay(100); 
        if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET)
        {
            return 1; 
        }
    }
    return 0;
}

void mode1(){
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "Button mode!", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(10, 20, "Press OK to", 16, 1);
	OLED_ShowString(10, 40, "turn on the light", 12, 1);
	OLED_Refresh();
	while(1){
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET){
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,100);
			OLED_Clear();
			OLED_ShowString(10, 20, "Release OK to", 16, 1);
			OLED_ShowString(10, 40, "turn off the light", 12, 1);
			OLED_Refresh();
		}else{
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
			OLED_Clear();
			OLED_ShowString(10, 20, "Press OK to", 16, 1);
			OLED_ShowString(10, 40, "turn on the light", 12, 1);
			OLED_Refresh();
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			break;
		}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<switch>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				break;
			}
		}
	}
	OLED_Clear();
	OLED_ShowString(20, 20, "Swiching...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(10, 20, "Press key to", 16, 1);
	OLED_ShowString(10, 40, "turn off the light", 12, 1);
	OLED_Refresh();
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
	while(1){
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET){
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
			OLED_Clear();
			OLED_ShowString(10, 20, "Release OK to", 16, 1);
			OLED_ShowString(10, 40, "turn on the light", 12, 1);
			OLED_Refresh();
		}else{
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,100);
			OLED_Clear();
			OLED_ShowString(10, 20, "Press OK to", 16, 1);
			OLED_ShowString(10, 40, "turn off the light", 12, 1);
			OLED_Refresh();
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			break;
		}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<switch>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				break;
			}
		}
	}
	OLED_Clear();
	OLED_ShowString(20, 20, "Swiching...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	OLED_Clear();
	OLED_ShowString(10, 20, "Press OK to", 16, 1);
	OLED_ShowString(10, 40, "turn on the light", 12, 1);
	OLED_Refresh();
	uint8_t pin0_status = 0;
	while (1) {
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) {
        HAL_Delay(50); 
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) {
            pin0_status = !pin0_status;
            if(pin0_status){
							__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,100);
							OLED_Clear();
							OLED_ShowString(10, 20, "Press OK to", 16, 1);
							OLED_ShowString(10, 40, "turn off the light", 12, 1);
							OLED_Refresh();
						}else{
							__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
							OLED_Clear();
							OLED_ShowString(10, 20, "Press OK to", 16, 1);
							OLED_ShowString(10, 40, "turn on the light", 12, 1);
							OLED_Refresh();
						}
            while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) {
                HAL_Delay(10);
            }
        }
    }

    if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
        switchmode();
    }
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
				wifiswitch();
			}
		}
	}
}

void mode2(void){
	uint32_t lastTimeLed1 = 0;
  uint32_t lastTimeLed2 = 0;
	uint32_t time1 = 0;
	uint32_t time2 = 0;
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "Period mode!", 16, 1);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	OLED_Refresh();
	while (1) {
        uint32_t currentTime = HAL_GetTick();
        if (currentTime - lastTimeLed1 >= ptime1) {
            if(time1 == 0){
							time1 = 100;
						}else{
							time1 = 0;
						}
						__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,time1);
            lastTimeLed1 = currentTime; 
        }
        if (currentTime - lastTimeLed2 >= ptime2) {
            if(time2 == 0){
							time2 = 100;
						}else{
							time2 = 0;
						}
						__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,time2);
            lastTimeLed2 = currentTime; 
        }
				if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
					setperiod();
				}
				if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
					switchmode();
				}
				if(receive_flag)
				{
					receive_flag=0;
					if(strstr((const char*)my_order,(const char*)"<set>"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));
						esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
						esp8266_send_cmd("OK!\n","OK",50);
						setperiod();
					} else if(strstr((const char*)my_order,(const char*)"<exit>"))
				{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				wifiswitch();
				}
			}
    }
}

void setperiod(void){
	uint8_t status = 1;
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
	while(1){
		if(status){
			OLED_Clear();
			OLED_ShowString(20, 20, "Setting Green...", 12, 1);
			OLED_ShowString(20, 40, "Current:", 12, 1);
			OLED_ShowString(100, 40, "ms", 12, 1);
			uint8_t num_digits = 0;
			uint32_t temp = ptime1;
			while (temp > 0) {
					temp /= 10;
					num_digits++;
			}
			if (num_digits == 0) num_digits = 1; 
			OLED_ShowNum(70, 40, ptime1, num_digits, 12, 1);
			OLED_Refresh();
			HAL_Delay(500);
		}
		if(!status){
			OLED_Clear();
			OLED_ShowString(20, 20, "Setting Blue...", 12, 1);
			OLED_ShowString(20, 40, "Current:", 12, 1);
			OLED_ShowString(100, 40, "ms", 12, 1);
			uint8_t num_digits = 0;
			uint32_t temp = ptime2;
			while (temp > 0) {
					temp /= 10;
					num_digits++;
			}
			if (num_digits == 0) num_digits = 1;
			OLED_ShowNum(70, 40, ptime2, num_digits, 12, 1);
			OLED_Refresh();
			HAL_Delay(500);
		}
		if (Button_ShortPress(GPIOB, GPIO_PIN_14)) {
			if(status){
				ptime1 += 500;
			}else{
				ptime2 += 500;
			}
		}
		if (Button_ShortPress(GPIOB, GPIO_PIN_15)) {
			if(status){
				ptime1 -= 500;
			}else{
				ptime2 -= 500;
			}
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
			mode2();
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
			status = !status;
		}
		if(receive_flag)
		{
			int received_value = 0;
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<+>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				if(status){
					ptime1 += 500;
				}else{
					ptime2 += 500;
				}
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
			}else if(strstr((const char*)my_order,(const char*)"<->"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				if(status){
					ptime1 -= 500;
				}else{
					ptime2 -= 500;
				}
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
			}else if(strstr((const char*)my_order,(const char*)"<switch>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				status = !status;
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
			}
			if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				mode2();
			}else if(sscanf((const char*)my_order, "<%d>", &received_value) == 1)
					{
						if(received_value >= 0 && received_value <= 9999) // 示例范围限制
						{
								Uart2_Rx_Cnt = 0;
								memset(RxBuffer, 0x00, sizeof(RxBuffer));
								memset(my_order, 0x00, sizeof(my_order));
								if(status){
									ptime1 = received_value;
								}else{
									ptime2 = received_value;
								}
								esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
								esp8266_send_cmd("OK!\n","OK",50);
						}
			}
		}
	}
}

void mode3(void){
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "Breathing light!", 12, 1);
	OLED_Refresh();
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	while(1){
		for(int i = 0; i<100;i++){
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,i);
			HAL_Delay(pwmtime1);
			if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
					setpwm1();
				}
			if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,50);
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			OLED_Clear();
			switchmode();
				}
		}
		for(int i = 99; i>=0;i--){
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,i);
			HAL_Delay(pwmtime1);
			if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
					setpwm1();
				}
			if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,50);
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			OLED_Clear();
			switchmode();
			}
		}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<set>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				setpwm1();
			}else if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);				
				wifiswitch();
			}
		}
	}
}

void setpwm1(void){
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
	while(1){
		OLED_Clear();
		OLED_ShowString(20, 10, "Setting the", 12, 1);
		OLED_ShowString(20, 20, "breathing light...", 12, 1);
		OLED_ShowString(20, 30, "Current:", 12, 1);
		OLED_ShowString(90, 30, "ms", 12, 1);
		uint8_t num_digits = 0;
		uint32_t temp = pwmtime1;
		while (temp > 0) {
				temp /= 10;
				num_digits++;
		}
		if (num_digits == 0) num_digits = 1;
		OLED_ShowNum(70, 30, pwmtime1, num_digits, 12, 1);
		OLED_ShowString(20, 40, "Total:", 12, 1);
		OLED_ShowNum(70, 40, pwmtime1 * 200, 6, 12, 1);
		OLED_ShowString(110, 40, "ms", 12, 1);
		OLED_Refresh();
		HAL_Delay(500);
		if (Button_ShortPress(GPIOB, GPIO_PIN_14)) {
			  pwmtime1 += 5;
		}
			if (Button_ShortPress(GPIOB, GPIO_PIN_15)) {
			  pwmtime1 -= 5;
		}
			if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
			mode3();
			}
			if(receive_flag)
			{
				int received_value = 0;
				receive_flag=0;
				if(strstr((const char*)my_order,(const char*)"<+>"))
				{
					Uart2_Rx_Cnt=0;
					memset(RxBuffer,0x00,sizeof(RxBuffer));
					memset(RxBuffer,0x00,sizeof(my_order));
					pwmtime1 += 5;
					esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
					esp8266_send_cmd("OK!\n","OK",50);
				}else if(strstr((const char*)my_order,(const char*)"<->"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));
						pwmtime1 -= 5;
						esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
						esp8266_send_cmd("OK!\n","OK",50);
					}else if(strstr((const char*)my_order,(const char*)"<exit>"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));	
						esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
						esp8266_send_cmd("Exiting...\n","OK",50);
						mode3();
					}else if(sscanf((const char*)my_order, "<%d>", &received_value) == 1)
					{
						if(received_value >= 0 && received_value <= 9999) // 示例范围限制
						{
								Uart2_Rx_Cnt = 0;
								memset(RxBuffer, 0x00, sizeof(RxBuffer));
								memset(my_order, 0x00, sizeof(my_order));
								
								pwmtime1 = received_value; // 使用接收到的值设置vlight
								esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
								esp8266_send_cmd("OK!\n","OK",50);
						}
					}
			}
	}
}

void mode4(void){
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "Changable light!", 12, 1);
	OLED_Refresh();
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,50);
	while(1){
		if (Button_ShortPress(GPIOB, GPIO_PIN_14)) {
			light += 5;
			if(light >= 100){
				light = 95;
			}
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,light);
		}
		if (Button_ShortPress(GPIOB, GPIO_PIN_15)) {
			light -= 5;
			if(light == 0){
				light = 5;
			}
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,light);
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			switchmode();
		}
		if(receive_flag)
			{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<+>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				light += 5;
				if(light >= 100){
				light = 95;
				}
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,light);
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
			}else if(strstr((const char*)my_order,(const char*)"<->"))
				{
					Uart2_Rx_Cnt=0;
					memset(RxBuffer,0x00,sizeof(RxBuffer));
					memset(RxBuffer,0x00,sizeof(my_order));
					light -= 5;
					if(light == 0){
					light = 5;
					}
					__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,light);
					esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
					esp8266_send_cmd("OK!\n","OK",50);
				}else if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				wifiswitch();
			}
		}
	}
}

void mode5(void){
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 10, "Voltage light!", 12, 1);
	OLED_Refresh();
	HAL_ADCEx_Calibration_Start(&hadc1);
	while (1){
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
		OLED_ShowString(20, 30, "ADC Value:", 12, 1);
		OLED_ShowNum(80, 30, adc_value, 4, 12, 1);
		OLED_ShowString(20, 50, "Voltage:", 12, 1);
		float voltagei = ((float)adc_value/4095)*3.3;
		uint32_t voltagef = (int)(voltagei * 1000) % 1000;
		OLED_ShowNum(75, 50, voltagei, 1, 12, 1);
		OLED_ShowString(80, 50, ".", 12, 1);
		OLED_ShowNum(86, 50, voltagef, 3, 12, 1);
		OLED_Refresh();
		if (adc_value > vlight) {
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,100);
		} else {
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
		}
		HAL_Delay(500);
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
			OLED_Clear();
			switchmode();
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_13)){
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
			setvlight();
		}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
				OLED_Clear();
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				wifiswitch();
			}else if(strstr((const char*)my_order,(const char*)"<set>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
				OLED_Clear();
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				setvlight();
			}
		}
	}
}

void setvlight(void){
	OLED_Clear();
	while(1){
		OLED_ShowString(20, 10, "Setting the", 12, 1);
		OLED_ShowString(20, 20, "Voltage light...", 12, 1);
		OLED_ShowString(20, 30, "Current:", 12, 1);
		uint8_t num_digits = 0;
		uint32_t temp = vlight;
		while (temp > 0) {
				temp /= 10;
				num_digits++;
		}
		if (num_digits == 0) num_digits = 1;
		OLED_ShowNum(70, 30, vlight, 4, 12, 1);
		OLED_ShowString(20, 40, "Voltage:", 12, 1);
		float voltagei = ((float)vlight/4095)*3.3;
		uint32_t voltagef = (int)(voltagei * 1000) % 1000;
		OLED_ShowNum(75, 40, voltagei, 1, 12, 1);
		OLED_ShowString(80, 40, ".", 12, 1);
		OLED_ShowNum(86, 40, voltagef, 3, 12, 1);
		OLED_Refresh();
		HAL_Delay(500);
		if (Button_ShortPress(GPIOB, GPIO_PIN_14)) {
			  vlight += 10;
		}
			if (Button_ShortPress(GPIOB, GPIO_PIN_15)) {
			  vlight -= 10;
		}
			if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
			mode5();
		}
			if(receive_flag)
			{
				int received_value = 0;
				receive_flag=0;
				if(strstr((const char*)my_order,(const char*)"<+>"))
				{
					Uart2_Rx_Cnt=0;
					memset(RxBuffer,0x00,sizeof(RxBuffer));
					memset(RxBuffer,0x00,sizeof(my_order));
					vlight += 10;
					esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
					esp8266_send_cmd("OK!\n","OK",50);
				}else if(strstr((const char*)my_order,(const char*)"<->"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));
						vlight -= 10;
						esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
						esp8266_send_cmd("OK!\n","OK",50);
					}else if(strstr((const char*)my_order,(const char*)"<exit>"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));
						esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
						esp8266_send_cmd("Exiting...\n","OK",50);						
						mode5();
					}else if(sscanf((const char*)my_order, "<%d>", &received_value) == 1)
					{
						if(received_value >= 0 && received_value <= 4095) // 示例范围限制
						{
								Uart2_Rx_Cnt = 0;
								memset(RxBuffer, 0x00, sizeof(RxBuffer));
								memset(my_order, 0x00, sizeof(my_order));
								
								vlight = received_value; // 使用接收到的值设置vlight
								esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
								esp8266_send_cmd("OK!\n","OK",50);
						}
			}
		}
	}
}

void Display_Temperature_Humidity(void)
{
	DHT_Read();
	humidity = Data[0]; 
	HAL_Delay(200);
	DHT_Read();
	temperature = Data[2]; 
	OLED_ShowNum(48, 40, humidity, 2, 12, 1);
	OLED_ShowChar(64, 40, '%', 12, 1);
	OLED_ShowNum(48, 50,temperature , 2, 12, 1);
	OLED_ShowChar(64, 50, 'C', 12, 1);
	OLED_Refresh();
}

void mode6(void){
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "Temp light!", 12, 1);
	OLED_Refresh();
	while(1){
		Display_Temperature_Humidity();
		if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
					setdht();
				}
		if (temperature > tempr) {
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,100);
		} else {
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			OLED_Clear();
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
			switchmode();
		}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<set>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				setdht();
			}else if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				OLED_Clear();
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				wifiswitch();
			}
		}
	}
}

void setdht(void){
	while(1){
		OLED_Clear();
		OLED_ShowString(20, 20, "Setting the", 12, 1);
		OLED_ShowString(20, 30, "Temp light...", 12, 1);
		OLED_ShowString(20, 40, "Current:", 12, 1);
		OLED_ShowString(90, 40, "C", 12, 1);
		uint8_t num_digits = 0;
		uint32_t temp = tempr;
		while (temp > 0) {
				temp /= 10;
				num_digits++;
		}
		if (num_digits == 0) num_digits = 1;
		OLED_ShowNum(70, 40, tempr, num_digits, 12, 1);
		OLED_Refresh();
		HAL_Delay(500);
		if (Button_ShortPress(GPIOB, GPIO_PIN_14)) {
			  tempr += 1;
		}
			if (Button_ShortPress(GPIOB, GPIO_PIN_15)) {
			  tempr -= 1;
		}
			if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
			mode6();
		}
			if(receive_flag)
			{
				int received_value = 0;
				receive_flag=0;
				if(strstr((const char*)my_order,(const char*)"<+>"))
				{
					Uart2_Rx_Cnt=0;
					memset(RxBuffer,0x00,sizeof(RxBuffer));
					memset(RxBuffer,0x00,sizeof(my_order));
					tempr += 1;
					esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
					esp8266_send_cmd("OK!\n","OK",50);
				}else if(strstr((const char*)my_order,(const char*)"<->"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));
						tempr -= 1;
						esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
						esp8266_send_cmd("OK!\n","OK",50);
					}else if(strstr((const char*)my_order,(const char*)"<exit>"))
					{
						Uart2_Rx_Cnt=0;
						memset(RxBuffer,0x00,sizeof(RxBuffer));
						memset(RxBuffer,0x00,sizeof(my_order));
						esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
						esp8266_send_cmd("Exiting...\n","OK",50);						
						mode6();
					}else if(sscanf((const char*)my_order, "<%d>", &received_value) == 1)
					{
						if(received_value >= 0 && received_value <= 99) // 示例范围限制
						{
								Uart2_Rx_Cnt = 0;
								memset(RxBuffer, 0x00, sizeof(RxBuffer));
								memset(my_order, 0x00, sizeof(my_order));
								tempr = received_value; // 使用接收到的值设置vlight
								esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
								esp8266_send_cmd("OK!\n","OK",50);
						}
					}
			}
	}
}

void mode7(void){
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,50);
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "Sound light!", 12, 1);
	OLED_ShowString(20, 40, "ADC Value:", 12, 1);
	OLED_Refresh();
	while(1){
		HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
		uint32_t sound_value = HAL_ADC_GetValue(&hadc2);
		OLED_ShowNum(80, 40, sound_value, 4, 12, 1);
		OLED_Refresh();
		uint32_t temp1 = (sound_value *100) / 4095 ;
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,temp1);
		HAL_Delay(500);
		if (Button_LongPress(GPIOB, GPIO_PIN_11)){
			OLED_Clear();
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			switchmode();
		}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				OLED_Clear();
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				wifiswitch();
			}
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file*/
	if(Uart2_Rx_Cnt >= 255) 
	{
		Uart2_Rx_Cnt = 0;
		memset(RxBuffer,0x00,sizeof(RxBuffer));	        
	}
	else
	{
		static uint8_t Uart2_count=0;
		
		RxBuffer[Uart2_Rx_Cnt] = aRxBuffer;
		if(receive_flag==0)
		{
		if(RxBuffer[Uart2_Rx_Cnt-Uart2_count]=='<')
		{	
			Uart2_count++;
		if((RxBuffer[Uart2_Rx_Cnt]=='>')||Uart2_count>=14)
		{
			uint8_t My_i=0;
		for(int i=Uart2_Rx_Cnt-Uart2_count+1;i<Uart2_Rx_Cnt+1;i++)
			my_order[My_i++]=RxBuffer[i];
			receive_flag=1;
			Uart2_count=0;
		}
	}
	}
		Uart2_Rx_Cnt++;
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRxBuffer, 1); 
}

void wifiswitch(void){
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "WIFI mode!", 16, 1);
	OLED_Refresh();
	uint8_t my_test_v = 0 ;
	while(1){
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<mode1>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				my_test_v=1;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode2>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=2;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode3>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=3;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode4>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=4;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode5>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=5;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode6>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=6;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode7>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=7;
			}
			else if(strstr((const char*)my_order,(const char*)"<mode8>"))
			{
						Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
				my_test_v=8;
			}
			else
				{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));
			
			}
				
		}
			if(my_test_v==1){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode1();
			}else if(my_test_v==2){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode2();
			}else if(my_test_v==3){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode3();
			}else if(my_test_v==4){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode4();
			}else if(my_test_v==5){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode5();
			}else if(my_test_v==6){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode6();
			}else if(my_test_v==7){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode7();
			}
			else if(my_test_v==8){
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				mode8_pre();
			}
			if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
				switchmode();
			}
		}
}

void mode8_pre(void){
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,0);
	mode8();
}

void mode8(void){
	OLED_Clear();
	OLED_ShowString(20, 20, "Loading...", 16, 1);
	OLED_Refresh();
	HAL_Delay(2000);
	OLED_Clear();
	OLED_ShowString(20, 20, "RGB mode!", 16, 1);
	OLED_Refresh();
	R1 = (100*R)/256;
	G1 = (100*G)/256;
	B1 = (100*B)/256;
	while(1){
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,B1);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,G1);
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,R1);
		if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
			setrgb();
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				switchmode();
			}
		if(receive_flag)
		{
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<set>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3,0);
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_4,0);
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
				setrgb();
			}else if(strstr((const char*)my_order,(const char*)"<exit>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				wifiswitch();
			}
		}
	}
}

void setrgb(void){
	OLED_Clear();
	while(1){
		OLED_ShowString(20, 10, "Setting the", 12, 1);
		OLED_ShowString(20, 20, "RGB light...(%)", 12, 1);
		OLED_ShowString(20, 30, "Current R:", 12, 1);
		OLED_ShowString(20, 40, "Current G:", 12, 1);
		OLED_ShowString(20, 50, "Current B:", 12, 1);
		OLED_ShowNum(85, 30, R, 3, 12, 1);
		OLED_ShowNum(85, 40, G, 3, 12, 1);
		OLED_ShowNum(85, 50, B, 3, 12, 1);
		if (stat == 1){
			OLED_ShowString(105, 30, "*", 12, 1);
		}else if(stat == 2){
			OLED_ShowString(105, 40, "*", 12, 1);
		}else if(stat == 3){
			OLED_ShowString(105, 50, "*", 12, 1);
		}
		OLED_Refresh();
		if (Button_ShortPress(GPIOB, GPIO_PIN_14)) {
			 if (stat == 1){ 
					R += 5;
					if(R >= 256){
						R -= 5;
					}
					HAL_Delay(500);
				}else if(stat == 2){
					G += 5;
					if(G >= 256){
						G -= 5;
					}
					HAL_Delay(500);
				}else if(stat == 3){
					B += 5;
					if(B >= 256){
						B -= 5;
					}
					HAL_Delay(500);
			}
		}
		if (Button_ShortPress(GPIOB, GPIO_PIN_15)) {
			  if (stat == 1){ 
					R -= 5;
					if(R >= 10000){
						R += 5;
					}
					HAL_Delay(500);
				}else if(stat == 2){
					G -= 5;
					if(G >= 10000){
						G += 5;
					}
					HAL_Delay(500);
				}else if(stat == 3){
					B -= 5;
					if(B >= 10000){
						B += 5;
					}
					HAL_Delay(500);
			}
		}
			if (Button_LongPress(GPIOA, GPIO_PIN_12)) {
				if(stat == 2 || stat == 3){
					stat -= 1;
					setrgb();
				}else{
					stat = 3;
					setrgb();
				}
			}
			if (Button_LongPress(GPIOA, GPIO_PIN_11)) {
				if(stat == 1 || stat == 2){
					stat += 1;
					setrgb();
				}else{
					stat = 1;
					setrgb();
				}
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
			mode8();
		}
		if(receive_flag)
		{
			int received_value = 0;
			receive_flag=0;
			if(strstr((const char*)my_order,(const char*)"<+>"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				if (stat == 1){ 
					R += 5;
					if(R >= 256){
						R -= 5;
					}
				}else if(stat == 2){
					G += 5;
					if(G >= 256){
						G -= 5;
					}
				}else if(stat == 3){
					B += 5;
					if(B >= 256){
						B -= 5;
					}
			}
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
			}else if(strstr((const char*)my_order,(const char*)"<->"))
			{
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				if (stat == 1){ 
					R -= 5;
					if(R >= 10000){
						R += 5;
					}
					HAL_Delay(500);
				}else if(stat == 2){
					G -= 5;
					if(G >= 10000){
						G += 5;
					}
					HAL_Delay(500);
				}else if(stat == 3){
					B -= 5;
					if(B >= 10000){
						B += 5;
					}
					HAL_Delay(500);
				}
				esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
				esp8266_send_cmd("OK!\n","OK",50);
			}else if(strstr((const char*)my_order,(const char*)"<switch>")){
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				if(stat == 1 || stat == 2){
					stat += 1;
					esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
					esp8266_send_cmd("OK!\n","OK",50);
					setrgb();
				}else{
					stat = 1;
					esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
					esp8266_send_cmd("OK!\n","OK",50);
					setrgb();
				}
			}else if(strstr((const char*)my_order,(const char*)"<exit>")){
				Uart2_Rx_Cnt=0;
				memset(RxBuffer,0x00,sizeof(RxBuffer));
				memset(RxBuffer,0x00,sizeof(my_order));	
				esp8266_send_cmd("AT+CIPSEND=0,11","OK",50);
				esp8266_send_cmd("Exiting...\n","OK",50);
				mode8();
			}else if(sscanf((const char*)my_order, "<%d>", &received_value) == 1)
					{
						if(received_value >= 0 && received_value <= 255) // 示例范围限制
						{
								Uart2_Rx_Cnt = 0;
								memset(RxBuffer, 0x00, sizeof(RxBuffer));
								memset(my_order, 0x00, sizeof(my_order));
								if (stat == 1){ 
									R = received_value;
									if(R >= 10000){
										R = 0;
									}
									HAL_Delay(500);
								}else if(stat == 2){
									G = received_value;
									if(G >= 10000){
										G = 0;
									}
									HAL_Delay(500);
								}else if(stat == 3){
									B = received_value;
									if(B >= 10000){
										B = 0;
									}
									HAL_Delay(500);
								}
								esp8266_send_cmd("AT+CIPSEND=0,4","OK",50);
								esp8266_send_cmd("OK!\n","OK",50);
						}
			}
		}
	}
}

void setrtc(void){
	OLED_Clear();
	uint8_t position = 0;
	while(1){
		setting = *MY_RTC_GetTime();
		char str[30];
		sprintf(str, "%d-%02d-%02d", setting.tm_year + 1900,setting.tm_mon + 1,setting.tm_mday);
		OLED_ShowString(20, 0, (uint8_t *)str, 16, 1);
		sprintf(str, "%02d:%02d:%02d", setting.tm_hour,setting.tm_min,setting.tm_sec);
		OLED_ShowString(16, 20, (uint8_t *)str, 24, 1);
		if(position == 0){
			OLED_DrawLine(24-4,17,56-4,17,1);
			if (Button_LongPress(GPIOB, GPIO_PIN_14)) {
				setting.tm_year++;
				MY_RTC_SetTime(&setting);
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_15)) {
			setting.tm_year--;
			MY_RTC_SetTime(&setting);
			}
		}
		if(position == 1){
			OLED_DrawLine(24+5*8-4,17,24+7*8-4,17,1);
			if (Button_LongPress(GPIOB, GPIO_PIN_14)) {
				setting.tm_mon++;
				MY_RTC_SetTime(&setting);
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_15)) {
			setting.tm_mon--;
			MY_RTC_SetTime(&setting);
			}
		}
		if(position == 2){
			OLED_DrawLine(24+8*8-4,17,24+10*8-4,17,1);
			if (Button_LongPress(GPIOB, GPIO_PIN_14)) {
				setting.tm_mday++;
				MY_RTC_SetTime(&setting);
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_15)) {
			setting.tm_mday--;
			MY_RTC_SetTime(&setting);
			}
		}
			if(position == 3){
			OLED_DrawLine(16,45,16+2*12,45,1);
			if (Button_LongPress(GPIOB, GPIO_PIN_14)) {
			setting.tm_hour++;
				MY_RTC_SetTime(&setting);
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_15)) {
			setting.tm_hour--;
			MY_RTC_SetTime(&setting);
			}
		}
			if(position == 4){
			OLED_DrawLine(16+3*12,45,16+5*12,45,1);
			if (Button_LongPress(GPIOB, GPIO_PIN_14)) {
			setting.tm_min++;
				MY_RTC_SetTime(&setting);
			}
		if (Button_LongPress(GPIOB, GPIO_PIN_15)) {
			setting.tm_min--;
			MY_RTC_SetTime(&setting);
			}
		}
		OLED_Refresh();
		if (Button_LongPress(GPIOA, GPIO_PIN_11)) {
			position++;
			if(position == 5){
				position = 0;
			}
			OLED_Clear();
		}
			if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
				MY_RTC_SetTime(&setting);
				init();
		}
	}
}

void displayrtc(void){
	OLED_Clear();
	struct tm* now = MY_RTC_GetTime();
	char str[30];
	sprintf(str, "%d-%02d-%02d", now ->tm_year + 1900,now ->tm_mon + 1,now ->tm_mday);
	OLED_ShowString(20, 0, (uint8_t *)str, 16, 1);
	sprintf(str, "%02d:%02d:%02d", now ->tm_hour,now ->tm_min,now ->tm_sec);
	OLED_ShowString(16, 20, (uint8_t *)str, 24, 1);
	char* week = weeks[now -> tm_wday];
	uint8_t xweek = (128 - (strlen(week) * 8)) /2;
	OLED_ShowString(xweek, 50, (uint8_t *)week, 8, 1);
	OLED_Refresh();
	if (Button_LongPress(GPIOB, GPIO_PIN_13)) {
		setrtc();
	}
	HAL_Delay(500);
}

void UpdateTime(TimeStruct *time) {
  uint32_t total_ms = ms_counter;
  
  time->hours = total_ms / 360000;
  total_ms %= 360000;
  
  time->minutes = total_ms / 6000;
  total_ms %= 6000;
  
  time->seconds = total_ms / 100;
  time->milliseconds = total_ms % 100;
}

void DisplayTime(TimeStruct *t) {
  char buffer[16];
  
  sprintf(buffer, "%02d:%02d:%02d", 
         t->minutes, 
         t->seconds, 
         t->milliseconds);
	OLED_ShowString(16, 20, (uint8_t *)buffer, 24, 1);
	OLED_Refresh();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(is_running){
		if (htim->Instance == TIM2) {
			ms_counter += 1;
		}
	}
}

void timer(void){
	TimeStruct current_time = {0};
	OLED_Clear();
	UpdateTime(&current_time);
	DisplayTime(&current_time);
	while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == GPIO_PIN_RESET){
		HAL_Delay(10);
	}
	while(1){
		if (Button_ShortPress(GPIOA, GPIO_PIN_15)) {
			is_running = !is_running;
			while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == GPIO_PIN_RESET){
			HAL_Delay(10);
			}
		}
		if(is_running){
			UpdateTime(&current_time);
			DisplayTime(&current_time);
		}
		if (Button_ShortPress(GPIOB, GPIO_PIN_13)) {
			ms_counter = 0;
			UpdateTime(&current_time);
			DisplayTime(&current_time);
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
			switchmode();
		}
	}
}
