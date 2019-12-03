#ifndef UART_H
#define UART_H

#include <msp430.h>
#include <stdint.h>

void uart_init(void);
void uart_tx(char data);
void uart_tx_str(const char* data);
void uart_tx_num(uint32_t num);
void uart_rx_check_queue(void);
void uart_rx_add_char_to_cmd(char c);
void uart_rx_execute_cmd(void);
void uart_rx_execute_cmd_with_arg(void);

#endif

