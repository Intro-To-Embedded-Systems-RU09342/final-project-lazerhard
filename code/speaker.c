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

#define F_CPU 8000000

uint8_t current_octave = 1;

double freqs[3][7] = {
    {130.8, 146.8, 164.8, 174.6, 196, 220, 146.9},
    {261.6, 293.7, 329.6, 349.2, 392, 440, 493.9},
    {523.2, 587.3, 659.2, 698.4, 784, 880, 987.8}
};

uint16_t outputs[7] = {0, 0, 0, 0, 0, 0, 0};

void speaker_init()
{
    P1DIR |= BIT5 + BIT6 + BIT7;
    P1SEL0 |= BIT5 + BIT6 + BIT7;
    P1SEL1 |= BIT5 + BIT6 + BIT7;
    TA0CCTL0 = CCIS_0 + OUTMOD_0 + CCIE;
    TA0CCTL1 = CCIS_0 + OUTMOD_0 + CCIE;
    TA0CCTL2 = CCIS_0 + OUTMOD_0 + CCIE;
    TA0CTL = TASSEL_2 + MC_2 + TAIE;

    P2DIR |= BIT4 + BIT5 + BIT6 + BIT7;
    P2SEL0 |= BIT4 + BIT5 + BIT6 + BIT7;
    TB0CCTL3 = CCIS_0 + OUTMOD_0 + CCIE;
    TB0CCTL4 = CCIS_0 + OUTMOD_0 + CCIE;
    TB0CCTL5 = CCIS_0 + OUTMOD_0 + CCIE;
    TB0CCTL6 = CCIS_0 + OUTMOD_0 + CCIE;
    TB0CTL = TASSEL_2 + MC_2 + TAIE;
}

void speaker_set_octave(uint8_t octave)
{
    current_octave = octave;
}

void speaker_set(uint8_t note, uint8_t octave)
{
    if(octave >= 0 && octave <= 2)
    {
        switch(note)
        {
            case 0: TA0CCTL0 &= ~OUTMOD_0; TA0CCTL0 |= OUTMOD_4; break;
            case 1: TA0CCTL1 &= ~OUTMOD_0; TA0CCTL1 |= OUTMOD_4; break;
            case 2: TA0CCTL2 &= ~OUTMOD_0; TA0CCTL2 |= OUTMOD_4; break;
            case 3: TB0CCTL3 &= ~OUTMOD_0; TB0CCTL3 |= OUTMOD_4; break;
            case 4: TB0CCTL4 &= ~OUTMOD_0; TB0CCTL4 |= OUTMOD_4; break;
            case 5: TB0CCTL5 &= ~OUTMOD_0; TB0CCTL5 |= OUTMOD_4; break;
            case 6: TB0CCTL6 &= ~OUTMOD_0; TB0CCTL6 |= OUTMOD_4; break;
            default: break;
        }

        current_octave = octave;
        double freq = freqs[current_octave][note];
        double output = (double) F_CPU / freq;
        outputs[note] = (uint16_t) output / 2;
    }
    else
    {
        switch(note)
        {
            case 0: TA0CCTL0 &= ~OUTMOD_4; TA0CCTL0 |= OUTMOD_0; break;
            case 1: TA0CCTL1 &= ~OUTMOD_4; TA0CCTL1 |= OUTMOD_0; break;
            case 2: TA0CCTL2 &= ~OUTMOD_4; TA0CCTL2 |= OUTMOD_0; break;
            case 3: TB0CCTL3 &= ~OUTMOD_4; TB0CCTL3 |= OUTMOD_0; break;
            case 4: TB0CCTL4 &= ~OUTMOD_4; TB0CCTL4 |= OUTMOD_0; break;
            case 5: TB0CCTL5 &= ~OUTMOD_4; TB0CCTL5 |= OUTMOD_0; break;
            case 6: TB0CCTL6 &= ~OUTMOD_4; TB0CCTL6 |= OUTMOD_0; break;
            default: break;
        }
    }
}

void speaker_set_all(uint8_t octave)
{
    for(uint8_t i = 0; i < 7; i++)
    {
        speaker_set(i, octave);
    }
}

uint16_t speaker_get_volume(uint8_t note)
{
    return outputs[note];
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    TA0CCR0 += outputs[0];
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A1_VECTOR
__interrupt void TIMER0_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A1_VECTOR))) TIMER0_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(TA0IV)
    {
        case 2: TA0CCR1 += outputs[1]; break;
        case 4: TA0CCR2 += outputs[2]; break;
        default: break;
    }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B1_VECTOR))) TIMER0_B1_ISR (void)
#else
#error Compiler not supported!
#endif
{
    switch(TB0IV)
    {
        case 6: TB0CCR3 += outputs[3]; break;
        case 8: TB0CCR4 += outputs[4]; break;
        case 10: TB0CCR5 += outputs[5]; break;
        case 12: TB0CCR6 += outputs[6]; break;
    }
}

