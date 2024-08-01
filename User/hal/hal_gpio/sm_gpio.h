/*
 * sm_gpio.h
 *
 *  Created on: Jun 12, 2024
 *      Author: lekhacvuong
 */

#ifndef SM_BOARD_SM_GPIO_SM_GPIO_H_
#define SM_BOARD_SM_GPIO_SM_GPIO_H_
#include "stdint.h"

typedef void sm_gpio_t;

sm_gpio_t* sm_gpio_create(void* _port, uint32_t _pin, uint8_t _mode);

int32_t sm_gpio_write(sm_gpio_t* _this, int8_t _value);

int32_t sm_gpio_read(sm_gpio_t* _this);

int32_t sm_gpio_toggle(sm_gpio_t* _this);

int32_t sm_gpio_destroy(sm_gpio_t* _this);

#endif /* SM_BOARD_SM_GPIO_SM_GPIO_H_ */
