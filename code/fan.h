#ifndef FAN_H
#define FAN_H

#include <msp430.h>
#include <stdint.h>

void fan_init(void);
void fan_set(uint16_t speed);
void fan_set_manual(uint16_t speed);
void fan_set_auto(uint16_t temp);
uint16_t fan_get_speed(void);
void fan_calc_speed(void);


#endif

