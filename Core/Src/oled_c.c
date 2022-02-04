#include "oled_c.h"
#include "main.h"
#include "gamer.h"

/*                 Preprocessors
 ******************************************************************************/

#define OLED_C_IMG_HEAD     (6)
#define OLED_C_IMG_WIDE(x)  ( (x)[2] )
#define OLED_C_IMG_HIGH(x)  ( (x)[4] )

/*                Variables
 ******************************************************************************/




static const uint8_t*   _font;
static uint16_t         _font_color;
static uint8_t          _font_orientation;
static uint16_t         _font_first_char;
static uint16_t         _font_last_char;
static uint16_t         _font_height;
static uint16_t         x_cord;
static uint16_t         y_cord;

static uint8_t DEFAULT_MUX_RATIO      = 95;                                     // FIXED
static uint8_t DEFAULT_START_LINE     = 0x80;                                   // FIXED
static uint8_t DEFAULT_OFFSET         = 0x20;                                   // FIXED
static uint8_t DEFAULT_REMAP          = RMP_INC_HOR |
                                        RMP_COLOR_REV |
                                        RMP_SEQ_RGB |
                                        RMP_SCAN_REV |
                                        RMP_SPLIT_ENABLE |
                                        COLOR_65K;

static uint8_t DEFAULT_OLED_LOCK      = 0x12;
static uint8_t DEFAULT_CMD_LOCK       = 0xB1;
static uint8_t DEFAULT_DIVSET         = 0xF0;
static uint8_t DEFAULT_PRECHARGE      = 0x32;
static uint8_t DEFAULT_VCOMH          = 0x05;
static uint8_t DEFAULT_MASTER_CONT    = 0xCF;
static uint8_t DEFAULT_PRECHARGE_2    = 0x01;
static uint8_t DEFAULT_VSL[ 3 ]       = { 0xA0, 0xB5, 0x55 };
static uint8_t DEFAULT_CONTRAST[ 3 ]  = { 0x8A, 0x51, 0x8A };

static uint8_t cols[ 2 ]    = { OLED_C_COL_OFF, OLED_C_COL_OFF + 95 };          // INITIAL
static uint8_t rows[ 2 ]    = { OLED_C_ROW_OFF, OLED_C_ROW_OFF + 95 };          // INITIAL


/*                 Private Function Prototypes
 ******************************************************************************/
/**
 * @brief SPI_Write
 *
 * @param[in] SPI_HandleTypeDef*  pointer to SPI to be used
 * @param[in] uint8_t *    pointer to the data
 * @param[in] uint8_t      length of the data
 *
 * writes data over SPI and sends it to the display
 */
void SPI_Write(SPI_HandleTypeDef*, uint8_t *, uint8_t);

/**
 * @brief pixel - sets one pixel
 *
 * @param[in] OLED_C *      pointer to OLED_C Struct
 * @param[in] uint8_t 		row
 * @param[in] uint8_t 		col
 * @param[in] uint16_t 		color
 */
static void pixel(volatile OLED_C *,  uint8_t, uint8_t, uint16_t );

/**
 * @brief character - writes one character to the display
 *
 * @param[in] OLED_C *      pointer to OLED_C Struct
 * @param[in] uint16_t 		character
 * @param[in] bool 			override, if true the screen is overwritten in the background
 */
static void character(volatile OLED_C *, volatile uint16_t , bool);

/**
 * @brief Set_Start_Line - sets start line
 *
 * @param[in] OLED_C *      	pointer to OLED_C Struct
 * @param[in] unsigned char 	line
 */
void Set_Start_Line(volatile OLED_C * , unsigned char );

/**
 * @brief Set_Display_Offset - sets the offset from the startline
 *
 * @param[in] OLED_C *      	pointer to OLED_C Struct
 * @param[in] unsigned char 	offset
 */
void Set_Display_Offset(OLED_C * oled_c, unsigned char data);

/**
 * @brief Vertical_Scroll - Scroll vertically
 *
 * @param[in] OLED_C *      	pointer to OLED_C Struct
 * @param[in] unsigned int 	direction
 * @param[in] unsigned int		rows
 */
void Vertical_Scroll(volatile OLED_C * , unsigned int, unsigned int);


/*                 Private Function Definitions
 ******************************************************************************/
void SPI_Write(SPI_HandleTypeDef* hspi, uint8_t * data, uint8_t len){
	if(HAL_SPI_Transmit(hspi, data, sizeof(uint8_t)*len, HAL_MAX_DELAY) != HAL_OK)
	{
		uint8_t errorMessage[] = "SPI Busy";
		sendErrorMessage(&huart2 ,(uint8_t *)&errorMessage);
		return;
	}
	//ensures the SPI is not busy after before the function quits
	while(HAL_SPI_GetState(hspi) != HAL_SPI_STATE_READY){
	}
}

//Initalizes the struct which is used for SPI communication
void OLED_C_Init_Struct(volatile OLED_C * oled_c,SPI_HandleTypeDef * hspi){
	//SPI Interface
	oled_c -> hspi = hspi;
	//Reset PIN of the click module
	oled_c ->rst  = (GPIO *)malloc(sizeof(GPIO));
	oled_c ->rst  -> Pin = OLED_RST_Pin;
	oled_c ->rst  -> Port = OLED_RST_GPIO_Port;
	//chip select PIN of the click module
	oled_c -> cs= (GPIO *)malloc(sizeof(GPIO));
	oled_c -> cs -> Pin = OLED_CS_Pin;
	oled_c -> cs -> Port = OLED_CS_GPIO_Port;
	//Data or command PIN (C=0, D=1)
	oled_c -> dc = (GPIO *)malloc(sizeof(GPIO));
	oled_c -> dc -> Pin = OLED_DC_Pin;
	oled_c -> dc -> Port = OLED_DC_GPIO_Port;
	//read or write PIN (write = 0)
	oled_c -> rw = (GPIO *)malloc(sizeof(GPIO));
	oled_c -> rw  -> Pin = OLED_RW_Pin;
	oled_c -> rw  -> Port = OLED_RW_GPIO_Port;
	//enable pin of the click module, is needed so the display is powered
	oled_c -> en = (GPIO *)malloc(sizeof(GPIO));
	oled_c -> en -> Pin = OLED_EN_Pin;
	oled_c -> en -> Port = OLED_EN_GPIO_Port;
}

//sets one pixel
static void pixel(volatile OLED_C * oled_c, uint8_t col, uint8_t row, uint16_t color )
{
    uint8_t     cmd         = OLED_C_WRITE_RAM;
    uint8_t     clr[ 2 ]    = { 0 };

    if( ( col > OLED_C_SCREEN_WIDTH ) || ( row > OLED_C_SCREEN_HEIGHT ) )
        return;

    cols[ 0 ] = OLED_C_COL_OFF + col;
    cols[ 1 ] = OLED_C_COL_OFF + col;
    rows[ 0 ] = OLED_C_ROW_OFF + row;
    rows[ 1 ] = OLED_C_ROW_OFF + row;
    clr[ 0 ] |= color >> 8;
    clr[ 1 ] |= color & 0x00FF;

    oled_c_command(oled_c, OLED_C_SET_COL_ADDRESS, cols, 2 );
    oled_c_command(oled_c, OLED_C_SET_ROW_ADDRESS, rows, 2 );
    HAL_GPIO_WritePin(oled_c->cs->Port, oled_c->cs->Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(oled_c->dc->Port, oled_c->dc->Pin, GPIO_PIN_RESET);
    SPI_Write(oled_c->hspi,  &cmd, 1 );
    HAL_GPIO_WritePin(oled_c->dc->Port, oled_c->dc->Pin, GPIO_PIN_SET);
    SPI_Write(oled_c->hspi,  clr, 2 );
    HAL_GPIO_WritePin(oled_c->cs->Port, oled_c->cs->Pin, GPIO_PIN_SET);
}

//writes one character
static void character(volatile OLED_C * oled_c, volatile uint16_t ch, bool override)
{
    uint8_t     ch_width = 0;
    uint8_t     x_cnt;
    uint8_t     y_cnt;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint16_t    tmp;
    uint8_t     temp = 0;
    uint8_t     mask = 0;
    uint32_t    offset;
    const uint8_t *ch_table;
    const uint8_t *ch_bitmap;

    //Line Feed / new line
    if(ch == 10){
    	box_area(oled_c, x_cord, y_cord, OLED_C_SCREEN_WIDTH, y_cord + _font_height, 0 );
        x_cord= 0;
        y_cord = y_cord + (uint16_t)(_font_height * 1.2);
      }
    //not defineed in font
    if( ch < _font_first_char )
        return;
    if( ch > _font_last_char )
        return;

    offset = 0;
    tmp = (ch - _font_first_char) << 2;
    ch_table = _font + 8 + tmp;
    ch_width = *ch_table;

    //line over
    if(x_cord + ch_width >= 95 || ch == 10){
    	x_cord= 0;
    	y_cord = y_cord + (uint16_t)(_font_height * 1.2);
    }
    //override the display where the char will be placed
    if(override){
       	box_area(oled_c, x_cord, y_cord, x_cord+ ch_width*1.2, y_cord + _font_height*1.2, 0 );
       }


    offset = (uint32_t)ch_table[1] + ((uint32_t)ch_table[2] << 8) + ((uint32_t)ch_table[3] << 16);

    ch_bitmap = _font + offset;

    if( ( _font_orientation == FO_HORIZONTAL ) ||
        ( _font_orientation == FO_VERTICAL_COLUMN ) )
    {
        y = y_cord;
        for (y_cnt = 0; y_cnt < _font_height; y_cnt++)
        {
            x = x_cord;
            mask = 0;
            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( !mask )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }

                if( temp & mask )
                    pixel(oled_c, x, y, _font_color );

                x++;
                mask <<= 1;
            }
            y++;
        }

        if ( _font_orientation == FO_HORIZONTAL )
            x_cord = x + 1;
        else
            y_cord = y;
    }
    else
    {
        y = x_cord;

        for( y_cnt = 0; y_cnt < _font_height; y_cnt++ )
        {
            x = y_cord;
            mask = 0;

            for( x_cnt = 0; x_cnt < ch_width; x_cnt++ )
            {
                if( mask == 0 )
                {
                    temp = *ch_bitmap++;
                    mask = 0x01;
                }

                if( temp & mask )
                {
                    pixel(oled_c, y, x, _font_color );
                }

                x--;
                mask <<= 1;
            }
            y++;
        }
        y_cord = x - 1;
    }
}

//sets the section to a specific color value
void box_area(volatile OLED_C * oled_c, uint8_t start_col, uint8_t start_row,
                      uint8_t end_col, uint8_t end_row,
                      uint16_t color )
{
    uint8_t     cmd         = OLED_C_WRITE_RAM;
    uint16_t    cnt         = ( end_col - start_col ) * ( end_row - start_row );
    uint8_t     clr[ 2 ]    = { 0 };

    if( ( start_col > OLED_C_SCREEN_WIDTH ) ||
        ( end_col > OLED_C_SCREEN_WIDTH ) )
        return;

    if( ( start_row > OLED_C_SCREEN_HEIGHT ) ||
        ( end_row > OLED_C_SCREEN_HEIGHT ) )
        return;

    if( ( end_col < start_col ) ||
        ( end_row < start_row ) )
        return;

    cols[ 0 ] = OLED_C_COL_OFF + start_col;
    cols[ 1 ] = OLED_C_COL_OFF + end_col - 1;
    rows[ 0 ] = OLED_C_ROW_OFF + start_row;
    rows[ 1 ] = OLED_C_ROW_OFF + end_row - 1;
    clr[ 0 ] |= color >> 8;
    clr[ 1 ] |= color & 0x00FF;

    oled_c_command(oled_c, OLED_C_SET_COL_ADDRESS, cols, 2 );
    oled_c_command(oled_c,  OLED_C_SET_ROW_ADDRESS, rows, 2 );
    HAL_GPIO_WritePin(oled_c->cs->Port, oled_c->cs->Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(oled_c->dc->Port, oled_c->dc->Pin, GPIO_PIN_RESET);
    SPI_Write(oled_c->hspi,  &cmd, 1 );
    HAL_GPIO_WritePin(oled_c->dc->Port, oled_c->dc->Pin, GPIO_PIN_SET);
    while( cnt-- )
        SPI_Write(oled_c->hspi,  clr, 2 );
    HAL_GPIO_WritePin(oled_c->cs->Port, oled_c->cs->Pin, GPIO_PIN_SET);
}


/*                Public Function Definitions
 ******************************************************************************/

//initzialise the display according to the datasheet
void oled_c_init(volatile OLED_C * oled_c)
{
	//set the prescaler to the highest baud rate
	oled_c -> hspi -> Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	if (HAL_SPI_Init(oled_c->hspi) != HAL_OK) {
		uint8_t errorMessage[] = "SPI Init for Display failed";
		sendErrorMessage(&huart2, (uint8_t*)&errorMessage);
	}
	HAL_Delay( 1 );
	//activate writing
	HAL_GPIO_WritePin(oled_c->rw->Port, oled_c->rw->Pin, GPIO_PIN_RESET);
	oled_c_reset(oled_c);
	oled_c_enable(oled_c, true );
	oled_c_command(oled_c,  OLED_C_SLEEP_OFF,       NULL,                   0 );
	HAL_Delay(400);

    // Unlock display and turn off
    oled_c_command(oled_c,  OLED_C_COMMAND_LOCK,    &DEFAULT_OLED_LOCK,     1 );
    oled_c_command(oled_c,  OLED_C_COMMAND_LOCK,    &DEFAULT_CMD_LOCK,      1 );
    oled_c_command(oled_c,  OLED_C_SLEEP_ON,        NULL,                   0 );

    // Setup SSD1351
    oled_c_command(oled_c,  OLED_C_SET_REMAP,       &DEFAULT_REMAP,         1 );
    oled_c_command(oled_c,  OLED_C_MUX_RATIO,       &DEFAULT_MUX_RATIO,     1 );
    oled_c_command(oled_c,  OLED_C_SET_START_LINE,  &DEFAULT_START_LINE,    1 );
    oled_c_command(oled_c,  OLED_C_SET_OFFSET,      &DEFAULT_OFFSET,        1 );
    oled_c_command(oled_c,  OLED_C_VCOMH,           &DEFAULT_VCOMH,         1 );
    oled_c_command(oled_c,  OLED_C_CLOCK_DIV,       &DEFAULT_DIVSET,        1 );
    oled_c_command(oled_c,  OLED_C_SET_RESET_PRECH, &DEFAULT_PRECHARGE,     1 );
    oled_c_command(oled_c,  OLED_C_SETSEC_PRECH,    &DEFAULT_PRECHARGE_2,   1 );
    oled_c_command(oled_c,  OLED_C_MASTER_CONTRAST, &DEFAULT_MASTER_CONT,   1 );
    oled_c_command(oled_c,  OLED_C_CONTRAST,        DEFAULT_CONTRAST,       3 );
    oled_c_command(oled_c,  OLED_C_VSL,             DEFAULT_VSL,            3 );


    // Set normal mode and turn on display
    oled_c_command(oled_c,  OLED_C_MODE_NORMAL,     NULL,                   0 );
    oled_c_command(oled_c,  OLED_C_SLEEP_OFF,       NULL,                   0 );

    //set screen to black
    oled_c_fill_screen(oled_c, 0 );
}
//reset the display according to the datasheet
void oled_c_reset( volatile OLED_C * oled_c )
{
    HAL_GPIO_WritePin(oled_c->rst->Port, oled_c->rst->Pin, GPIO_PIN_RESET);
    HAL_Delay( 1 );
    HAL_GPIO_WritePin(oled_c->rst->Port, oled_c->rst->Pin, GPIO_PIN_SET);
    HAL_Delay( 1 );
}
//enable the display according to the datasheet
void oled_c_enable(volatile OLED_C * oled_c,  bool state )
{
    if( state ){
    	HAL_GPIO_WritePin(oled_c->en->Port, oled_c->en->Pin, GPIO_PIN_SET);
    	HAL_Delay(10);
    }
    else
    	HAL_GPIO_WritePin(oled_c->en->Port, oled_c->en->Pin, GPIO_PIN_RESET);
}
//send a command to the OLED_C CLick
void oled_c_command(volatile OLED_C * oled_c,  uint8_t command, uint8_t *args, uint16_t args_len )
{
    HAL_GPIO_WritePin(oled_c->cs->Port, oled_c->cs->Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(oled_c->dc->Port, oled_c->dc->Pin, GPIO_PIN_RESET);
    SPI_Write(oled_c->hspi,  &command, 1 );
    //comand only, no data
    if(args_len != 0){
		HAL_GPIO_WritePin(oled_c->dc->Port, oled_c->dc->Pin, GPIO_PIN_SET);
		SPI_Write(oled_c->hspi,  args, args_len );
    }
    HAL_GPIO_WritePin(oled_c->cs->Port, oled_c->cs->Pin, GPIO_PIN_SET);
}
//sets the whole screen to one color
void oled_c_fill_screen(volatile OLED_C * oled_c, uint16_t color )
{
    box_area(oled_c, 0, 0, OLED_C_SCREEN_WIDTH, OLED_C_SCREEN_HEIGHT, color );
}

//writes a text on the screen whiout resetting the background
void oled_c_text(OLED_C * oled_c, unsigned char *text, uint16_t x, uint16_t y )
{
    uint8_t *ptr = text;

    if( ( x >= OLED_C_SCREEN_WIDTH ) || (y >= OLED_C_SCREEN_HEIGHT ) )
        return;

    x_cord = x;
    y_cord = y;

    while( *ptr ){
    	character(oled_c, *ptr++, false);
    }
    if (strlen((char*) text) >= 66){
    	 Vertical_Scroll(oled_c, 0x00, 1);
       }

}
//writes a text on the screen and resets the background
void oled_c_text_overwrite(volatile OLED_C * oled_c, volatile unsigned char *text, uint16_t x, uint16_t y, uint16_t y_end)
{
	volatile uint8_t *ptr = text;
	if ((x >= OLED_C_SCREEN_WIDTH) || (y >= OLED_C_SCREEN_HEIGHT))
		return;

	x_cord = x;
	y_cord = y;
	//box_area(oled_c, x_cord, y_cord, x_end, y_end, 0 );
	while (*ptr) {
		character(oled_c, *ptr++, true);
	}
	uint8_t len = strlen((char*) text);
	//to clear DisplayMessage
	memset((uint8_t *)messageToDisplay, 0, MAX_MESSAGE_LENGTH);
	box_area(oled_c, x_cord, y_cord, OLED_C_SCREEN_WIDTH, y_cord - _font_height, 0 );
	box_area(oled_c, 0, y_cord + _font_height, OLED_C_SCREEN_WIDTH, y_end, 0 );
	if (len >= 66) {
		Vertical_Scroll(oled_c, 0x00, 1);
		oled_c_fill_screen(oled_c, 0);
	}

}
//displays a message for a given Timeslot
void displayMessageForTimeSlot(volatile OLED_C * oled_c, TIM_HandleTypeDef* htim, volatile uint8_t * message){
	if(htim ->State == HAL_TIM_STATE_BUSY){
		__HAL_TIM_SET_COUNTER(htim, 0);
	}
	if (htim ->State == HAL_TIM_STATE_READY){
		htim->Init.Period = DISPLAY_TIME-1;
		HAL_TIM_Base_Init(htim);
		HAL_TIM_Base_Start_IT(htim);
	}
	if(prioOnly){
		box_area(oled_c, x_cord, y_cord, OLED_C_SCREEN_WIDTH, OLED_C_SCREEN_HEIGHT, 0 );
	}
	if(strlen((char*)message) >= 66)
		oled_c_text_overwrite(oled_c, message, 0, 0, OLED_C_SCREEN_HEIGHT);
	else
		oled_c_text_overwrite(oled_c, message, 0, 20, OLED_C_SCREEN_HEIGHT);
}
//sets the font of the display
void oled_c_set_font( const uint8_t *font, uint16_t color, uint8_t orientation )
{
    _font               = font;
    _font_first_char    = font[2] + (font[3] << 8);
    _font_last_char     = font[4] + (font[5] << 8);
    _font_height        = font[6];
    _font_color         = color;
    _font_orientation   = orientation ;
}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Vertical Scrolling (Full Screen)
//
// direction: Scrolling Direction
// "0x00" (Upward)
// "0x01" (Downward)
// rows: Set Numbers of Row Scroll per Step
// t: Set Time Interval between Each Scroll Step (t*200 ms)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Vertical_Scroll(volatile OLED_C * oled_c, unsigned int direction, unsigned int rows)
{
HAL_Delay(SCROLL_DELAY);
uint16_t delayT = (uint16_t)((DISPLAY_TIME - (SCROLL_DELAY + 2000)) / 128);
switch(direction)
{
case 0:
	for(unsigned int i=0;i<128;i+=rows)
	{
		Set_Start_Line(oled_c, i);
		HAL_Delay(delayT);
	}
	break;
case 1:
	for(unsigned int i=0;i<128;i+=rows)
	{
		Set_Start_Line(oled_c, 128-i);
		HAL_Delay(delayT);
	}
	break;
}
Set_Start_Line(oled_c, 0x00);
}

void Set_Start_Line(volatile OLED_C * oled_c, unsigned char data)
{
	oled_c_command(oled_c,  OLED_C_SET_START_LINE, (uint8_t *)&data, 1 );

}
void Set_Display_Offset(OLED_C * oled_c, unsigned char data)
{
	oled_c_command(oled_c,  OLED_C_SET_OFFSET, (uint8_t *)&data, 1 );
}



/*                                                              End of File
 ******************************************************************************/
