#include "main.h"

extern float motor_duty;
extern uint8_t motor_turn_on;
extern uint8_t motor_turn_off;
extern uint8_t motor_vibrate;
extern uint8_t direction;
extern float rps;

void motor_on(void);
void motor_off(void);
void motor_set_speed(float Duty);
void motor_read_rps(void);
int duty_to_angle(float duty);
int rps_to_angle(float rps);