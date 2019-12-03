/*
 * fan.c
 *
 * Controls fan using PWM. Uses a PI loop to regulate fan based on temperature
 *
 * Created: 11/15/19
 * Last Edited: 12/1/19
 * Author: Andrew Hollabaugh
 */

#include "fan.h"
#include "uart.h"
#include "temp.h"

#define CONTROL_LOOP_PERIOD 50000
#define MAX_OUTPUT 10000 //max fan pwm value
#define MIN_OUTPUT 0 //min fan pwm value
#define FAN_MIN_SPEED 1000

uint16_t target = 0;
uint16_t output = 0;
int64_t kp = -10; //p constant
double ki = -0.1; //i constant
int64_t i_term = 0;
uint8_t control_mode = 0; //manual = 0, auto = 1

void fan_init()
{
    //timer A0, for fan output, P1.6
    P1DIR |= BIT6; //set 1.6 as output
    P1SEL1 |= BIT6; //set 1.6 for tertiary function, which is pwm
    P1SEL0 |= BIT6;
    TA0CTL |= TASSEL_2 + MC_1; //smclk, up mode
    TA0CCTL1 |= OUTMOD_7; //pwm output
    TA0CCR0 = MAX_OUTPUT - 1; //define pwm frequency
    TA0CCR1 = 0; //start with zero output

    //timer A1, for control loop timing
    TA1CTL |= TASSEL_2 + MC_2 + ID_3; //smclk, continuous mode
    TA1CCTL0 = CCIE; //enable CCR0 interrupt
    TA1CCR0 = CONTROL_LOOP_PERIOD;
}

//set the fan to a speed
void fan_set(uint16_t speed)
{
    //make output LOW if speed is 0
    if(speed <= 0)
    {
        TA0CCTL1 &= ~OUT; //make output constant
        TA0CCTL1 &= ~OUTMOD_7; //disable pwm out
        TA0CCTL1 |= OUTMOD_0; //output is LOW
        output = MIN_OUTPUT;
    }
    //make output HIGH if speed is max
    else if(speed >= MAX_OUTPUT)
    {
        TA0CCTL1 |= OUT;
        TA0CCTL1 &= ~OUTMOD_7;
        TA0CCTL1 |= OUTMOD_0;
        output = MAX_OUTPUT;
    }
    //set speed to speed parameter
    else
    {
        TA0CCTL1 |= OUTMOD_7;
        TA0CCR1 = speed;
        output = speed;
    }
}

//set the fan speed manually, called when F command is sent
void fan_set_manual(uint16_t speed)
{
    control_mode = 0;
    fan_set(speed);
}

//set the fan to attempt a specified temperature, called when T command is sent
void fan_set_auto(uint16_t temp)
{
    control_mode = 1;
    target = temp;
}

//get current fan speed
uint16_t fan_get_speed()
{
    return output;
}

//calculate fan speed using PI loop; uses milli degrees C for greater accuracy
void fan_calc_speed()
{
    //only run when in auto mode
    if(control_mode == 1)
    {
        int64_t current_value = temp_get(); //get temp in milli degrees C
        int64_t error = target - current_value; //calculate error from different from target and current temps

        int64_t p_term = kp * error; //proportional term is just a scalar of the error
        i_term += (int64_t)(ki * error); //integral term is a scalar of the error that accumulates over multiple control cycles

        //cap the integral term to prevent windup
        if(i_term > MAX_OUTPUT)
            i_term = MAX_OUTPUT;
        else if(i_term < MIN_OUTPUT)
            i_term = MIN_OUTPUT;

        //calculate output as sum of two terms
        int64_t new_output = p_term + i_term;

        //cap the output at the output range
        if(new_output > MAX_OUTPUT)
            new_output = MAX_OUTPUT;
        else if(new_output < MIN_OUTPUT)
            new_output = MIN_OUTPUT;

        //if need to cool down, make sure fan is moving at at least the min speed
        if(error < 0 && new_output < FAN_MIN_SPEED)
            new_output = FAN_MIN_SPEED;
        
        fan_set((uint16_t) new_output);
    }
}

//recalculate fan speed every period of the control loop timer
void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) TIMER1_A0_ISR (void)
{
    fan_calc_speed();
}

