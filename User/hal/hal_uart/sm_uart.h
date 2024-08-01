/*
 * sm_uart.h
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#ifndef SM_BOARD_SM_UART_SM_UART_H_
#define SM_BOARD_SM_UART_SM_UART_H_
#include "stdint.h"

typedef void sm_uart_t;

sm_uart_t* sm_uart_create(void* _instance, uint32_t _baudrate, uint16_t _fifo_size);

int32_t sm_uart_set_baudrate(sm_uart_t* _this, uint32_t _baudrate);

int32_t sm_uart_recv_handle(sm_uart_t* _this);

int32_t sm_uart_get_bytes_availabe(sm_uart_t* _this);

int32_t sm_uart_clear_buffer(sm_uart_t* _this);

int32_t sm_uart_get_bytes(sm_uart_t* _this, uint8_t* _buffer, int32_t _size, uint32_t _timeout);

int32_t sm_uart_send_bytes(sm_uart_t* _this, uint8_t* _data, int32_t _size, uint32_t _timeout);

int32_t sm_uart_destroy(sm_uart_t* _this);

#endif /* SM_BOARD_SM_UART_SM_UART_H_ */
