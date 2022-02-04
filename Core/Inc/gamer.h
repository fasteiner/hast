/*
 * gamer.h
 *
 *  Created on: 19.01.2022
 *      Author: Philipp
 */

#ifndef INC_GAMER_H_
#define INC_GAMER_H_

#include "oled_c.h"
#include "ldc1000.h"
#include "stm32l4xx_hal.h"
#include <stdbool.h>
#include "application.h"

#define MAX_MESSAGE_LENGTH 114
#define MAX_PAYLOAD_LENGTH 110

/**
 * @brief GAMER Initialization
 *
 * Function initializes the GAMER Protocol
 * @param[in] UART_HandleTypeDef *		UART that is used to send messages
 * @param[in] volatile uint8_t *		Pointer to the message buffer
 */
void initGAMER(UART_HandleTypeDef *, volatile uint8_t *);

/**
 * @brief UART Interrupt Callback Function
 *
 * Function is called when a UART Interrupt is detected
 * @param[in] UART_HandleTypeDef *		UART that caused the interrupt
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *);

/**
 * @brief Send Error Message to UART
 *
 * Function sends defined Error Message to UART
 * @param[in] UART_HandleTypeDef *		UART that is used to send
 * @param[in] volatile uint8_t *		Pointer to the error message, must terminate with \0
 */
bool sendErrorMessage(UART_HandleTypeDef *, uint8_t *);

/**
 * @brief Print the menu to UART
 *
 * Sends the menu via UART
 * @param[in] UART_HandleTypeDef *		UART that is used to send
 * @param[in] bool						true to send welcome message, false to only display options
 */
void printMenu(UART_HandleTypeDef *, bool);


#endif /* INC_GAMER_H_ */
