/*
 * board.h
 *
 *  Created on: Aug 14, 2024
 *      Author: lekhacvuong
 */

#ifndef INC_BOARD_H_
#define INC_BOARD_H_

#include "main.h"
#include "stdio.h"
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
    uint8_t m_fireSensor;
    uint8_t m_fireBuzzer;
    uint8_t m_hallwayDetectHuman;

    uint8_t m_autoFan;
    devTime_t m_autoFanStart;
    devTime_t m_autoFanStop;

    light_info_t m_light[LIGHT_NUMBER];

    uint8_t m_isFire;

    char m_doorKey[8];

    uint8_t m_humi;
    uint8_t m_temp;
}dev_info_t;

extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart2;


void board_init();

void increaseTimeMin(devTime_t* _time);
bool isInTime(devTime_t _current, devTime_t _timeStart, devTime_t _timeStop);
int compareTime(devTime_t _first, devTime_t _second);

#endif /* INC_BOARD_H_ */
