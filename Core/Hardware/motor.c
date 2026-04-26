#include "system.h"
#include "tim.h"

float motor_duty = 40.f;
uint8_t motor_turn_on = 0;  // 开启动作标志位
uint8_t motor_turn_off = 0; // 停车动作标志位
uint8_t motor_vibrate = 0;  // 高頻震动标志位
uint8_t direction = 0;
float rps = 0.f;

void motor_on(void)
{
    pid_reset(&pid_rps);
    pid_set_target(&pid_rps, 6.0f);
    pid_set_limit(&pid_rps, 70.0f);
    HAL_GPIO_WritePin(GPIOA, BIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, BIN2_Pin, GPIO_PIN_SET);
    motor_turn_on = 1;
    Led_Blue(1);
    ring_buffer_put(&uart_rb5, TRANSMIT, "motor_on=1\xff\xff\xff");
    ring_buffer_put(&uart_rb1, TRANSMIT, "motor_on=1\r\n");
}

void motor_off(void)
{
    pid_reset(&pid_rps);
    pid_set_target(&pid_rps, 0.0f);
    pid_set_limit(&pid_rps, 70.0f);
}


void motor_set_speed(float Duty)
{
    if (Duty <= 10)
        Duty = 10;
    if (Duty >= 90)
        Duty = 90;
    pwm_set_param(&htim5, TIM_CHANNEL_4, 1000, Duty);
}

void motor_read_rps(void)
{
    static int32_t total_count = 0;
    static uint32_t last_count = 0;
    
    direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2); //读取编码器旋转方向
    uint32_t current_count = __HAL_TIM_GET_COUNTER(&htim2);
    
    int diff = (int32_t)current_count - (int32_t)last_count;
    
    if(diff > 32767) 
    {
        diff -= 65536;  // 正向溢出修正
    } 
    else if(diff < -32768) 
    {
        diff += 65536;  // 反向溢出修正
    }
    
    if (diff < 0)
    {
        diff *= -1;
    }
    rps = (float)diff / (4.f * 20.f * 13.f) * 10.f;
    last_count = current_count;
    
//    ring_buffer_put(&uart_rb1, "Dir %d Spd %.2f\r\n", direction, rps);
}

int duty_to_angle(float duty) 
{
    float angle = -30 + duty * 3;
    angle = fmod(angle, 360);
    if (angle < 0) angle += 360;
    return (int)angle;
}

int rps_to_angle(float rps)
{
    float angle = (rps - 2.f) * 20;
    angle = fmodf(angle, 360);
    if (angle < 0) angle += 360;
    return (int)angle;
}