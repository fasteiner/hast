/*
 * application.h
 *
 *  Created on: Jan 28, 2022
 *      Author: Fabian & Philipp
 */



#ifndef APPLICATION_H_
#define APPLICATION_H_

/* Includes */

#include "main.h"
#include <stdlib.h>

/* Function Prototypes */

void printCoins(volatile uint8_t *);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef*);
void HAL_GPIO_EXTI_Callback(uint16_t);

#endif /* APPLICATION_H_ */
