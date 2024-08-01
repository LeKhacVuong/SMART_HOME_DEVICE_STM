/*
 * board.h
 *
 *  Created on: Aug 1, 2024
 *      Author: lekhacvuong
 */

#ifndef USER_BOARD_BOARD_H_
#define USER_BOARD_BOARD_H_

#include "stm32l1xx_hal.h"
#include "stdbool.h"


enum LIGHT{
    BED_LIGHT,
    LIVING_LIGHT,
    KITCHEN_LIGHT,
    HALLWAY_LIGHT,
    LIGHT_NUMBER
};

typedef struct devTime_t{
    uint8_t m_hour;
    uint8_t m_min;
}devTime_t;

typedef struct light_info_t{
    uint8_t m_stt;
    uint8_t m_brightness;
    uint8_t m_auto;
    devTime_t m_autoConfigStart[3];
    devTime_t m_autoConfigStop[3];
}light_info_t;

typedef struct dev_info_t{
    devTime_t m_devTime;

    uint8_t m_doorStt;
    uint8_t m_bedFan;
    uint8_t m_humidity;
    uint8_t m_temperature;
    uint8_t m_fireSensor;
    uint8_t m_smokeSensor;
    uint8_t m_fireBuzzer;
    uint8_t m_hallwayDetectHuman;

    uint8_t m_autoFan;
    devTime_t m_autoFanStart;
    devTime_t m_autoFanStop;

    light_info_t m_light[LIGHT_NUMBER];
}dev_info_t;

#define DHTT11_2_Pin GPIO_PIN_0
#define DHTT11_2_GPIO_Port GPIOH
#define DHT11_1_Pin GPIO_PIN_1
#define DHT11_1_GPIO_Port GPIOH
#define OUT_DOOR_CRL_Pin GPIO_PIN_0
#define OUT_DOOR_CRL_GPIO_Port GPIOC
#define IN_DOOR_CLS_BT_Pin GPIO_PIN_1
#define IN_DOOR_CLS_BT_GPIO_Port GPIOC
#define IN_FIRE_SS_Pin GPIO_PIN_2
#define IN_FIRE_SS_GPIO_Port GPIOC
#define IN_SMOKE_SS_Pin GPIO_PIN_3
#define IN_SMOKE_SS_GPIO_Port GPIOC
#define OUT_FIRE_BUZZER_Pin GPIO_PIN_0
#define OUT_FIRE_BUZZER_GPIO_Port GPIOA
#define OUT_HUMAN_SS_Pin GPIO_PIN_1
#define OUT_HUMAN_SS_GPIO_Port GPIOA
#define OUT_FAN_CTRL_Pin GPIO_PIN_4
#define OUT_FAN_CTRL_GPIO_Port GPIOA
#define RC522_RST_Pin GPIO_PIN_11
#define RC522_RST_GPIO_Port GPIOB
#define RC522_CS_Pin GPIO_PIN_12
#define RC522_CS_GPIO_Port GPIOB
#define PWM_CHL_BED_Pin GPIO_PIN_6
#define PWM_CHL_BED_GPIO_Port GPIOB
#define PWM_CHL_LIVING_Pin GPIO_PIN_7
#define PWM_CHL_LIVING_GPIO_Port GPIOB
#define PWM_CHL_KITC_Pin GPIO_PIN_8
#define PWM_CHL_KITC_GPIO_Port GPIOB
#define PWM_CHL_HALLW_Pin GPIO_PIN_9
#define PWM_CHL_HALLW_GPIO_Port GPIOB


extern RTC_HandleTypeDef hrtc;
extern SPI_HandleTypeDef hspi2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void board_init();

void increaseTimeMin(devTime_t* _time);
bool isInTime(devTime_t _current, devTime_t _timeStart, devTime_t _timeStop);
int compareTime(devTime_t _first, devTime_t _second);


#endif /* USER_BOARD_BOARD_H_ */
