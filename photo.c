/*
 * photo.c
 *
 * Pins:
 * P8.4 - A7 - primary
 * P8.5 - A6 - primary
 * P8.6 - A5 - primary
 * P8.7 - A4 - primary
 * P9.0 - A8 - tertiary
 * P9.1 - A9 - tertiary
 * P9.5 - A13 - secondary
 *
 * Created: 11/7/19
 * Last Edited: 12/1/19
 * Author: Andrew Hollabaugh
 */

#include "temp.h"
#include "uart.h"

#define THRESHOLD = 2000

int photo_array_values[7] = {0,0,0,0,0,0,0};

//sets up ADC
void temp_init()
{
    //P8SEL0 |= 0x01; //enable ADC on P8.4, A7

    ADC12CTL0 = ADC12ON+ADC12SHT0_8+ADC12MSC; //turn on ADC12, set sampling time, multi-sample conversion
    ADC12CTL1 = ADC12SHP+ADC12CONSEQ_2; //use sampling timer, set mode

    ADC12MCTL0 |= ADC12INCH_7; //set memory 0 reg to be from A7
    ADC12IER0 |= ADC12INCH_7; //enable ADC12IFG0 to trigger on converson of A7

    /*ADC12MCTL1 |= ADC12INCH_6; //set memory 1 reg to be from A6
    ADC12IER1 |= ADC12INCH_6; //enable ADC12IFG1 to trigger on converson of A6
    ADC12MCTL2 |= ADC12INCH_5; //set memory 2 reg to be from A5
    ADC12IER2 |= ADC12INCH_5; //enable ADC12IFG2 to trigger on converson of A5
    ADC12MCTL3 |= ADC12INCH_4; //set memory 3 reg to be from A4
    ADC12IER3 |= ADC12INCH_4; //enable ADC12IFG3 to trigger on converson of A4
    ADC12MCTL4 |= ADC12INCH_8; //set memory 4 reg to be from A8
    ADC12IER4 |= ADC12INCH_8; //enable ADC12IFG4 to trigger on converson of A8
    ADC12MCTL5 |= ADC12INCH_9; //set memory 5 reg to be from A9
    ADC12IER5 |= ADC12INCH_9; //enable ADC12IFG5 to trigger on converson of A9
    ADC12MCTL6 |= ADC12INCH_13; //set memory 6 reg to be from A13
    ADC12IER6 |= ADC12INCH_13; //enable ADC12IFG6 to trigger on converson of A13
    */

    ADC12CTL0 |= ADC12ENC; //enable conversions
    ADC12CTL0 |= ADC12SC; //start conversion
}

//get raw value from ADC, for debug
int photo_array_get(int channel)
{
    if(photo_array_values[channel] < threshold)
        return 0;
    else
        return 1;
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
        case ADC12IV_ADC12IFG0:
            photo_array_state[0] = ADC12MEM0;
            break;
        case ADC12IV_ADC12IFG1: 
            photo_array_state[1] = ADC12MEM1;
            break;
        case ADC12IV_ADC12IFG2:
            photo_array_state[2] = ADC12MEM2;
            break;
        case ADC12IV_ADC12IFG3:
            photo_array_state[3] = ADC12MEM3;
            break;
        case ADC12IV_ADC12IFG4:
            photo_array_state[4] = ADC12MEM4;
            break;
        case ADC12IV_ADC12IFG5:
            photo_array_state[5] = ADC12MEM5;
            break;
        case ADC12IV_ADC12IFG6:
            photo_array_state[6] = ADC12MEM6;
            break;
        default: break;
    }
}

