#include "system.h"

uint8_t iic_write = 0; 
uint16_t iic_temperature = 0;
uint8_t iic_str[32], iic_buf[32];

void iic_write_task(uint16_t temp)
{
    memset(iic_str, 0, sizeof(iic_str));
    sprintf((char*)iic_str, "%d", temp);

    EP24C_WriteLongData(0x01, iic_str, sizeof(iic_str));
    iic_write = 1;
    return; 
}

void iic_read_task(void)
{
//    memset(iic_buf, 0, sizeof(iic_buf));
//    if (EP24C_ReadBytes(0x01, iic_buf, sizeof(iic_str)) == HAL_OK)
//    {
//        state = 0;
//        // 将字符串转换为 uint16_t 类型
//        iic_temperature = (uint16_t)atoi((char*)iic_buf);
//        ring_buffer_put(&uart_rb1, "IIC Read! Temperature = %.2f\r\n", (float)iic_temperature/10.f);
//    }
//    else
//    {
//        state = 2;
//        ring_buffer_put(&uart_rb1, "IIC Communication Failed!\r\n");
//    }
//    iic_write = 0;
}
