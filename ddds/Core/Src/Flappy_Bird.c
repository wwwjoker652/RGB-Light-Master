#include "Flappy_Bird.h"
#include "oled.h"
#include "stdlib.h"
#include <stdio.h>
#include "main.h"
#include "rtc.h"
#include "maintask.h"
#include "my_rtc.h"

#define GRAVITY 1  // 重力
#define FLAP -3    // 鸟跳跃的上升速度
#define BIRD_X 20  // 鸟的横坐标固定
#define MAX_OBSTACLES 3
typedef struct {
    int x;       // 障碍物的横坐标
    int height;  // 障碍物的高度
		int passed;
} Obstacle;

int bird_y = 30;     // 鸟的初始纵坐标
int bird_speed = 0;  // 鸟的速度

int obstacle_x = 128; // 障碍物的初始位置
int obstacle_gap = 20; // 障碍物上下间距
int obstacle_height = 30; // 障碍物的高度
int score = 0;
uint32_t highsc = 0;
int time111 = 50;
Obstacle obstacles[MAX_OBSTACLES];
int obstacle_speed = 2; // 障碍物初始移动速度
static int frame_count = 0;

// 保存高分（始终写入DR4和DR5）
void SaveHighScore(uint32_t highScore) {
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, highScore & 0xFFFF);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (highScore >> 16) & 0xFFFF);
}

// 读取高分
uint32_t ReadHighScore(void) {
    uint32_t low = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
    uint32_t high = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
    return (high << 16) | low;
}

void InitObstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles[i].x = 128 + i * 43; // 每个障碍物在 x 轴上的初始间隔
        obstacles[i].height = rand() % 40 + 10; // 随机生成障碍物高度
    }
}

void fail(void){
	OLED_Clear();
	while(1){
		char score_text[20];
		char score_high[30];
    sprintf(score_text, "Score: %d", score);
    OLED_ShowString(20, 35, (uint8_t *)score_text, 8, 1);
		OLED_ShowString(20,10,"You win!!!",16,1);
		OLED_Refresh();
		if(score > highsc){
			highsc = score;
			SaveHighScore(highsc);
		}
		sprintf(score_high, "Highest score: %d", highsc);
		OLED_ShowString(10, 50, (uint8_t *)score_high, 8, 1);
		OLED_Refresh();
		if (Button_LongPress(GPIOA, GPIO_PIN_15)){
			break;
		}
		if (Button_LongPress(GPIOB, GPIO_PIN_11)) {
			switchmode();
		}
	}
}

void UpdateObstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        // 移动障碍物
        obstacles[i].x -= obstacle_speed;

        // 如果障碍物离开屏幕，重新生成在队列末尾
        if (obstacles[i].x < -5) {
            obstacles[i].x = 128;
            obstacles[i].height = rand() % 40 + 10;
						obstacles[i].passed = 0;
        }
				 if (obstacles[i].x + 5 < BIRD_X && !obstacles[i].passed) {
            score += obstacle_speed - 1;  // 增加分数
            obstacles[i].passed = 1;  // 更新通过标记
        }
    
    }

    // 随时间增加速度
    frame_count++;
    if (frame_count % 500 == 0 && obstacle_speed < 10) { // 每 500 帧加速一次
        obstacle_speed++;
    }
}

int CheckCollision(void) {
		if(bird_y < 0 || bird_y > 63){
				return 1;
		}
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (BIRD_X > obstacles[i].x && BIRD_X < obstacles[i].x + 5 &&
           (bird_y < obstacles[i].height || bird_y > obstacles[i].height + obstacle_gap)) {
            return 1; // 碰撞发生
        }
    }
    return 0;
}

void DisplayUpdate(void) {
    // 清除屏幕
    OLED_Clear();

    // 绘制鸟
    OLED_DrawCircle(BIRD_X, bird_y, 3);

    // 绘制所有障碍物
    for (int i = 0; i < MAX_OBSTACLES; i++) {
			if(obstacles[i].x > 0 && obstacles[i].x <= 128){
        OLED_DrawLine(obstacles[i].x, 0, obstacles[i].x, obstacles[i].height, 1); // 上障碍物
        OLED_DrawLine(obstacles[i].x, obstacles[i].height + obstacle_gap, obstacles[i].x, 63, 1); // 下障碍物
			}
    }

    // 显示分数
    char score_text[10];
    sprintf(score_text, "Score: %d", score);
    OLED_ShowString(0, 0, (uint8_t *)score_text, 8, 1);

    // 刷新屏幕
    OLED_Refresh();
}


void GameLoop(void) {
    // 1. 更新鸟的位置
    bird_speed += GRAVITY; // 重力影响
    bird_y += bird_speed;

    // 2. 处理按键（跳跃）
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15) == GPIO_PIN_RESET) { // 检测按键是否按下
        bird_speed = FLAP; // 设置鸟的上升速度
    }

    // 3. 更新障碍物
    UpdateObstacles(); // 调用障碍物更新逻辑
}


void Flappy_Bird_Game(void)
{
    while(1)
    {
			GameLoop();
			UpdateObstacles();
			DisplayUpdate();
			if (CheckCollision()) {
            // 游戏结束，重新初始化
						fail();
            bird_y = 30;
            bird_speed = 0;
            InitObstacles();
            obstacle_speed = 2;
            score = 0;
				    frame_count = 0;
        }
			HAL_Delay(50);
    }
}

void Flappy_Bird_Init(void)
{
    OLED_Clear();
    OLED_ShowString(20,0,"Flappy Bird",16,1);
    OLED_ShowString(10,30,"Press OK to start",12,1);
    OLED_Refresh();
		while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15) == GPIO_PIN_RESET){
			HAL_Delay(10);
		}
    while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15));
    OLED_Clear();
    OLED_Refresh();
	  highsc = ReadHighScore();
		InitObstacles();
    Flappy_Bird_Game();
}
