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
#include "stm32f3xx_hal.h"

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
#define GAIN_Pin GPIO_PIN_0
#define GAIN_GPIO_Port GPIOA
#define AOUT_Pin GPIO_PIN_4
#define AOUT_GPIO_Port GPIOA
#define DOUT1_Pin GPIO_PIN_5
#define DOUT1_GPIO_Port GPIOA
#define DOUT_Pin GPIO_PIN_6
#define DOUT_GPIO_Port GPIOA
#define IN0_Pin GPIO_PIN_0
#define IN0_GPIO_Port GPIOB
#define IN2_back_Pin GPIO_PIN_1
#define IN2_back_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_2
#define IN1_GPIO_Port GPIOB
#define BUTTON0_Pin GPIO_PIN_15
#define BUTTON0_GPIO_Port GPIOB
#define IN3_Pin GPIO_PIN_8
#define IN3_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_8
#define LED2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_9
#define LED1_GPIO_Port GPIOA
#define LED0_Pin GPIO_PIN_10
#define LED0_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
