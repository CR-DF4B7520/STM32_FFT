
/*
 *  文件名：24cxx.h
 *  功能描述： 24C02 EEPROM驱动程序，使用HAL库
 *
 *  (1)24C02是 256字节EEPROM，可以单个字节读写，连续读写时按页读写，每页8字节。所以，按页读写时最多8字节,页号0--31
 *  (2)连续写入数据时要注意不要超过页的边界，否则从页的开始处重新写，会覆盖原来的内容。
 *  (3)24C02地址是 0b1010xxxy, 其中xxx由芯片的地址引脚A2,A1,A0决定，一般接地，所以是0xA0, HAL库在读写时自动在最后一位写0或1进行读或写操作
 *  (4)24C02的地址数据长度是8位，使用宏定义符号I2C_MEMADD_SIZE_8BIT
 *
 */


#ifndef __24cxx_H
#define __24cxx_H

#include    "stm32f4xx_hal.h"
#include "stdbool.h"

#include    "i2c.h"                 //i2c.h中定义了hi2c2

/* 两个与硬件相关的定义，   */
#define     I2C_HANDLE	hi2c1       //I2C外设对象变量，利用i2c.h中定义的hi2c1

#define     DEV_ADDR_24CXX      0x00A0      //24C02的写地址
// EEPROM存储器参数

#define     PAGE_SIZE_24CXX     0x0008      //24C02的Page大小为8字节

#define     MEM_SIZE_24CXX      (uint16_t)256   //24C02总共容量字节数，256字节


//检查设备是否准备好
HAL_StatusTypeDef   EP24C_IsDeviceReady(void);

//在任意地址写入一个字节
HAL_StatusTypeDef   EP24C_WriteOneByte(uint16_t memAddress, uint8_t byteData, bool DMA);

//在任意地址读出一个字节
HAL_StatusTypeDef   EP24C_ReadOneByte(uint16_t memAddress, uint8_t *byteData);

//连续读取数据，任意地址，任意长度，不受页的限制
HAL_StatusTypeDef   EP24C_ReadBytes(uint16_t memAddress, uint8_t *pBuffer, uint16_t bufferLen);

/*限定在一个页内写入连续数据，最多8字节。任意起始地址，
 * 但是起始地址+数据长度不能超过页边界，即不能超过 地址  8*N-1 */
HAL_StatusTypeDef   EP24C_WriteInOnePage(uint16_t memAddress, uint8_t *pBuffer, uint16_t bufferLen, bool DMA);

//写任意长的数据，可以超过8字节，但数据地址必须从页首开始，即 8*N
HAL_StatusTypeDef   EP24C_WriteLongData(uint16_t memAddress, uint8_t *pBuffer, uint16_t bufferLen);

#endif
