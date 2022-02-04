/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled_c.h"
#include "gamer.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "application.h"
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_RW_Pin GPIO_PIN_0
#define OLED_RW_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_1
#define OLED_RST_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define OLED_EN_Pin GPIO_PIN_3
#define OLED_EN_GPIO_Port GPIOA
#define LDC_INT_Pin GPIO_PIN_4
#define LDC_INT_GPIO_Port GPIOA
#define LDC_INT_EXTI_IRQn EXTI4_IRQn
#define LDC_CS_Pin GPIO_PIN_6
#define LDC_CS_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_0
#define OLED_CS_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_1
#define OLED_DC_GPIO_Port GPIOB
#define INT_PRINT_Pin GPIO_PIN_8
#define INT_PRINT_GPIO_Port GPIOA
#define INT_PRINT_EXTI_IRQn EXTI9_5_IRQn
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */
#define SCROLL_DELAY 7000
uint32_t DISPLAY_TIME;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;
RTC_HandleTypeDef hrtc;
TIM_HandleTypeDef htim7;
RTC_TimeTypeDef * currTime;
RTC_DateTypeDef * currDate;
volatile OLED_C * oled_c;
volatile bool prioOnly;
volatile bool isSoftwareInt;
volatile uint8_t * messageToDisplay;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
