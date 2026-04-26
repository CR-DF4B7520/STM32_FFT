#include "system.h"
#include "tim.h"

PID_T pid_rps;
float pid_duty = 0.f;

void pid_proc(void)
{
    motor_read_rps();
    if (motor_vibrate)      // Disable pid while vibrating
    {
        return;
    }
    else if (motor_turn_on)     // Enable motor pid
    {
        pid_duty = pid_calculate_incremental(&pid_rps, rps);
        pwm_set_param(&htim5, TIM_CHANNEL_4, 1000, pid_duty);
        if (pid_rps.target ==0 && pid_duty <= 10)   // Slow down to stop
        {
            HAL_GPIO_WritePin(GPIOA, BIN1_Pin|BIN2_Pin, GPIO_PIN_RESET);
            HAL_TIM_PWM_Stop(&htim5, TIM_CHANNEL_4);
            motor_turn_on = 0;
            ring_buffer_put(&uart_rb5, TRANSMIT, "duty_val=330\xff\xff\xffrps_val=320\xff\xff\xffmotor_on=0\xff\xff\xff");
            ring_buffer_put(&uart_rb1, TRANSMIT, "motor_on=0\r\n");
            Led_Blue(0);
            Led_Red(0);
        }
    }
    
}
