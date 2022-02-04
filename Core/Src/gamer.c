/*
 * gamer.c
 *
 *  Created on: 19.01.2022
 *      Author: Philipp & Fabian
 */

/* Includes  */

#include "gamer.h"


/*  Defines  */

/**
 * @brief Flush all buffers in gamer.c
 */
void flushBuffers();
bool setDateTime(char *);
bool runMessage(UART_HandleTypeDef *);
bool getCommand(UART_HandleTypeDef *);
bool answerCommand(UART_HandleTypeDef *);
bool messageCommand(UART_HandleTypeDef *);
bool errorCommand(UART_HandleTypeDef *);
bool resetCommand(UART_HandleTypeDef *);
bool sendError(UART_HandleTypeDef *);
bool sendUART(UART_HandleTypeDef *, uint8_t [], uint8_t);
bool sendACK(UART_HandleTypeDef *);


/* Variables */
// Indicates that a message has started because a # was received
bool isMessage;

// Message is stored here upon receive
volatile uint8_t message[MAX_MESSAGE_LENGTH] = {0};

// Buffer Variable for sending to UART
char *txBuffer/*[MAX_MESSAGE_LENGTH] = {0}*/;

// messageIndex indicates the next free index on message; data is a Buffer to receive from DMA
volatile uint8_t messageIndex, data;

// Pointer to messageToDisplay Variable in main.c where Messages are stored that are going to be printed on the Display
volatile uint8_t * messageToDisplay;


/* Functions */

// Initialize Variables, enable Interrupts and DMA
void initGAMER(UART_HandleTypeDef *huart, volatile uint8_t * _messageToDisplay)
{
	messageToDisplay = _messageToDisplay;
	txBuffer = malloc(sizeof(char) * MAX_MESSAGE_LENGTH);
	flushBuffers();
	// Enable Interrupts for UART2
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

	// Start UART2 in Interrupt Mode
	HAL_UART_Receive_DMA(huart, (uint8_t *)&data, sizeof(data));
}

// After a End Of Message - \0 - was received, the message is evaluated here
bool runMessage(UART_HandleTypeDef *huart)
{
	uint32_t newVal;
	// At index zero there is the command
	switch(message[0])
		{
		case 'g': // Send current Coin Count via UART
			if(!getCommand(huart))
				sendError(huart);
			break;
		case 'a': // Print answer to a previous message to Display unless its an Acknowledgment
			if(message[1] == 'A' && message[2] == 'C' && message[3] == 'K' && message[4] == '\0')
				break;
			prioOnly = true;
			if(!answerCommand(huart))
				sendError(huart);
			break;
		case 'm': // Print Message to Display
			prioOnly = true;
			if(!messageCommand(huart))
				sendError(huart);
			break;
		case 'e': // Error received
			if(!errorCommand(huart))
				sendError(huart);
			break;
		case 'r': // Reset Coin Count
			prioOnly = false;
			box_area(oled_c, 0, 20, 95, 128, 0);
			if(!resetCommand(huart))
				sendError(huart);
			break;
		case 'd': // Display number of coins on Display
			prioOnly = false;
			box_area(oled_c, 0, 20, 95, 128, 0);
			printCoins(messageToDisplay);
			sendACK(huart);
			break;
		case 's': // Set values on the MC
		{
			switch(message[1])
			{
			case 'c': // Set the coin counter variable
				newVal = (uint32_t)strtoul((char*)&message+2, NULL, 10);
				if(newVal > 0)  // Value must be greater than Zero
				{
					setCount(newVal);
					prioOnly = false; // Set Priority to low so it doesn't overwrite messages
					box_area(oled_c, 0, 20, 95, 128, 0);
					printCoins(messageToDisplay);
					sendACK(huart);
				}
				else // Send Errormessage
				{
					uint8_t errorMessage[] = "Value must be numeric and greater than Zero!";
					sendErrorMessage(huart, (uint8_t*)&errorMessage);
				}
				break;
			case 't': // Set Time and Date
				{
					char * stringptr = (char*) &message + 2;

					// Correct request must be at least 19 characters long and finish successfully
					if(strlen (stringptr) >= 19 && setDateTime(stringptr))
						sendACK(huart);
					else
					{
						uint8_t errorMessage[] = "Date and Time must be valid and meet this format: YYYY-MM-DD-hh:mm:ss";
						sendErrorMessage(huart, (uint8_t*)&errorMessage);
					}
				}
				break;
			case 'd': //set Displaytime of messages, minimum 25, maximum 60
				newVal = atoi((char*)&message+2);
				if (newVal >= 25 && newVal <= 60) // Value must be between 25 and 60
				{
					DISPLAY_TIME = newVal * 1000;
					sendACK(huart);
				}
				else
				{
					uint8_t errorMessage[] = "Value must be numeric and between 25 and 60!";
					sendErrorMessage(huart, (uint8_t*) &errorMessage);
				}
				break;
			default:
			{
				uint8_t errorMessage[] = "Command not supported!";
				sendErrorMessage(huart, (uint8_t*) &errorMessage);
				break;
			}
			}
			break;
		}
		case '?':
		{
		printMenu(huart, false);
		sendACK(huart);
		break;
		}
		default: // Message invalid, send Error and Flush Buffers
			flushBuffers();
			uint8_t errorMessage[] = "Command not supported!";
			sendErrorMessage(huart, (uint8_t*) &errorMessage);
			return false;
			break;
		}
	flushBuffers();
	return true;
}

// Validates and sets date and time
bool setDateTime(char * stringptr)
{
	RTC_TimeTypeDef *sTime = malloc(sizeof(RTC_TimeTypeDef));
	RTC_DateTypeDef *sDate = malloc(sizeof(RTC_DateTypeDef));
	//splits the string on the given delimiter
	char *token = strtok(stringptr, "-");
	sDate->Year = (uint8_t) (atoi(token) - 2000);
	token = strtok(NULL, "-");
	sDate->Month = (uint8_t) atoi(token);
	token = strtok(NULL, "-");
	sDate->Date = (uint8_t) atoi(token);
	//change delimiter to parse the time
	token = strtok(NULL, "-");
	token = strtok(token, ":");
	sTime->Hours = (uint8_t) atoi(token);
	token = strtok(NULL, ":");
	sTime->Minutes = (uint8_t) atoi(token);
	token = strtok(NULL, ":");
	sTime->Seconds = (uint8_t) atoi(token);
	sTime->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime->StoreOperation = RTC_STOREOPERATION_RESET;
	//validate the values
	if(sDate->Year > 99 || sDate->Month > 12 || sDate->Date > 31 || sTime->Hours > 23 ||
			sTime->Minutes > 59 || sTime->Seconds > 59 || sDate->Month == 0 || sDate->Date == 0)
	{
		free(sTime);
		free(sDate);
		return false;
	}
	//Sets the RTC Time
	if (HAL_RTC_SetTime(&hrtc, sTime, RTC_FORMAT_BIN) != HAL_OK) {
		free(sTime);
		free(sDate);
		return false;
	}
	//Sets the RTC Date
	if (HAL_RTC_SetDate(&hrtc, sDate, RTC_FORMAT_BIN) != HAL_OK) {
		free(sTime);
		free(sDate);
		return false;
	}
	//free the memory
	free(sTime);
	free(sDate);
	return true;
}

// Executes the get-command: Polls the Data and sends it via UART
bool getCommand(UART_HandleTypeDef *huart)
{
	sprintf(txBuffer,"#a%u",(unsigned int)getCount());
	if(!sendUART(huart, (uint8_t*)txBuffer, strlen(txBuffer)+1))
		return false;
	return true;
}

// Executes the answer-command: Prefixes "Answer: " to the Message and prints it on Display
bool answerCommand(UART_HandleTypeDef *huart)
{
	uint8_t ans[] = "Answer:\n";
	memcpy((uint8_t *)messageToDisplay, &ans, sizeof(ans));
	memcpy((uint8_t *)messageToDisplay+sizeof(ans)-1, (uint8_t*)message + sizeof(message[0]), MAX_MESSAGE_LENGTH-1);
	isSoftwareInt = true;
	__HAL_GPIO_EXTI_GENERATE_SWIT(INT_PRINT_Pin);
	HAL_NVIC_SetPendingIRQ(EXTI9_5_IRQn);
	return sendACK(huart);
}

// Executes the message-command: Prints it on Display
bool messageCommand(UART_HandleTypeDef *huart)
{
	memcpy((uint8_t*)messageToDisplay, (uint8_t*)message + sizeof(message[0]), MAX_MESSAGE_LENGTH-1);
	isSoftwareInt = true;
	__HAL_GPIO_EXTI_GENERATE_SWIT(INT_PRINT_Pin);
	HAL_NVIC_SetPendingIRQ(EXTI9_5_IRQn);
	return sendACK(huart);
}

// Executes the error-command: ACK for Error is sent via UART
bool errorCommand(UART_HandleTypeDef *huart)
{
	uint8_t mess[17] = {'#', 'a', 'E', 'r', 'r', 'o', 'r', ' ', 'r', 'e', 'c', 'e', 'i', 'v', 'e', 'd', '\0'};
	if(!sendUART(huart, (uint8_t*)&mess, 17))
		return false;
	return true;
}

// Executes the reset-command: resets the count Variable and sends ACK via UART
bool resetCommand(UART_HandleTypeDef *huart)
{
	resetCount();
	return sendACK(huart);
}

// Sends an ACK via UART
bool sendACK(UART_HandleTypeDef *huart)
{
	uint8_t data[] = "#aACK\0";
	memcpy((uint8_t*)txBuffer, &data, sizeof(data));
	if (!sendUART(huart, (uint8_t*)txBuffer, strlen(txBuffer) + 1))
		return false;
	return true;
}

// Sends an Error Message without Payload
bool sendError(UART_HandleTypeDef *huart)
{
	uint8_t data[MAX_MESSAGE_LENGTH] = {'#', 'e', '\0'};
	memcpy((uint8_t*)txBuffer, (uint8_t*)&data, MAX_MESSAGE_LENGTH);
	if(!sendUART(huart, (uint8_t*)txBuffer, strlen(txBuffer))+1)
		return false;
	return true;
}

// Sends an Error Message with Payload
bool sendErrorMessage(UART_HandleTypeDef *huart, uint8_t * mess)
{
	uint8_t ans[MAX_MESSAGE_LENGTH] = "#e", lengthMess = (strlen((char *)mess) +1);
	if(lengthMess > MAX_MESSAGE_LENGTH - 3)
		lengthMess = ( MAX_MESSAGE_LENGTH - 3);

	memcpy((uint8_t *)&ans + strlen((char*)&ans), mess, lengthMess);
	memcpy((uint8_t*)txBuffer, (uint8_t*)&ans, MAX_MESSAGE_LENGTH);
	return sendUART(huart, (uint8_t*)txBuffer, strlen(txBuffer)+1);
}

// Send mess via UART
bool sendUART(UART_HandleTypeDef *huart, uint8_t mess[], uint8_t len)
{
	HAL_UART_Transmit_DMA(huart, mess, len);
	return true;
}

// flushes all Buffers
void flushBuffers()
{
	isMessage = false;
	messageIndex = 0;
	data = 0;
	memset((uint8_t*)&message, 0, MAX_MESSAGE_LENGTH);
}

// sends the menu via UART. When isWelcome is true, it also sends a welcome message
void printMenu(UART_HandleTypeDef *huart, bool isWelcome)
{
	char * localBuffer;
	if(isWelcome)
		localBuffer = calloc(920, 1);
	else
		localBuffer = calloc(500, 1);
	if(isWelcome)
	{
		sprintf(localBuffer,
				"-----------------------------------------\n-------Welcome to our MCSD Project-------");

		sprintf(localBuffer + strlen(localBuffer),
				"\n-----------------------------------------\n /$$   /$$  /$$$$$$   /$$$$$$  /$$$$$$$$");

		sprintf(localBuffer + strlen(localBuffer),
				"\n	| $$  | $$ /$$__  $$ /$$__  $$|__  $$__/\n	| $$  | $$| $$  \\ $$| $$  \\__/   | $$");

		sprintf(localBuffer + strlen(localBuffer),
				"\n	| $$$$$$$$| $$$$$$$$|  $$$$$$    | $$\n	| $$__  $$| $$__  $$ \\____  $$   | $$");

		sprintf(localBuffer + strlen(localBuffer),
				"\n	| $$  | $$| $$  | $$ /$$  \\ $$   | $$\n	| $$  | $$| $$  | $$|  $$$$$$/   | $$");

		sprintf(localBuffer + strlen(localBuffer),
				"\n	|__/  |__/|__/  |__/ \\______/    |__/\n-----------------------------------------");
	}
	sprintf(localBuffer + strlen(localBuffer),
			"\nYour options are:\n#g   Receive the current amount of coins\n#a   Send an Answer");

	sprintf(localBuffer + strlen(localBuffer),
			"\n#m   Send a message\n#e   Send an error\n#r   Reset amount of coins\n#d   Display number of coins on Display");

	sprintf(localBuffer + strlen(localBuffer),
			"\n#s...\n#sc  Set amount of coins\n#st  Set time and date YYYY-MM-DD-hh:mm:ss");

	sprintf(localBuffer + strlen(localBuffer),
			"\n#sd  Set delay time that messages\n     are displayed between 25 and 60");

	sprintf(localBuffer + strlen(localBuffer),
			"\n#?   Show this menu again.\n-----------------------------------------");

	HAL_UART_Transmit(huart, (uint8_t *)localBuffer, strlen(localBuffer) + 1, 100);
	free(localBuffer);
}

// Callback function for when a Message was received on UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// Enable Interrupt and DMA
	__HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
	HAL_UART_Receive_DMA(huart, (uint8_t *)&data, sizeof(data));

	// received data without prior #
	if(data != '#' && !isMessage)
		return;

	// beginn of message, Hash is skipped as implicit Information and isMessage is set to true
	if(data == '#' && !isMessage)
	{
		isMessage = true;
		return;
	}

	// read message
	message[messageIndex] = data;

	// If the Message is too long, an error is sent via UART and the message is discarded
	if(messageIndex > MAX_PAYLOAD_LENGTH && data != '\0')
	{
		flushBuffers();
		uint8_t errorMessage[] = "Message too long";
		sendErrorMessage(huart, (uint8_t*)&errorMessage);
		return;
	}

	messageIndex++;

	// The End Of Message was received
	if(data == '\0')
		runMessage(huart);
}
