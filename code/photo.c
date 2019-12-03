/*
 * photo.c
 *
 * description
 *
 * Created: 12/3/19
 * Last Edited: 12/3/19
 * Author: Andrew Hollabaugh
 */

#include "photo.h"
#include "uart.h"

#define MAX_ADC_VAL 4095.0
#define MAX_VOLTAGE 3.3

volatile uint16_t adc_val = 0;

//sets up ADC
void photo_init()
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
uint16_t photo_adc_get()
{
    return adc_val;
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

