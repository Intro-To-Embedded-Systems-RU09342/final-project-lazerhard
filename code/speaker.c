/*
 * speaker.c
 *
 * description
 *
 * Created: 12/3/19
 * Last Edited: 12/3/19
 * Author: Andrew Hollabaugh
 */

#include "speaker.h"
#include "uart.h"
#include "photo.h"

void speaker_init()
{
    //timer A0, for fan output, P1.6
    P1DIR |= BIT6; //set 1.6 as output
    P1SEL1 |= BIT6; //set 1.6 for tertiary function, which is pwm
    P1SEL0 |= BIT6;
    TA0CTL |= TASSEL_2 + MC_1; //smclk, up mode
    TA0CCTL1 |= OUTMOD_7; //pwm output
    //TA0CCR0 = MAX_OUTPUT - 1; //define pwm frequency
    TA0CCR1 = 0; //start with zero output

    //timer A1, for control loop timing
    TA1CTL |= TASSEL_2 + MC_2 + ID_3; //smclk, continuous mode
    TA1CCTL0 = CCIE; //enable CCR0 interrupt
    //TA1CCR0 = CONTROL_LOOP_PERIOD;
}

void speaker_tone_begin(uint16_t freq)
{
}

void speaker_tone_end(uint16_t freq)
{
}

