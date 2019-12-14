/*
 * 2D Photo Harp
 *
 * Description
 *
 * Created: 12/3/19
 * Last Edited: 12/3/19
 * Author: Andrew Hollabaugh
 */

#include "main.h"
#include "uart.h"
#include "speaker.h"

int main()
{
    WDTCTL = WDTPW | WDTHOLD; //stop watchdog timer

    uart_init();
    speaker_init();

    __bis_SR_register(GIE); //enable interrupts

    while(1)
    {
        uart_rx_check_queue(); //check the uart_rx queue for new commands
    }
}

