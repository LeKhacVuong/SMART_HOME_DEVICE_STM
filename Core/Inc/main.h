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
#include "stm32f1xx_hal.h"

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
#define DHT11_CRL_Pin GPIO_PIN_2
#define DHT11_CRL_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_3
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_RST_Pin GPIO_PIN_4
#define SPI1_RST_GPIO_Port GPIOA
#define TX_3_Pin GPIO_PIN_10
#define TX_3_GPIO_Port GPIOB
#define RX_3_Pin GPIO_PIN_11
#define RX_3_GPIO_Port GPIOB
#define CHL_HALL_Pin GPIO_PIN_8
#define CHL_HALL_GPIO_Port GPIOA
#define CHL_LIV_Pin GPIO_PIN_9
#define CHL_LIV_GPIO_Port GPIOA
#define CHL_KIT_Pin GPIO_PIN_10
#define CHL_KIT_GPIO_Port GPIOA
#define CHL_BED_Pin GPIO_PIN_11
#define CHL_BED_GPIO_Port GPIOA
#define OUT_DOOR_RL_Pin GPIO_PIN_3
#define OUT_DOOR_RL_GPIO_Port GPIOB
#define OUT_BUZZ_Pin GPIO_PIN_4
#define OUT_BUZZ_GPIO_Port GPIOB
#define OUT_FAN_RL_Pin GPIO_PIN_5
#define OUT_FAN_RL_GPIO_Port GPIOB
#define IN_FIRE_SS_Pin GPIO_PIN_6
#define IN_FIRE_SS_GPIO_Port GPIOB
#define IN_SMOKE_SS_Pin GPIO_PIN_7
#define IN_SMOKE_SS_GPIO_Port GPIOB
#define IN_DOOR_BT_Pin GPIO_PIN_8
#define IN_DOOR_BT_GPIO_Port GPIOB
#define IN_HUMAN_SS_Pin GPIO_PIN_9
#define IN_HUMAN_SS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
