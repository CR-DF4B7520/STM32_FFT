#include "system.h"

float temperature = 0.f;
float humidity = 0.f;

void data_proc(void)
{
    AHT20_Read(&temperature, &humidity);
//    sprintf((char*)tx_buffer, "T: %.1f C, H: %.1f %%\r\n" , temperature, humidity);
//    uart_tx_dma(&huart1, (char *) tx_buffer);
}