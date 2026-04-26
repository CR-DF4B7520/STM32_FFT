#include "system.h"
#include "tim.h"
#include "dma.h"
#include "gpio.h"

// 全局变量定义
uint8_t uart_error = 0;
uint16_t PWM1_Time_Count = 0;
uint16_t PWM1_Duty_Count = 0;
uint16_t PWM1_Freq = 0;
float PWM1_Duty = 0;

// 基本回传指令
uint8_t motor_str[] = {0x65, 0x02, 0x04, 0x01, 0xFF, 0xFF, 0xFF};
uint8_t fft_str[] = {0x65, 0x03, 0x1B, 0x00, 0xFF, 0xFF, 0xFF};

// 基本定时器中断响应接口
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM6)
    {
        // Trigger Interrupt
        
    }
}

// TIM2定时器输入捕获处理
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            PWM1_Time_Count = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1)+1;
            PWM1_Duty = (float)PWM1_Duty_Count/PWM1_Time_Count;
        }
        else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
            PWM1_Duty_Count = HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2)+1;
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == &huart5)
    {
        if (strncmp((char *) rx_buffer2, (char*)motor_str, sizeof(motor_str))==0)
        {
            if (!motor_turn_on)     // Motor start.
            {
                motor_on();
            }
            else
            {
                motor_off();
            }
        }
        if (strncmp((char *) rx_buffer2, (char*)fft_str, sizeof(fft_str))==0)
        {
            fft_bias ^= 1;
        }
        memset(rx_buffer2, 0, sizeof(rx_buffer2));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart5, rx_buffer2, MAX_MESSAGE_LENGTH);
    }
    if (huart == &huart1)
    {
        if (strncmp((char *) rx_buffer1, "ACK\r\n", strlen("ACK\r\n"))==0)
        {
            uart_error = 0;
            Led_Red(0);
        }
        if (strncmp((char *) rx_buffer1, "start", strlen("start"))==0 && !motor_turn_on)
        {
            motor_on();
        }
        else if (strncmp((char *) rx_buffer1, "stop", strlen("stop"))==0)
        {
            motor_off();
        }
        memset(rx_buffer1, 0, sizeof(rx_buffer1));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer1, MAX_MESSAGE_LENGTH);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        memset(rx_buffer1, 0, sizeof(rx_buffer1));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer1, MAX_MESSAGE_LENGTH);
    } 
    else if (huart == &huart5)
    {
        memset(rx_buffer2, 0, sizeof(rx_buffer2));
        HAL_UARTEx_ReceiveToIdle_DMA(&huart5, rx_buffer2, MAX_MESSAGE_LENGTH);
    }
}