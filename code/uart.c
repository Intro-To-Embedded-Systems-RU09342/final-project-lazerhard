/*
 * uart.c
 *
 * Functionality for transmitting and receiving data over uart
 *
 * Created: 11/7/19
 * Last Edited: 12/1/19
 * Author: Andrew Hollabaugh
 */

#include "uart.h"
#include "rx_queue.h"
#include "fan.h"
#include "main.h"

//command characters
#define SET_TEMP_CMD 'T'
#define SET_FAN_CMD 'F'

#define CMD_BUFFER_LENGTH 8 //max length of command array
char cmd[CMD_BUFFER_LENGTH]; //array to store receieved command in
uint8_t cmd_len = 0; //length of command array

void uart_init()
{
    // Configure GPIO
    P2SEL0 |= BIT0 | BIT1;                    // USCI_A0 UART operation
    P2SEL1 &= ~(BIT0 | BIT1);

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY >> 8;                    // Unlock clock registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;             // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;     // Set all dividers
    CSCTL0_H = 0;                             // Lock CS registers

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                      // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

//transmits a byte
void uart_tx(char data)
{
    while(!(UCA0IFG & UCTXIFG)); //wait for tx buffer to be ready
    UCA0TXBUF = data; //put byte in transmission buffer
}

//transmits a string
void uart_tx_str(const char *str)
{
    while(*str != '\0') //loop until end of string
    {
        while(!(UCA0IFG & UCTXIFG)); //wait for tx buffer to be ready
        UCA0TXBUF = *str; //put current byte in transmission buffer

        if(*str == '\n') //transmit \r if byte is \n
        {
            while(!(UCA0IFG & UCTXIFG));
            UCA0TXBUF = '\r';
        }
        str++; //go to next byte
    }
}

//transmits an integer in decmial format
void uart_tx_num(uint32_t num)
{
    char num_str[10] = "";
    uint8_t i = 0;
    if(num == 0) //special case for zero
    {
        num_str[0] = '0';
        i++;
    }
    else
    {
        while(num > 0) //loop until all digits of num are processed
        {
            num_str[i] = num % 10 + '0'; //add rightmost digit of num to the string
            num /= 10; //remove rightmost digit
            i++;
        }
    }

    char num_str_rev[10] = "";
    uint8_t j = 0;
    while(j <= i) //reverse the string
    {
        num_str_rev[j] = num_str[i-j-1];
        j++;
    }
    num_str_rev[i] = '\0'; //add end of string character
    uart_tx_str(num_str_rev); //transmit the string
}

//check the uart_rx queue for characters that were sent and start execution sequence
void uart_rx_check_queue()
{
    if(!rx_queue_is_empty())
        uart_rx_add_char_to_cmd(rx_queue_pop()); //remove character from queue and execute it
}

//adds the char to the command array; command is terminated at \n
void uart_rx_add_char_to_cmd(char c)
{
    if(c != '\n') //add normal char
    {
        cmd[cmd_len++] = c;
    }
    else if(cmd_len > 0) //newline is received; terminate command and execute
    {
        if(cmd_len == 1)
            uart_rx_execute_cmd();
        else
            uart_rx_execute_cmd_with_arg();
        cmd_len = 0;
    }
}

//no commands without arguments, so useless for now
void uart_rx_execute_cmd()
{
}

//executes commands that have an argument after converting argument to an integer
void uart_rx_execute_cmd_with_arg()
{
    uint16_t arg = 0;
    uint8_t arg_len = cmd_len - 1;

    //does place value math on each arg char, and adds to arg int
    for(uint8_t i = 1; i <= arg_len; i++)
    {
        uint8_t exp_factor = arg_len - i;
        uint16_t mul_factor = 1;
        for(uint8_t j = 0; j < exp_factor; j++)
            mul_factor *= 10;
        arg += (cmd[i]-'0') * mul_factor;
    }

    //execute command based on first char of command array
    if(cmd[0] == SET_TEMP_CMD)
        fan_set_auto(arg);
    else if(cmd[0] == SET_FAN_CMD)
        fan_set_manual(arg);
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
        //push received characters to the rx_queue
        if(!rx_queue_is_full())
            rx_queue_push(UCA0RXBUF);
        break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
  }
}

