#include "stm32f1xx.h"
#include "tim.h"
#include "dht11.h"

uint8_t Data[5] = { 0x00,0x00,0x00,0x00,0x00 };   //Data�洢��ȡ����ʪ����Ϣ



void Delay_us(uint16_t udelay)
{
  __IO uint32_t Delay = udelay * 72 / 8;//(SystemCoreClock / 8U / 1000000U)
    //��stm32f1xx_hal_rcc.c -- static void RCC_Delay(uint32_t mdelay)
  do
  {
    __NOP();
  }
  while (Delay --);
}

//void Delay_us(uint16_t us) {     //΢����ʱ
//volatile	uint16_t differ = 0xffff - us - 5;
//	__HAL_TIM_SET_COUNTER(&htim1, differ);	//�趨TIM1��������ʼֵ
//	HAL_TIM_Base_Start(&htim1);		//������ʱ��	

//	while (differ < 0xffff - 5) {	//�ж�
//		differ = __HAL_TIM_GET_COUNTER(&htim1);		//��ѯ�������ļ���ֵ
//	}
//	HAL_TIM_Base_Stop(&htim1);
//}

/*------------------------------*/
void DHT_GPIO_SET_OUTPUT(void)     //����GPIOxΪ���ģʽ��MCU��IO����DHT11�������źţ�
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};    //��GPIO_InitTypeDef�ṹ�����޸�IO�ڲ������ṹ���Ա��
	GPIO_InitStructure.Pin = GPIO_PIN_8;      //���õĸ�ʽ�����ϸ���ѭע�ͣ�����GPIO_PIN_define
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	//	GPIO_InitStructure.Pull=;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void DHT_GPIO_SET_INPUT(void)     //����GPIOxΪ����ģʽ��DHT11��MUC��IO����ƽ�źţ��ź����������ʪ����Ϣ��
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*------------------------------*/

/*
	 uint8_t DHT_Read_Byte(void)����ת��ɼ�DHT11���͸�IO�ڵĵ�ƽ�źţ�8λ����
*/
uint8_t DHT_Read_Byte(void)  //��DHT11��ȡһλ��8�ֽڣ��ź�
{
	uint8_t ReadData = 0;  //ReadData���ڴ��8bit���ݣ���8�����ζ�ȡ��1bit���ݵ����
	uint8_t temp;      //��ʱ����źŵ�ƽ��0��1��
	uint8_t retry = 0;   //retry���ڷ�ֹ����
	uint8_t i;
	for (i = 0; i < 8; i++)   //һ����ʪ���źŶ�ȡ��λ
	{
		while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0 && retry < 100)
			//�ȴ�ֱ��DHT11����ߵ�ƽ����PA5=1�������أ���ʾ��ʼ�������ݣ������ж�0 or 1������ѭ����ִ�к����жϣ���PA5=0����һֱѭ���ȴ���
		{
			Delay_us(1);
			retry++;             //retry��ֹPA5��ȡ�������ݿ�������һ����������100us��retry������100������ѭ����
		}
		retry = 0;

		Delay_us(40);    //��ʱ30us
		//����ʱ��ͼ��DHT���ظߵ�ƽ�ź�ά��26us~28us��ʾ0��	ά��70us��ʾ1
	  //��ʱ30us�����IO��ȡ�����Ǹߵ�ƽ��˵���ɼ���1�����IO��ȡ���͵�ƽ��˵���ɼ���0
		//��ȡ��ƽ�ź��ݴ�temp�ڣ�����ѹ��ReadData��
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 1)   temp = 1;
		else   temp = 0;

		while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 1 && retry < 100)
			//�ȴ�ֱ��DHT11����͵�ƽ����ʾ�˳�������1bit�źŽ�����ϡ�
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;

		ReadData <<= 1;    //ReadData���ź���ȫ������һλ���ճ�ĩβλ��
		ReadData |= temp;        //��tempд��ReadData
	}

	return ReadData;
}

/*------------------------------*/

/*
	 uint8_t DHT_Read(void)�������ʱ��
*/
uint8_t DHT_Read(void)
{
	uint8_t retry = 0;
	uint8_t i;

	DHT_GPIO_SET_OUTPUT();    //IO����Ϊ���ģʽ���ڴ�����ʼ��MCUҪ��DHT11�����ź�
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);   //IO->DHT11:�����͵�ƽ18ms��Ӧʱ��Ҫ��
	Delay_us(18000);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   //IO->DHT11:������ߵ�ƽ20us
	Delay_us(20);

	//MCUͨ��IO��DHT11����������ϡ�������DHT11��IO������Ӧ,IOתΪ����ģʽ������֮��Ϳ�ʼ�źŵ�ת���ȡ��
	DHT_GPIO_SET_INPUT();
	Delay_us(20);
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0) //DHT11���ص͵�ƽ��Ӧ����ȡ���͵�ƽ��˵��DHT11����Ӧ��
	{
		//��������DHT11���͵�ƽһ��ʱ������ߵ�ƽһ��ʱ��
		while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0 && retry < 100)
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;
		while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 1 && retry < 100)
		{
			Delay_us(1);
			retry++;
		}
		retry = 0;

		//һ������40λ��һ��DHT_Read_Byte����8λ������ȡ5�Ρ��洢��Data[]�С���Data[]����Ϊȫ�֣�
		for (i = 0; i < 5; i++)
		{
			Data[i] = DHT_Read_Byte();  //ÿ�ζ�ȡһ�ֽڣ�8λ��
		}
		Delay_us(50);
		//˵����Data[0]ʪ�ȣ� Data[2]�¶ȡ�Data[1]��Data[3]�ֱ�Ϊ0��2��С��λ��Data[4]����У�顣
	}

	uint32_t sum = Data[0] + Data[1] + Data[2] + Data[3];  //У��
	if ((sum) == Data[4])    return 1;
	else   return 0;

}
