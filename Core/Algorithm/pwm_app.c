#include "tim.h"

void pwm_set_param(TIM_HandleTypeDef *htim,uint32_t Channel,uint32_t Freq,float Duty)
{
    if (Duty>= 90)
    {
        Duty = 90;
    }
    else if (Duty <= 10)
    {
        Duty = 10;
    }
    //基本参数参考 APB1频率 84Mhz； 预分频系数 84-1=83
    float Freq_Arr = 1000000.f/Freq - 1;
    float Duty_Pulse = Duty*1.0f/100*(Freq_Arr+1);
    __HAL_TIM_SetAutoreload(htim,(uint16_t)Freq_Arr);
    __HAL_TIM_SetCompare(htim,Channel,(uint16_t)Duty_Pulse);
    HAL_TIM_PWM_Start(htim,Channel);
}