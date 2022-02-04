/*
 * ldc1000.c
 *
 *  Created on: 01.01.2022
 *      Author: Philipp
 */

/* Includes  */

#include "ldc1000.h"

/*  Defines  */

/* Variables */
volatile uint32_t count;

/* Register Map */
enum registerMap{
	LDC_ID          = 0x00,
	LDC_RPMAX       = 0x01,
	LDC_RPMIN       = 0x02,
	LDC_FREQ        = 0x03,
	LDC_LDCCONF     = 0x04,
	LDC_CLKCONFIG   = 0x05,
	LDC_THRESHI_LSB = 0x06,
	LDC_THRESHI_MSB = 0x07,
	LDC_THRESLO_LSB = 0x08,
	LDC_THRESLO_MSB = 0x09,
	LDC_INTBCONF    = 0x0A,
	LDC_PWRCONF     = 0x0B,
	LDC_STATUS      = 0x20,
	LDC_PROXLSB     = 0x21,
	LDC_PROXMSB     = 0x22,
	LDC_FREQLSB     = 0x23,
	LDC_FREQMID     = 0x24,
	LDC_FREQMSB     = 0x25
};

/* Command to write */
enum commandWrite{
	// Rp_Max = 38.785K
	LDC_RPMAX_38_785K        = 0x11,
	// Rp_Min = 2.39K
	LDC_RPMIN_2_39K          = 0x3B,
	// Sensor Frequency - N = 217; F = 20% * Rf
	LDC_FREQ_WRITE           = 0xB0,
	// LDC Configuration - Amplitude = 2V, Response Time = 6144;
	LDC_LDCCONF_A2V_T6144    = 0x0F,
	// Clock Configuration - External Crystal used for Frequency Counter; Enable External time base clock;
	LDC_CLKCONF_EC_ET_ENABLE = 0x01,
	// Power Configuration - Active Mode, Conversion is Enabled
	LDC_PWRCONF_ACTIVE_MODE  = 0x01,
	// Enable DRDYB Mode (Interrupt Mode)
	LDC_COMP_ON              = 0x02,
	// Command SPI read
	LDC_SPIREAD    			 = 0x80
};

/* Function Prototypes */

/**
 * @brief Write to LDC1000
 *
 * @param[in] uint8_t						Address of the register to write
 * @param[in] uint8_t						Data to write
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
void writeLDC1000     (uint8_t, uint8_t, SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read from LDC 1000
 *
 * @param[in] uint8_t						Address of the register to read
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 * @param[out] uint8_t						Data read from LDC1000
 */
uint8_t readLDC1000   (uint8_t, SPI_HandleTypeDef*, GPIO_TypeDef *, uint32_t);

/* Functions */

// Reset the count Variable
void resetCount()
{
	count = 0;
}

// Sets the Count variable to set.
void setCount(uint32_t set)
{
	count = set;
}

// Increment the count Variable
uint32_t incCount()
{
	return ++count;
}

// Return count Variable
uint32_t getCount()
{
	return count;
}

// read and return INTB Conf from LDC 1000
uint8_t readINTBConf(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_INTBCONF, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return Clock Conf from LDC 1000
uint8_t readClkConf(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_CLKCONFIG, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return from LDC 1000 at given address
uint8_t readLDC1000(uint8_t address, SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	uint8_t data = 0;
	uint8_t prescaler = 0;  // Buffer for preconfigured Prescaler Value

	// Checks if the SPI Speed is set accordingly and adjusts it
	if (hspi->Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_32) {
		prescaler = hspi->Init.BaudRatePrescaler;
		hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		if (HAL_SPI_Init(hspi) != HAL_OK) {
			Error_Handler();
		}
	}

	// If the Address lacks the Read Flag it is added
	if (address < LDC_SPIREAD)
		address += LDC_SPIREAD;

	// Set Chip Select
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

	// Transmit address
	if ( HAL_SPI_Transmit(hspi, (uint8_t *) &address, sizeof(address), 100) != HAL_OK )
		Error_Handler();

	// Receive Data
	if ( HAL_SPI_Receive(hspi, (uint8_t *) &data, sizeof(uint8_t), 100) != HAL_OK)
		Error_Handler();

	// Unset Chip Select
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

	// Reset the Prescaler to its original value if it was altered
	if (prescaler)
	{
		hspi->Init.BaudRatePrescaler = prescaler;
		if (HAL_SPI_Init(hspi) != HAL_OK) {
			Error_Handler();
		}
	}

	return data;
}

// read and return Frequency Conf from LDC 1000
uint32_t readFreq(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	uint32_t freq = 0;
	freq = readLDC1000(LDC_FREQMSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	freq = freq<<8;
	freq += readLDC1000(LDC_FREQMID, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	freq = freq<<8;
	freq += readLDC1000(LDC_FREQLSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	return freq;
}

// read and return ID from LDC 1000
uint8_t readID(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_ID, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return RP Max Conf from LDC 1000
uint8_t readRpMax(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_RPMAX, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return RP Min Conf from LDC 1000
uint8_t readRpMin(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_RPMIN, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return Timer Frequency from LDC 1000
uint8_t readTimFreq(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_FREQ, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return LDC Conf from LDC 1000
uint8_t readLDCConf(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_LDCCONF, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return Comparator High Value from LDC 1000
uint16_t readCompHi(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	uint16_t buf = 0;
	buf = readLDC1000(LDC_THRESHI_MSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	buf <<= 8;
	buf += readLDC1000(LDC_THRESHI_LSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	return buf;
}

// read and return Comparator Low Value from LDC 1000
uint16_t readCompLo(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	uint16_t buf = 0;
	buf = readLDC1000(LDC_THRESLO_MSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	buf <<= 8;
	buf += readLDC1000(LDC_THRESLO_LSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	return buf;
}

// read and return Power Configuration from LDC 1000
uint8_t readPowerConf(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_PWRCONF, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return Status from LDC 1000
uint8_t readStatus(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	return readLDC1000(LDC_STATUS, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
}

// read and return Proximity Value from LDC 1000
uint32_t readProx(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	uint32_t prox = 0;

	// initiate conversion (manual page 19, table 14)
	readLDC1000(LDC_PROXLSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	//HAL_Delay(5);

	prox = readLDC1000(LDC_PROXMSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	prox = prox<<8;
	prox += readLDC1000(LDC_PROXLSB, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	return prox;
}

// Write to LDC 1000 at given address
void writeLDC1000( uint8_t address, uint8_t data, SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
    uint8_t buffer[2] = {address, data};
    uint8_t prescaler = 0;  // Buffer for preconfigured Prescaler Value

    // Checks if the SPI Speed is set accordingly and adjusts it
	if (hspi->Init.BaudRatePrescaler != SPI_BAUDRATEPRESCALER_32) {
		prescaler = hspi->Init.BaudRatePrescaler;
		hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		if (HAL_SPI_Init(hspi) != HAL_OK) {
			Error_Handler();
		}
	}

	// Set Chip Select
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

	// Transmit address and data
	if ( HAL_SPI_Transmit(hspi, (uint8_t *) &buffer, sizeof(buffer), 50) != HAL_OK )
	{
		Error_Handler();
		return;
	}

	// Unset Chip Select
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

	// Reset the Prescaler to its original value if it was altered
	if (prescaler)
	{
		hspi->Init.BaudRatePrescaler = prescaler;
		if (HAL_SPI_Init(hspi) != HAL_OK) {
			Error_Handler();
		}
	}
}

// Initialize LDC 1000 Settings
void applicationInit(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{

	count = 0;
    // Rp Maximum
    writeLDC1000( LDC_RPMAX, LDC_RPMAX_38_785K, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
    // Rp Minimum
    writeLDC1000( LDC_RPMIN, LDC_RPMIN_2_39K, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
    // Sensor Frequency
    writeLDC1000( LDC_FREQ, LDC_FREQ_WRITE, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
    // LDC Configuration
    writeLDC1000( LDC_LDCCONF, LDC_LDCCONF_A2V_T6144, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
    // Clock Configuration
    writeLDC1000( LDC_CLKCONFIG, LDC_CLKCONF_EC_ET_ENABLE, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
    // Power Configuration
    writeLDC1000( LDC_PWRCONF, LDC_PWRCONF_ACTIVE_MODE, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);

    HAL_Delay(100);


}

// Set Comparator Values and INTB Config to Comparator Mode in order for Interrupts to work
void enableInterrupt(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin, uint16_t compLo, uint16_t compHi)
{
	writeLDC1000(LDC_THRESHI_MSB, compHi / 256, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	writeLDC1000(LDC_THRESHI_LSB, compHi % 256, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	writeLDC1000(LDC_THRESLO_MSB, compLo / 256, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	writeLDC1000(LDC_THRESLO_LSB, compLo % 256, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	writeLDC1000( LDC_INTBCONF, LDC_COMP_ON, hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	HAL_Delay(100);
}

// Calculates the Inductance (Formula from reference implementation)
float getInductance(SPI_HandleTypeDef* hspi, GPIO_TypeDef * SPI_CS_GPIO_Port, uint32_t SPI_CS_Pin)
{
	float buf = readFreq (hspi, SPI_CS_GPIO_Port, SPI_CS_Pin);
	float fFreqCount = ( 1.0 / 3.0 ) * ( 8000000.0 / buf ) * 6144.0;
	return (float) ( 1000000.0 / ( 0.0000000001 * ( ( 2.0 * 3.14 * fFreqCount ) * ( 2.0 * 3.14 * fFreqCount ) ) ) );
}
