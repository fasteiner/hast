/*
 * application.c
 *
 *  Created on: 28.01.2022
 *      Author: Philipp & Fabian
 */

/* Includes  */

#include "application.h"

/* Functions */

// prints Coins to Display by triggering Print Interrupt
void printCoins(volatile uint8_t * messageToDisplay)
{
	// Coins are only printed when there isn't a message of higher priority on the display
	if(!prioOnly){
		char data[MAX_MESSAGE_LENGTH + 2];
		sprintf(data, "Coins:\n%u", (unsigned int)getCount());
		memcpy((uint8_t *)messageToDisplay, data, MAX_MESSAGE_LENGTH);
		isSoftwareInt = true;
		__HAL_GPIO_EXTI_GENERATE_SWIT(INT_PRINT_Pin);
		HAL_NVIC_SetPendingIRQ(EXTI9_5_IRQn);
	}
}

// Timer Callback Function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim){
	// Timer 6 prints the time every second
	if (htim->Instance==TIM6){
		HAL_RTC_GetTime(&hrtc, currTime, RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc, currDate, RTC_FORMAT_BIN);
		char timeBuff[20];
		sprintf(timeBuff,"%02d-%02d-%02d:%02d:%02d\n", currDate->Month, currDate->Date, currTime->Hours, currTime->Minutes, currTime->Seconds);
		oled_c_text_overwrite(oled_c, (uint8_t *)&timeBuff, 5, 0, 15 );
	}
	// Timer 7 clears the Display after set time passed
	if (htim->Instance==TIM7 && messageToDisplay[0] == 0){
		prioOnly = false;
		if (htim->State == HAL_TIM_STATE_BUSY)
			HAL_TIM_Base_Stop(htim);
		box_area(oled_c, 0, 20, 95, 128, 0);
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// LDC_INT_PIN is triggered when a coin is detected by LDC1000
	if (GPIO_Pin==LDC_INT_Pin)
		if(!incCount())
			box_area(oled_c, 0, 20, 95, 128, 0);

	// This Interrupt is triggered by Software when there is something to print in messageToDisplay
	if (GPIO_Pin==INT_PRINT_Pin && isSoftwareInt)
	{
		isSoftwareInt = false;
		displayMessageForTimeSlot(oled_c, &htim7, messageToDisplay);
	}
}
