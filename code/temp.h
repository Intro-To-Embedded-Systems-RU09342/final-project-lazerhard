#ifndef TEMP_H
#define TEMP_H

#include <msp430.h>
#include <stdint.h>

void temp_init(void);
double temp_get(void);
uint16_t temp_adc_get(void);

#endif

