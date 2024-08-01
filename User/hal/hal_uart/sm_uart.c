/*
 * sm_uart.c
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#include "sm_uart.h"
#include <stdlib.h>
#include "stm32l1xx_hal.h"
#include "sm_elapsed_timer.h"

typedef struct sm_uart_impl{
	UART_HandleTypeDef* m_instance;
	uint16_t m_fifo_size;
	uint8_t* m_fifo;
	uint16_t m_fifo_head;
	uint16_t m_fifo_tail;
	uint16_t m_baudrate;
	uint8_t m_buffer;
}sm_uart_impl_t;

#define impl(x) ((sm_uart_impl_t*)(x))

sm_uart_t* sm_uart_create(void* _instance, uint32_t _baudrate, uint16_t _fifo_size){
	sm_uart_impl_t* this = malloc(sizeof(sm_uart_impl_t));

	if(!this)
		return NULL;

	this->m_fifo = malloc(_fifo_size);

	if(!this->m_fifo){
		free(this);
		return NULL;
	}

	this->m_instance = _instance;
	this->m_baudrate = _baudrate;
	this->m_fifo_size = _fifo_size;
	this->m_fifo_head = this->m_fifo_tail = 0;

	HAL_UART_Receive_IT(this->m_instance, &this->m_buffer, 1);
	return this;
}


int32_t sm_uart_set_baudrate(sm_uart_t* _this, uint32_t _baudrate){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	return 0;
}

int32_t sm_uart_recv_handle(sm_uart_t* _this){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	this->m_fifo[this->m_fifo_head] = this->m_buffer;
	this->m_fifo_head++;
	if(this->m_fifo_head >= this->m_fifo_size){
		this->m_fifo_head = 0;
	}
	HAL_UART_IRQHandler(this->m_instance);
	HAL_UART_Receive_IT(this->m_instance, &this->m_buffer, 1);
	return 0;
}



int32_t sm_uart_get_bytes_availabe(sm_uart_t* _this){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;


	uint16_t head = this->m_fifo_head;
	uint16_t tail = this->m_fifo_tail;
	uint16_t size = this->m_fifo_size;

	return (head >= tail) ? head - tail : size - tail + head;
}

int32_t sm_uart_clear_buffer(sm_uart_t* _this){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	this->m_fifo_head = this->m_fifo_tail = 0;
	return 0;
}

int32_t sm_uart_get_bytes(sm_uart_t* _this, uint8_t* _buffer, int32_t _size, uint32_t _timeout){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	elapsed_timer_t timeout;
	elapsed_timer_resetz(&timeout, _timeout);

	do{
		if(sm_uart_get_bytes_availabe(this) >= _size){
			for(int i = 0; i < _size; i++){
				_buffer[i] = this->m_fifo[this->m_fifo_tail];
				this->m_fifo_tail++;
				if(this->m_fifo_tail >= this->m_fifo_size){
					this->m_fifo_tail = 0;
				}
			}
			return _size;
		}
	}
	while(elapsed_timer_get_remain(&timeout));

	return 0;
}

int32_t sm_uart_send_bytes(sm_uart_t* _this, uint8_t* _data, int32_t _size, uint32_t _timeout){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	return HAL_UART_Transmit(this->m_instance, _data, _size, _timeout);
}



int32_t sm_uart_destroy(sm_uart_t* _this){
	sm_uart_impl_t* this = impl(_this);
	if(!this)
		return -1;

	free(this->m_fifo);
	free(this);
	return 0;
}
