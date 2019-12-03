/*
 * temp.c
 *
 * Uses ADC12 to read voltage on thermistor voltage divider; does math to get temperature
 *
 * Created: 11/7/19
 * Last Edited: 12/1/19
 * Author: Andrew Hollabaugh
 */

#include "temp.h"
#include "uart.h"

#define MAX_ADC_VAL 4095.0
#define MAX_VOLTAGE 3.3

//from ThermisterTTC103.xlsx voltage vs. temperature graph
#define VOLTS_TO_TEMP_LINEAR_FACTOR -62.3737 //9.31618
#define VOLTS_TO_TEMP_CONST 127.9

volatile uint16_t adc_val = 0;

//sets up ADC
void temp_init()
{
    P8SEL0 |= 0x01; //enable ADC on P8.4, A7
    ADC12CTL0 = ADC12ON+ADC12SHT0_8+ADC12MSC; //turn on ADC12, set sampling time, multi-sample conversion
    ADC12CTL1 = ADC12SHP+ADC12CONSEQ_2; //use sampling timer, set mode
    ADC12MCTL0 |= ADC12INCH_7; //set memory 0 reg to be from A7
    ADC12IER0 |= ADC12INCH_7; //enable ADC12IFG0 to trigger on converson of A7
    ADC12CTL0 |= ADC12ENC; //enable conversions
    ADC12CTL0 |= ADC12SC; //start conversion
}

//get raw value from ADC, for debug
uint16_t temp_adc_get()
{
    return adc_val;
}

//get temperature in mili degrees C
double temp_get()
{
    double voltage = adc_val / MAX_ADC_VAL * MAX_VOLTAGE;
    double temp = 1000 * (VOLTS_TO_TEMP_LINEAR_FACTOR * voltage + VOLTS_TO_TEMP_CONST);
    return temp;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG))
    {
        case ADC12IV_ADC12IFG0:             // Vector 12:  ADC12MEM0 Interrupt
            adc_val = ADC12MEM0;
            break;
        default: break;
    }
}

