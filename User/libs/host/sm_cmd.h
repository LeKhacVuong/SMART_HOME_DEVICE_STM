//
// Created by vnbk on 21/03/2023.
//

#ifndef SELEX_HOST_SM_CMD_H
#define SELEX_HOST_SM_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

/// CMD HOLDING

#define CMD_HOLDING_UPDATE_TIME              0
#define CMD_HOLDING_OPEN_DOOR                1
#define CMD_HOLDING_CONTROL_LIGHT_BED        2
#define CMD_HOLDING_SET_BED_BRIGHTNESS       3
#define CMD_HOLDING_CONTROL_FAN_BED          4
#define CMD_HOLDING_CONTROL_LIGHT_LIVING     5
#define CMD_HOLDING_SET_LIVING_BRIGHTNESS    6
#define CMD_HOLDING_CONTROL_LIGHT_KITCHEN    7
#define CMD_HOLDING_SET_KITCHEN_BRIGHTNESS   8
#define CMD_HOLDING_CONTROL_LIGHT_HALLWAY    9
#define CMD_HOLDING_SET_HALLWAY_BRIGHTNESS   10
#define CMD_HOLDING_CONTROL_FIRE_BUZZER      11
#define CMD_HOLDING_SET_AUTO_LIGHT           12
#define CMD_HOLDING_SET_AUTO_TIME            13
#define CMD_HOLDING_DELETE_AUTO_TIME         14
#define CMD_HOLDING_HALLWAY_HUMAN_DETECT     15
#define CMD_HOLDING_DEV_SEND_SYNC_DATA       16
#define CMD_HOLDING_SET_AUTO_FAN             17
#define CMD_HOLDING_BLINK_LED_TEST           20


#ifdef __cplusplus
};
#endif

#endif //SELEX_HOST_SM_CMD_H
