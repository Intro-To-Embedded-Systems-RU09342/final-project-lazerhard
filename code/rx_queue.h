#ifndef RX_QUEUE_H
#define RX_QUEUE_H

#include <stdbool.h>
#include <stdint.h>

#define MAX 30

char rx_queue_peek(void);
bool rx_queue_is_empty(void);
bool rx_queue_is_full(void);
uint32_t rx_queue_size(void);
void rx_queue_push(char data);
char rx_queue_pop(void);

#endif

