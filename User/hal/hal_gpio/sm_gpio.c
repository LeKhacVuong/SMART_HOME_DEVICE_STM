/*
 * sm_gpio.c
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#include "sm_gpio.h"
#include "stddef.h"
#include "stdlib.h"
#include "stm32l1xx_hal.h"

typedef struct sm_gpio_impl{
	GPIO_TypeDef* m_port;
	uint32_t m_pin;
}sm_gpio_impl_t;

#define impl(x) ((sm_gpio_impl_t*)(x))

sm_gpio_t* sm_gpio_create(void* _port, uint32_t _pin, uint8_t _mode){

	sm_gpio_impl_t* this = malloc(sizeof(sm_gpio_impl_t));
	if(!this)
		return NULL;

	this->m_port = _port;
	this->m_pin = _pin;
	sm_gpio_write(this, 0);
	return this;
}

int32_t sm_gpio_write(sm_gpio_t* _this, int8_t _value){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	HAL_GPIO_WritePin(this->m_port, this->m_pin, _value);
	return 0;
}

int32_t sm_gpio_read(sm_gpio_t* _this){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;


	return HAL_GPIO_ReadPin(this->m_port, this->m_pin);
}

int32_t sm_gpio_toggle(sm_gpio_t* _this){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	sm_gpio_write(this, !sm_gpio_read(_this));
	return 0;
}

int32_t sm_gpio_destroy(sm_gpio_t* _this){
	sm_gpio_impl_t* this = impl(_this);
	if(!this)
		return -1;

	free(this);
	return 0;
}
