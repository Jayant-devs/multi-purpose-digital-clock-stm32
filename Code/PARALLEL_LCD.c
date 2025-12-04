#include "parallel_lcd.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

// Simple delay functions matching your 8051 code
static void LCD_DelayMs(uint32_t ms) {
    HAL_Delay(ms);
}

static void LCD_DelayUs(uint32_t us) {
    // Approximate microsecond delay
    uint32_t cycles = us * (SystemCoreClock / 1000000) / 10;
    while(cycles--) {
        __NOP();
    }
}

// Write 4 bits to LCD
static void LCD_Write4Bits(uint8_t data) {
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Pulse Enable pin (exactly like your 8051 code)
static void LCD_PulseEnable(void) {
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_DelayUs(2);  // Increased from 1us to 2us for stability
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_DelayUs(100); // Increased delay
}

// Send command to LCD (like lcd_com in your 8051 code)
void LCD_Command(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET); // Command mode (RS=0)

    // Send high nibble
    LCD_Write4Bits(cmd >> 4);
    LCD_PulseEnable();

    // Send low nibble
    LCD_Write4Bits(cmd & 0x0F);
    LCD_PulseEnable();

    // Extra delay for clear and home commands (like your 8051 code)
    if(cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) {
        LCD_DelayMs(10); // Increased to 10ms like your 8051 code
    } else {
        LCD_DelayUs(100);
    }
}

// Send data to LCD (like lcd_dat in your 8051 code)
void LCD_WriteChar(char ch) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET); // Data mode (RS=1)

    // Send high nibble
    LCD_Write4Bits(ch >> 4);
    LCD_PulseEnable();

    // Send low nibble
    LCD_Write4Bits(ch & 0x0F);
    LCD_PulseEnable();

    LCD_DelayUs(100); // Small delay after data
}

// Initialize LCD with proven timing
void LCD_Init(void) {
    HAL_Delay(50); // Wait for LCD to power up

    // Reset all control lines
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // Step 1: Initialization command 1
    LCD_Write4Bits(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);  // Wait 5ms

    // Step 2: Initialization command 2
    LCD_Write4Bits(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);  // Wait 1ms

    // Step 3: Initialization command 3
    LCD_Write4Bits(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);  // Wait 1ms

    // Step 4: Set to 4-bit mode
    LCD_Write4Bits(0x02);
    LCD_PulseEnable();
    HAL_Delay(5);  // Wait 1ms

    // Now send configuration commands
    LCD_Command(0x28); // 4-bit, 2 lines, 5x8 font
    HAL_Delay(5);

    LCD_Command(0x0C); // Display ON, cursor OFF
    HAL_Delay(5);

    LCD_Command(0x06); // Entry mode: increment
    HAL_Delay(5);

    LCD_Command(0x01); // Clear display
    HAL_Delay(10);     // Extra delay after clear

    LCD_Command(0x80); // Set cursor to home position
    HAL_Delay(10);
}

// Clear LCD (exactly like your 8051)
void LCD_Clear(void) {
    LCD_Command(0x01); // Clear command
    LCD_DelayMs(10);   // Wait 10ms after clear (like your 8051)
}

// Set cursor position
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;

    switch(row) {
        case 0:
            address = 0x80 + col; // Line 1 start
            break;
        case 1:
            address = 0xC0 + col; // Line 2 start
            break;
        default:
            address = 0x80 + col;
    }

    LCD_Command(address);
}

// Write string (exactly like your 8051 approach)
void LCD_WriteString(char *str) {
    while(*str) {
        LCD_WriteChar(*str++);
        LCD_DelayUs(200); // Small delay between characters (like your 8051)
    }
}

// Write string at position
void LCD_WriteStringXY(uint8_t row, uint8_t col, char *str) {
    LCD_SetCursor(row, col);
    LCD_WriteString(str);
}

// Print formatted text
void LCD_Print(char *format, ...) {
    char buffer[32];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    LCD_WriteString(buffer);
}
void LCD_CreateChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x07; // We only have 8 locations 0-7
    LCD_Command(0x40 | (location << 3)); // Set CGRAM Address
    for (int i=0; i<8; i++) {
        LCD_WriteChar(charmap[i]);
    }
}
