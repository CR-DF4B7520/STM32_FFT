#include "system.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"


__IO uint32_t uwTick_Key_LS_SetPoint = 0;
uint8_t ucKey_Val,ucKey_Down,ucKey_Up,ucKey_Old;

int step = 10;

void led_proc(void)
{ 
    if (motor_vibrate)
    {
        HAL_GPIO_TogglePin(Led_Blue_GPIO_Port, Led_Blue_Pin);
    }
}

uint8_t Key_Scan(void)
{
    uint8_t ucKey_val = 0;
    
    if (HAL_GPIO_ReadPin(Key1_GPIO_Port, Key1_Pin) == GPIO_PIN_SET){
        ucKey_val = 1;
    }
    if (HAL_GPIO_ReadPin(Key2_GPIO_Port, Key2_Pin) == GPIO_PIN_RESET){
        ucKey_val = 2;
    }
    if (HAL_GPIO_ReadPin(Key3_GPIO_Port, Key3_Pin) == GPIO_PIN_RESET){
        ucKey_val = 3;
    }
    return ucKey_val;
}


void key_proc(void)
{
    static uint8_t long_press_handled = 0;
    ucKey_Val = Key_Scan();
    ucKey_Down = ucKey_Val & (ucKey_Val ^ ucKey_Old);
    ucKey_Up = ~ ucKey_Val & (ucKey_Val ^ ucKey_Old);
    ucKey_Old = ucKey_Val;
    
    if (ucKey_Down){
        uwTick_Key_LS_SetPoint = uwTick;
        long_press_handled = 0;
    }
    
    if (uwTick - uwTick_Key_LS_SetPoint < 1000)  // Short FSM
    {
        if (ucKey_Up == 1)              // Slow down to stop.
        {
            motor_vibrate = 0;            
            motor_off();
        }
        
        if(ucKey_Up == 2)               // Disable vibrating.
        {
            motor_vibrate = 0;
            Led_Red(0);
        }
        
        if(ucKey_Up == 3)              
        {
              HAL_UART_MspDeInit(&huart1);
              HAL_Init();
              SystemClock_Config();
              MX_DMA_Init();
              MX_USART1_UART_Init();
              HAL_UART_MspInit(&huart1);
              HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer1, MAX_MESSAGE_LENGTH);
              ring_buffer_put(&uart_rb1, TRANSMIT, "DeInit!\r\n");
        } 
    }
    else    // Long FSM
    {
        if (ucKey_Val == 1 && !long_press_handled)      // Long Press 01 to start.
        {
            long_press_handled = 1;
            if (!motor_turn_on)     // Long Press 01 to start.
            {
                motor_on();
            }
            else                    // Long Press 01 during motor_on();
            {
                motor_off();
            }
        }
        if (ucKey_Val == 2 && !long_press_handled)      // Long Press 02 to vibrate.
        {
            long_press_handled = 1;
            motor_vibrate = 1;
            pwm_set_param(&htim5, TIM_CHANNEL_4, 1000, 85);
        }
    }
}



