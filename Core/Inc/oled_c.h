

#ifndef OLED_C_H
#define OLED_C_H
#include <stdbool.h>
#include <string.h>
#include "fonts.h"
#include "stm32l4xx_hal.h"


/**
 * @name                 Device Properties
 ******************************************************************************/
///@{
#define OLED_C_SCREEN_WIDTH     (96)
#define OLED_C_SCREEN_HEIGHT    (128)
#define OLED_C_SCREEN_SIZE      ( OLED_C_SCREEN_WIDTH * OLED_C_SCREEN_HEIGHT )
#define OLED_C_ROW_OFF          (0x00)
#define OLED_C_COL_OFF          (0x10)
///@}
/**
 * @name                 GPIOS
 ******************************************************************************/
///@{


#define OLED_RW_Pin GPIO_PIN_0
#define OLED_RW_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_1
#define OLED_RST_GPIO_Port GPIOA
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define OLED_EN_Pin GPIO_PIN_3
#define OLED_EN_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_0
#define OLED_CS_GPIO_Port GPIOB
#define OLED_DC_Pin GPIO_PIN_1
#define OLED_DC_GPIO_Port GPIOB
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_15
#define VCP_RX_GPIO_Port GPIOA
///@}
/**
 * @name                 SSD1355 Commands
 ******************************************************************************/
///@{
#define OLED_C_SET_COL_ADDRESS  (0x15)
#define OLED_C_SET_ROW_ADDRESS  (0x75)
#define OLED_C_WRITE_RAM        (0x5C)
#define OLED_C_READ_RAM         (0x5D)
#define OLED_C_SET_REMAP        (0xA0)
#define OLED_C_SET_START_LINE   (0xA1)
#define OLED_C_SET_OFFSET       (0xA2)
#define OLED_C_MODE_OFF         (0xA4)
#define OLED_C_MODE_ON          (0xA5)
#define OLED_C_MODE_NORMAL      (0xA6)
#define OLED_C_MODE_INVERSE     (0xA7)
#define OLED_C_FUNCTION         (0xAB)
#define OLED_C_SLEEP_ON         (0xAE)
#define OLED_C_SLEEP_OFF        (0xAF)
#define OLED_C_NOP              (0xB0)
#define OLED_C_SET_RESET_PRECH  (0xB1)
#define OLED_C_ENHANCEMENT      (0xB2)
#define OLED_C_CLOCK_DIV        (0xB3)
#define OLED_C_VSL              (0xB4)
#define OLED_C_GPIO             (0xB5)
#define OLED_C_SETSEC_PRECH     (0xB6)
#define OLED_C_GREY_SCALE       (0xB8)
#define OLED_C_LUT              (0xB9)
#define OLED_C_PRECH_VOL        (0xBB)
#define OLED_C_VCOMH            (0xBE)
#define OLED_C_CONTRAST         (0xC1)
#define OLED_C_MASTER_CONTRAST  (0xC7)
#define OLED_C_MUX_RATIO        (0xCA)
#define OLED_C_COMMAND_LOCK     (0xFD)
#define OLED_C_SCROLL_HOR       (0x96)
#define OLED_C_START_MOV        (0x9F)
#define OLED_C_STOP_MOV         (0x9E)
///@}
/**
 * @name                 Font Direction
 ******************************************************************************/
///@{
#define FO_HORIZONTAL           (0)
#define FO_VERTICAL             (1)
#define FO_VERTICAL_COLUMN      (2)
///@}
/**
 * @name                 Types
 ******************************************************************************/
///@{
typedef enum
{
    RMP_INC_HOR         = 0x00,
    RMP_INC_VER         = 0x01,
    RMP_COLOR_NOR       = 0x00,
    RMP_COLOR_REV       = 0x02,
    RMP_SEQ_RGB         = 0x00,
    RMP_SEQ_BGR         = 0x04,
    RMP_SCAN_NOR        = 0x00,
    RMP_SCAN_REV        = 0x10,
    RMP_SPLIT_DISABLE   = 0x00,
    RMP_SPLIT_ENABLE    = 0x20,
    COLOR_65K           = 0x00,
    COLOR_262K          = 0x80

} OLED_C_REMAMP_SET;
typedef struct
{

	GPIO_TypeDef * Port;
	uint16_t Pin;
} GPIO;
typedef struct {
	SPI_HandleTypeDef * hspi;
	GPIO * rst;
	GPIO * cs;
	GPIO * dc;
	GPIO * en;
	GPIO * rw;
} OLED_C;

///@}
/**
 * @name                 Functions
 ******************************************************************************/
///@{
#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief OLED C Strzct Initialization
 * @param[in] OLED_C *   			pointer to struct to initalize
 * @param[in] SPI_HandleTypeDef * 	spi to be used for communication with the display
 *
 * Function initializes the Struct used to communicate with the Display
 * must be called first
 */
void OLED_C_Init_Struct(volatile OLED_C *, SPI_HandleTypeDef *);
/**
 * @brief OLED C Initialization
 * @param[in] OLED_C *   pointer to the OLED_C Struct
 *
 * Function initializes function pointers and perform setup of the SSD1351
 * must be called second
 */
void oled_c_init(volatile OLED_C *);

/**
 * @brief OLED C Hardware Reset
 * @param[in] OLED_C *   pointer to the OLED_C Struct
 *
 * Performs hardware reset of the click board. Init function calling executin
 * of this function.
 */
void oled_c_reset(volatile OLED_C *);

/**
 * @brief OLED C Enable
 * @param[in] OLED_C *   	pointer to the OLED_C Struct
 * @param[in] bool			state ( true ON / false OFF )
 *
 * Enable or disable the display usinf EN pin on the clickboard.
 */
void oled_c_enable(volatile OLED_C *,  bool);

/**
 * @brief OLED C Command
 *
 * @param[in] OLED_C *   	pointer to the OLED_C Struct
 * @param[in] uint8_t   	command, valid command
 * @param[in] uint8_t      	command, pointer to command argument
 * @param[in] uint16_t  	arguments size in bytes
 *
 * Executes provided comman.
 */
void oled_c_command(volatile OLED_C *, uint8_t, uint8_t *, uint16_t);

/**
 * @brief OLED C Fill Screen
 *
 * @param[in] OLED_C *   	pointer to the OLED_C Struct
 * @param[in] color     	RGB color
 *
 * Fills whole screen with provided color.
 */
void oled_c_fill_screen(volatile OLED_C *, uint16_t);


/**
 * @brief OLED C Draw Text
 *
 * @param[in] OLED_C *   	pointer to the OLED_C Struct
 * @param[in] text      	text string
 * @param[in] col_off   	column offset from the left border of the screen
 * @param[in] row_off   	row offset from the top border of the screen
 *
 * Function writes text on the screen.
 */
void oled_c_text(OLED_C *, unsigned char *, uint16_t, uint16_t);
/**
 * @brief OLED C Draw Text
 *
 * @param[in] OLED_C *   	pointer to the OLED_C Struct
 * @param[in] text      	text string
 * @param[in] col_off   	column offset from the left border of the screen
 * @param[in] row_off   	row offset from the top border of the screen
 *
 * Function overwrites the given range with text.
 */
void oled_c_text_overwrite(volatile OLED_C *, volatile unsigned char *, uint16_t, uint16_t, uint16_t);

/**
 * @brief OLED C Font Setup
 *
 * @param[in] font          pointer to font definition
 * @param[in] color         text color
 * @param[in] orientation   text orientation
 *
 * Function setup the parameters needed for @c oled_c_text. Before first usage
 * of @c oled_c_text it is needed to call this function and setup all
 * parameters.
 */
void oled_c_set_font(const uint8_t *, uint16_t, uint8_t);

/**
 * @brief OLED C Font Setup
 *
 * @param[in] OLED_C *      			pointer to OLED_C Struct
 * @param[in] TIM_HandleTypeDef*      	Timer for displayTime
 * @param[in] volatile uint8_t *   		displayTime in ms
 *
 * Function displays a message for a certain timeslot on the display
 */
void displayMessageForTimeSlot(volatile OLED_C *, TIM_HandleTypeDef*, volatile uint8_t *);
/**
 * @brief box_area sets the given area to a color
 *
 * @param[in] OLED_C *      pointer to OLED_C Struct
 * @param[in] uint8_t   	x from
 * @param[in] uint8_t   	y from
 * @param[in] uint8_t   	x to
 * @param[in] uint8_t   	y to
 * @param[in] uint16_t  	color
 */
void box_area(volatile OLED_C *, uint8_t, uint8_t, uint8_t, uint8_t, uint16_t);

#ifdef __cplusplus
} // extern "C"
#endif
#endif
///@}
/**
 * @}                                                           End of File
 ******************************************************************************/
