/*
 * ldc1000.h
 *
 *  Created on: Jan 12, 2022
 *      Author: Philipp
 */



#ifndef INC_LDC1000_H_
#define INC_LDC1000_H_

#include "stm32l4xx_hal.h"
#include "main.h"

/**
 * @brief Reset counter variable
 *
 * Function resets coin counter
 */
void resetCount();

/**
 * @brief Increment count
 *
 * Function increments counter
 * @param[out] uint32_t			new counter Value
 */
uint32_t incCount();

/**
 * @brief Set count to given value
 *
 * Function sets count variable
 * @param[in] uint32_t			Desired counter value
 */
void setCount(uint32_t);

/**
 * @brief Get counter value
 *
 * @param[out] uint32_t			counter Value
 */
uint32_t getCount();

/**
 * @brief Read INTB Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readINTBConf  (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Clock Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readClkConf   (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Frequency Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint32_t readFreq     (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read ID from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readID        (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read RP MAximum Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readRpMax     (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read RP Minimum from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readRpMin     (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Timer Frquency from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readTimFreq   (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read LDC Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readLDCConf   (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Comparator High Value from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint16_t readCompHi   (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Comparator Low Value from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint16_t readCompLo   (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Power Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readPowerConf (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Status from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint8_t readStatus    (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read Proximity Value from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
uint32_t readProx     (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Initilize LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
void applicationInit  (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

/**
 * @brief Read INTB Config from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 * @param[in] uint16_t						Comparator Low Value
 * @param[in] uint16_t						Comparator High Value
 */
void enableInterrupt  (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t, uint16_t, uint16_t);

/**
 * @brief Read Inductance Value from LDC1000
 *
 * @param[in] SPI_HandleTypeDef*			SPI Interface with LDC1000
 * @param[in] GPIO_TypeDef*					Chip Select Port of LDC1000
 * @param[in] uint32_t						Chip Select Pin of LDC1000
 */
float getInductance   (SPI_HandleTypeDef*, GPIO_TypeDef*, uint32_t);

#endif /* INC_LDC1000_H_ */


