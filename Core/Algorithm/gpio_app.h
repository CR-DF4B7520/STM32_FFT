#include "main.h"

#define Led_Red(x) HAL_GPIO_WritePin(Led_Red_GPIO_Port, Led_Red_Pin, (x) ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define Led_Blue(x) HAL_GPIO_WritePin(Led_Blue_GPIO_Port, Led_Blue_Pin, (x) ? GPIO_PIN_RESET : GPIO_PIN_SET)

extern uint8_t state;

void led_proc(void);
void key_proc(void);
