#ifndef PARALLEL_LCD_H
#define PARALLEL_LCD_H

#include "main.h"
#include <stdint.h>

// Pin definitions using CubeMX auto-generated labels
#define LCD_RS_PIN      LCD_RS_Pin
#define LCD_RS_PORT     LCD_RS_GPIO_Port

#define LCD_EN_PIN      LCD_EN_Pin
#define LCD_EN_PORT     LCD_EN_GPIO_Port

#define LCD_D4_PIN      LCD_D4_Pin
#define LCD_D4_PORT     LCD_D4_GPIO_Port

#define LCD_D5_PIN      LCD_D5_Pin
#define LCD_D5_PORT     LCD_D5_GPIO_Port

#define LCD_D6_PIN      LCD_D6_Pin
#define LCD_D6_PORT     LCD_D6_GPIO_Port

#define LCD_D7_PIN      LCD_D7_Pin
#define LCD_D7_PORT     LCD_D7_GPIO_Port
// Button pin definitions using CubeMX auto-generated labels
#define MODE_BUTTON_PIN      mode_pin_Pin
#define MODE_BUTTON_PORT     mode_pin_GPIO_Port

#define START_STOP_PIN       start_stop_pin_Pin
#define START_STOP_PORT      start_stop_pin_GPIO_Port

#define RESET_PIN            reset_pin_Pin
#define RESET_PORT           reset_pin_GPIO_Port
// Complete LCD Command Codes (from your research)
#define LCD_CMD_CLEAR           0x01    // Clear display
#define LCD_CMD_HOME            0x02    // Return home
#define LCD_CMD_ENTRY_MODE      0x04    // Entry mode set
#define LCD_DISPLAY_CTRL        0x08    // Display control
#define LCD_CMD_CURSOR_SHIFT    0x10    // Cursor/display shift
#define LCD_CMD_FUNCTION_SET    0x20    // Function set
#define LCD_CMD_SET_CGRAM       0x40    // Set CGRAM address
#define LCD_CMD_SET_DDRAM       0x80    // Set DDRAM address

// Entry mode flags
#define LCD_ENTRY_LEFT          0x02
#define LCD_ENTRY_RIGHT         0x00
#define LCD_ENTRY_SHIFT_ON      0x01
#define LCD_ENTRY_SHIFT_OFF     0x00

// Display control flags
#define LCD_DISPLAY_ON          0x04
#define LCD_DISPLAY_OFF         0x00
#define LCD_CURSOR_ON           0x02
#define LCD_CURSOR_OFF          0x00
#define LCD_BLINK_ON            0x01
#define LCD_BLINK_OFF           0x00

// Cursor/display shift flags
#define LCD_SHIFT_DISPLAY       0x08
#define LCD_SHIFT_CURSOR        0x00
#define LCD_SHIFT_RIGHT         0x04
#define LCD_SHIFT_LEFT          0x00

// Function set flags
#define LCD_8BIT_MODE           0x10
#define LCD_4BIT_MODE           0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10_DOTS           0x04
#define LCD_5x8_DOTS            0x00

// Common combined commands (from your table)
#define LCD_DEC_CURSOR          0x04    // Decrement cursor (shift left)
#define LCD_INC_CURSOR          0x06    // Increment cursor (shift right)
#define LCD_SHIFT_DISPLAY_R     0x05    // Shift display right
#define LCD_SHIFT_DISPLAY_L     0x07    // Shift display left
#define LCD_DISP_OFF_CURS_OFF   0x08    // Display off, cursor off
#define LCD_DISP_OFF_CURS_ON    0x0A    // Display off, cursor on
#define LCD_DISP_ON_CURS_OFF    0x0C    // Display on, cursor off
#define LCD_DISP_ON_CURS_BLINK  0x0F    // Display on, cursor blinking
#define LCD_SHIFT_CURSOR_L      0x10    // Shift cursor left
#define LCD_SHIFT_CURSOR_R      0x14    // Shift cursor right
#define LCD_SHIFT_DISP_L        0x18    // Shift entire display left
#define LCD_SHIFT_DISP_R        0x1C    // Shift entire display right
#define LCD_LINE1_START         0x80    // Force cursor to line 1 start
#define LCD_LINE2_START         0xC0    // Force cursor to line 2 start
#define LCD_2LINE_5x7           0x38    // 2 lines, 5x7 matrix

// Function Prototypes
void LCD_Init(void);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

// Display Control Functions
void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_CursorOn(void);
void LCD_CursorOff(void);
void LCD_BlinkOn(void);
void LCD_BlinkOff(void);

// Cursor and Display Shift Functions
void LCD_ShiftLeft(void);
void LCD_ShiftRight(void);
void LCD_ShiftDisplayLeft(void);
void LCD_ShiftDisplayRight(void);

// Data Writing Functions
void LCD_WriteChar(char ch);
void LCD_WriteString(char *str);
void LCD_WriteStringXY(uint8_t row, uint8_t col, char *str);
void LCD_Print(char *format, ...);

// Custom Character Functions
void LCD_CreateChar(uint8_t location, uint8_t charmap[]);
void LCD_WriteCustomChar(uint8_t location);

#endif
