#include "main.h"

extern uint16_t iic_temperature;
extern uint8_t iic_write;

void iic_write_task(uint16_t temp);
void iic_read_task(void);
