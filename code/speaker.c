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
#define F_OUT 880
#define F_DIV F_CPU / F_OUT

uint8_t current_octave = 1;

double freqs[3][7] = {
    {130.8, 146.8, 164.8, 174.6, 196, 220, 146.9},
    {261.6, 293.7, 329.6, 349.2, 392, 440, 493.9},
    {523.2, 587.3, 659.2, 698.4, 784, 880, 987.8}
};

uint16_t outputs[3] = {0, 0, 0};

void speaker_init()
{
    P1DIR |= BIT5 + BIT6 + BIT7;
    P1SEL0 |= BIT5 + BIT6 + BIT7;
    P1SEL1 |= BIT5 + BIT6 + BIT7;
    TA0CCTL0 = CCIS_0 + OUTMOD_0 + CCIE;
    TA0CCTL1 = CCIS_0 + OUTMOD_0 + CCIE;
    TA0CCTL2 = CCIS_0 + OUTMOD_0 + CCIE;
    TA0CTL = TASSEL_2 + MC_2 + TAIE;
}

void speaker_set_octave(uint8_t octave)
{
    current_octave = octave;
}

void speaker_set_volume(uint8_t note, uint8_t volume)
{
    if(volume > 0)
    {
        switch(note)
        {
            case 0: TA0CCTL0 &= ~OUTMOD_0; TA0CCTL0 |= OUTMOD_4; break;
            case 1: TA0CCTL1 &= ~OUTMOD_0; TA0CCTL1 |= OUTMOD_4; break;
            case 2: TA0CCTL2 &= ~OUTMOD_0; TA0CCTL2 |= OUTMOD_4; break;
            default: break;
        }

        double freq = freqs[current_octave][note];
        double output = (double) F_CPU / freq;
        outputs[note] = output;
    }
    else
    {
        switch(note)
        {
            case 0: TA0CCTL0 &= ~OUTMOD_4; TA0CCTL0 |= OUTMOD_0; break;
            case 1: TA0CCTL1 &= ~OUTMOD_4; TA0CCTL1 |= OUTMOD_0; break;
            case 2: TA0CCTL2 &= ~OUTMOD_4; TA0CCTL2 |= OUTMOD_0; break;
            default: break;
        }
    }
}
/*
void speaker_set_volume(uint8_t note, uint8_t volume)
{
    outputs_simple[note] = volume;
    double freq = freqs[current_octave][note];
    double max_div_val = (double) F_CPU / freq;
    double high_div_val = max_div_val * ((double) volume / 200.0);
    double low_div_val = max_div_val - high_div_val;
    uart_tx_num((uint16_t) max_div_val);
    uart_tx(':');
    uart_tx_num((uint16_t) high_div_val);
    uart_tx('\n');
    outputs[note][0] = (uint16_t) high_div_val;
    outputs[note][1] = (uint16_t) low_div_val;

    if(volume > 0)
    {
        switch(note)
        {
            case 0: TA0CCTL0 |= OUTMOD_4; break;
            case 1: TA0CCTL1 |= OUTMOD_4; break;
            case 2: TA0CCTL2 |= OUTMOD_4; break;
            default: break;
        }
    }
    else
    {
        switch(note)
        {
            case 0: TA0CCTL0 |= OUTMOD_0; break;
            case 1: TA0CCTL1 |= OUTMOD_0; break;
            case 2: TA0CCTL2 |= OUTMOD_0; break;
            default: break;
        }
    }

}
*/
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

/*
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_B0_VECTOR))) Timer0_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
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
}
*/

