/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
