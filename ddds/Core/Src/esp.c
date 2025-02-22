#include "esp.h"
#include "string.h"
#include "usart.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "bootanimation.h"
#include "oled.h"

void u2_printf(char* fmt);
char esp8266_send_cmd(char *cmd,char *ack,u16 waittime);
uint8_t* BMPS[] = {BMP9,BMP11,BMP13,BMP15,BMP17,BMP19,BMP21,BMP23,BMP25,BMP27,BMP29,BMP31,BMP32};
 

void esp8266_test(void)//测试
{
if(esp8266_send_cmd("AT","OK",50));
 
}
 
char esp8266_send_cmd(char *cmd,char *ack,u16 waittime)//发送指令和检查接收
{
 
u2_printf(cmd);
	
if(ack&&waittime)		
	{
		while(--waittime)	
		{
			HAL_Delay(10);
		if(strstr((const char*)RxBuffer,(const char*)ack))
		{
			Uart2_Rx_Cnt=0;
			memset(RxBuffer,0x00,sizeof(RxBuffer)); //清空数组	
    return 1;
 
    }
		}
 
	}
 
	return 0;
}
 
 
void esp8266_start_trans(void)//ESP8266初始化
{
	
	esp8266_send_cmd("AT+CWMODE=2","OK",50);//设置为AP模式
 
	esp8266_send_cmd("AT+RST","ready",20);//重启
	uint32_t current = HAL_GetTick();
	uint32_t now = HAL_GetTick();
	uint8_t cnt = 1;
	while(1){
		OLED_ShowPicture(0,0,128,64,BMPS[cnt - 1],1);
		OLED_Refresh();
		HAL_Delay(100);
		if(cnt == 6){
			cnt = 0;
		}
		cnt++;
		now = HAL_GetTick();
		if(now - current >= 4000 && cnt == 1){
			break;
		}
	}
	OLED_ShowPicture(0,0,128,64,BMPS[0],1);
	OLED_Refresh();
	HAL_Delay(100);
	esp8266_send_cmd("AT+CWSAP=\"OOPS\",\"12345678\",1,4","OK",200);//设置WiFi名称、密码，模式
	OLED_ShowPicture(0,0,128,64,BMPS[6],1);
	OLED_Refresh();
	HAL_Delay(100);
	OLED_ShowPicture(0,0,128,64,BMPS[7],1);
	OLED_Refresh();
	HAL_Delay(100);
	esp8266_send_cmd("AT+CIPMUX=1","OK",20);//进入透传模式
	OLED_ShowPicture(0,0,128,64,BMPS[8],1);
	OLED_Refresh();
	HAL_Delay(100);
	OLED_ShowPicture(0,0,128,64,BMPS[9],1);
	OLED_Refresh();
	HAL_Delay(100);
	esp8266_send_cmd("AT+CIPSERVER=1,8080","OK",200);//设置端口8080
	OLED_ShowPicture(0,0,128,64,BMPS[10],1);
	OLED_Refresh();
	HAL_Delay(100);
	OLED_ShowPicture(0,0,128,64,BMPS[11],1);
	OLED_Refresh();
	HAL_Delay(100);
	esp8266_send_cmd("AT+CIPSEND","OK",50);//开始发送数据
	OLED_ShowPicture(0,0,128,64,BMPS[12],1);
	OLED_Refresh();
	HAL_Delay(1000);
}
 
uint8_t esp8266_quit_trans(void)//退出透传模式
{
	uint8_t result=1;
	u2_printf("+++");
	HAL_Delay(1000);					
	result=esp8266_send_cmd("AT","OK",20);
	return result;
 
}
 
 
void u2_printf(char* fmt)  
{  
 
	uint8_t num=0;
  char my_ch[50]={0};
	while(*fmt!=0)
	my_ch[num++]=*fmt++;
	
	my_ch[num++]='\r';
	my_ch[num++]='\n';
	
	HAL_UART_Transmit(&huart1, (uint8_t *)my_ch,num, 0xffff);
   while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
 
}
