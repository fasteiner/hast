# Project HAST
<h2>Initialization</h2>
Requirements:
<ul>
    <li>D7 connected to A1 with a wire (Reset of Display)</li>
    <li>Display (MikroElektronika oled_c v101, SSD1351) in Slot 1 of Click Board</li>
    <li>LDC1000 in Slot 2 of Click Board</li>
    <li>USART Initalized</li>
    <li>USART DMA Channels configured and Inizalized</li>
    <li>TIM6, TIM7 Initalized</li>
    <li>LDC_CS_GPIO_Port --> LOW</li>
    <li>OLED_CS_GPIO_Port --> LOW</li>
</ul>
<h3>Initialization of Modules</h3>
<h4>LDC1000</h4>

```c
  applicationInit (&hspi1, LDC_CS_GPIO_Port, LDC_CS_Pin);
  enableInterrupt (&hspi1, LDC_CS_GPIO_Port, LDC_CS_Pin, 5500, 8500);
```
<h4>OLED_C v101</h4>

```c
  oled_c = (OLED_C *)malloc(sizeof(OLED_C));
  OLED_C_Init_Struct(oled_c, &hspi1);
  oled_c_init(oled_c);
  oled_c_set_font(guiFont_Tahoma_10_Regular, WHITE, 0 );
```

<h4>Gamer</h4>

```c
  prioOnly = false;
  isSoftwareInt = false;
  DISPLAY_TIME = 30000;
  messageToDisplay = (uint8_t *)malloc((10 + MAX_PAYLOAD_LENGTH) * sizeof(uint8_t));
  currTime = (RTC_TimeTypeDef*)malloc(sizeof(RTC_TimeTypeDef));
  currDate = (RTC_DateTypeDef*)malloc(sizeof(RTC_DateTypeDef));
  initGAMER(&huart2, messageToDisplay);
  HAL_TIM_Base_Start_IT(&htim6);

  // Print Coins after startup
  printCoins(messageToDisplay);

  // Send menu on startup via UART
  printMenu(&huart2);
```
<h2>Usage of Modules</h2>
<h3>Gamer Protokoll</h3>

```
-----------------------------------------
-------Welcome to our MCSD Project-------
-----------------------------------------
 /$$   /$$  /$$$$$$   /$$$$$$  /$$$$$$$$
| $$  | $$ /$$__  $$ /$$__  $$|__  $$__/
| $$  | $$| $$  \ $$| $$  \__/   | $$
| $$$$$$$$| $$$$$$$$|  $$$$$$    | $$
| $$__  $$| $$__  $$ \____  $$   | $$
| $$  | $$| $$  | $$ /$$  \ $$   | $$
| $$  | $$| $$  | $$|  $$$$$$/   | $$
|__/  |__/|__/  |__/ \______/    |__/
-----------------------------------------
Your options are:
#g   Receive the current amount of coins
#a   Send an Answer
#m   Send a message
#e   Send an error
#r   Reset amount of coins
#d   Display number of coins on Display
#s...
#sc  Set amount of coins
#st  Set time and date YYYY-MM-DD-hh:mm:ss
#sd  Set delay time that messages
     are displayed between 25 and 60
#?   Show this menu again."
-----------------------------------------
```
If a command with invalid content is send, then the gamer protocol sends an error and the needed format.

<h3>Application</h3>

```
prioOnly: flag that indicates that only messages with prio can be displayed
isSoftwareInt: flag that needs to be set to ensure the Software Interrupt is only triggered on purpose
```
<h2>Feature Description</h2>
A coin counter with a display of inserted coins. The display shows the ontime. 

<h3>After Power-on:</h3>
The Display is initialized to the following values:<br>
<b>Coin Counter:</b> 0<br>
<b>On Time:</b> 00-00-00:00:00<br>

<h3>Usecase 1:</h3>

A coin is inserted or the display Coin command `#d\0` is sent via UART so the coin count is displayed for 30 seconds (default) or for a specified time between 25 and 60 seconds.

![Usecase 1 example](/Dokumente/images/usecase1.jpg)

The displayed format is `MM-DD-hh:mm:ss`. As the RTC uses a calendar internally, the Month starts with 1 and ends with 12.


<h3>Usecase 2:</h3>

A message is recieved over UART and displayed on the display for the configured timeinterval. If the message is longer then the display, it scrolls in 1px steps every `delayT` ms.
```c
uint16_t delayT = (uint16_t)((DISPLAY_TIME - (SCROLL_DELAY + 2000)) / 128);
```
<b>Fits display:</b>

![Usecase 2 example](/Dokumente/images/usecase1.jpg)

<b>Does not fit display:</b>

![Usecase 2 example](/Dokumente/images/usecase2.1.jpg)

Linebreaks are added automatically if a word is to long.

Diplaytime: configurable over UART using `#sdxx\0`, where xx is in seconds and has to be between 25 and 60.

<h3>Usecase 3:</h3>
When nothing happens for the configured timeinterval (default 30s), then only the ontime is displayed.

![Usecase 3 example](/Dokumente/images/usecase3.jpg)

<h3>Usecase 4:</h3>
A coin is inserted while a message is being displayed. The coin is counted, but not displayed, because the display is blocked by a priority message.

<h3>Usecase 5:</h3>
<table>
  <thead>
    <tr>
        <th>Description</th>
        <th>char</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Beginn of Message</td>
      <td>#</td>
    </tr>
    <tr>
      <td>Get Number of coins</td>
      <td>g</td>
    </tr>
    <tr>
      <td>Answer to message</td>
      <td>a</td>
    </tr>
    <tr>
      <td>Post message</td>
      <td>m</td>
    </tr>
    <tr>
      <td>Error message</td>
      <td>e</td>
    </tr>
     <tr>
      <td>Reset coin counter</td>
      <td>r</td>
    </tr>
    <tr>
      <td>Set coins</td>
      <td>sc</td>
    </tr>
    <tr>
      <td>Set display delay</td>
      <td>sd</td>
    </tr>
    <tr>
      <td>Set time</td>
      <td>st</td>
    </tr>
    <tr>
      <td>Display coins</td>
      <td>d</td>
    </tr>
    <tr>
      <td>Display help message</td>
      <td>?</td>
    </tr>
     <tr>
      <td>End of message</td>
      <td>\0</td>
    </tr>
  </tbody>
</table>

<b>Example messages:</b>
<table>
  <thead>
    <tr><th>Description</th><th>message</th></tr>
  </thead>
  <tbody>
     <tr><td>Answer message</td><td>#aPAYLOAD\0</td></tr>
     <tr><td>Post message</td><td>#mLorem Ipsum dolor sit amet\0</td></tr>
     <tr><td>Get message</td><td>#g\0</td></tr>
     <tr><td>Error message</td><td>#eERRORMESSAGE\0</td></tr>
     <tr><td>Reset message</td><td>#r\0</td></tr>
     <tr><td>Display coins message</td><td>#d\0</td></tr>
     <tr><td>Set coins message</td><td>#sc1000\0</td></tr>
     <tr><td>Set display delay message</td><td>#sd59\0</td></tr>
     <tr><td>Set time message</td><td>#st2022-02-04-10:30:50\0</td></tr>
     <tr><td>Help message</td><td>#?\0</td></tr>
  </tbody>
</table>
<b>Maximal Payload length:</b> 110<br>
<b>Exceeding payload length:</b> error<br>
<b>Violation of the protocol:</b> error<br>

<h3>Usecase 6:</h3>
A answer message is recieved by the microcontroller. This displays the message answer: 'message' for the configured time interval.

![Usecase 6 example](/Dokumente/images/usecase6.jpg)
