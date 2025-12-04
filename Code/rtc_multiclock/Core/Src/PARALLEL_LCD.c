/**
 * @file    parallel_lcd.c
 * @author  Jayant Chopra
 * @brief   4-bit parallel interface driver for 16x2 HD44780 LCD
 *
 * This module provides:
 *  - Basic LCD initialization in 4-bit mode
 *  - Command and data write functions
 *  - Cursor positioning and string printing
 *  - Custom character creation (CGRAM)
 *
 * The timing and sequence are adapted from 8051-style LCD routines
 * and tuned for stable operation on STM32F446RE using HAL.
 */

#include "parallel_lcd.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ================== LOCAL DELAY HELPERS ==================
 * These wrappers abstract delays so that the LCD timing
 * matches the original 8051-based implementation.
 * ========================================================= */

// Millisecond delay using HAL (blocking delay)
static void LCD_DelayMs(uint32_t ms) {
    HAL_Delay(ms);
}

// Approximate microsecond delay using NOP loops
static void LCD_DelayUs(uint32_t us) {
    // Convert microseconds to loop iterations based on system clock
    uint32_t cycles = us * (SystemCoreClock / 1000000) / 10;
    while(cycles--) {
        __NOP();
    }
}

/* ================== LOW-LEVEL BIT OPERATIONS ==================
 * The LCD is driven in 4-bit mode. Only D4..D7 lines are used.
 * Data is written as two 4-bit nibbles (high first, then low).
 * ============================================================= */

// Write 4 bits to LCD data lines (D4–D7)
static void LCD_Write4Bits(uint8_t data) {
    HAL_GPIO_WritePin(LCD_D4_PORT, LCD_D4_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D5_PORT, LCD_D5_PIN, (data & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D6_PORT, LCD_D6_PIN, (data & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_D7_PORT, LCD_D7_PIN, (data & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Generate enable pulse to latch data into LCD
static void LCD_PulseEnable(void) {
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_SET);
    LCD_DelayUs(2);   // Enable pulse width
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);
    LCD_DelayUs(100); // Allow LCD to process
}

/* ================== COMMAND AND DATA FUNCTIONS ==================
 * These functions are equivalent to lcd_com and lcd_dat in
 * your original 8051 code, adapted for STM32 HAL.
 * =============================================================== */

// Send command to LCD (RS = 0)
void LCD_Command(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET); // Command mode (RS = 0)

    // Send high nibble
    LCD_Write4Bits(cmd >> 4);
    LCD_PulseEnable();

    // Send low nibble
    LCD_Write4Bits(cmd & 0x0F);
    LCD_PulseEnable();

    // Extra delay for clear and home commands
    if(cmd == LCD_CMD_CLEAR || cmd == LCD_CMD_HOME) {
        LCD_DelayMs(10);   // Clear and home need more time
    } else {
        LCD_DelayUs(100);  // Normal command delay
    }
}

// Send a single character (data) to LCD (RS = 1)
void LCD_WriteChar(char ch) {
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_SET); // Data mode (RS = 1)

    // Send high nibble
    LCD_Write4Bits(ch >> 4);
    LCD_PulseEnable();

    // Send low nibble
    LCD_Write4Bits(ch & 0x0F);
    LCD_PulseEnable();

    LCD_DelayUs(100); // Small delay after data write
}

/* ================== LCD INITIALIZATION ==================
 * Standard 4-bit initialization sequence for HD44780.
 * Based on the datasheet and compatible with your
 * original 8051 initialization flow.
 * ======================================================= */

void LCD_Init(void) {
    HAL_Delay(50); // Wait for LCD power-up (datasheet: >40ms)

    // Ensure control lines are low
    HAL_GPIO_WritePin(LCD_RS_PORT, LCD_RS_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_EN_PORT, LCD_EN_PIN, GPIO_PIN_RESET);

    // Initialization sequence: send 0x3 three times
    LCD_Write4Bits(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);

    LCD_Write4Bits(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);

    LCD_Write4Bits(0x03);
    LCD_PulseEnable();
    HAL_Delay(5);

    // Switch to 4-bit mode
    LCD_Write4Bits(0x02);
    LCD_PulseEnable();
    HAL_Delay(5);

    // Function set: 4-bit, 2 lines, 5x8 font
    LCD_Command(0x28);
    HAL_Delay(5);

    // Display ON, cursor OFF, blink OFF
    LCD_Command(0x0C);
    HAL_Delay(5);

    // Entry mode: increment, no display shift
    LCD_Command(0x06);
    HAL_Delay(5);

    // Clear display and go home
    LCD_Command(0x01);
    HAL_Delay(10);

    LCD_Command(0x80);  // Set cursor to home (line 1, pos 0)
    HAL_Delay(10);
}

/* ================== HIGH-LEVEL HELPERS ================== */

// Clear LCD and wait for completion
void LCD_Clear(void) {
    LCD_Command(0x01); // Clear command
    LCD_DelayMs(10);   // Wait 10ms after clear
}

// Set cursor to given row and column (0-based)
void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address;

    switch(row) {
        case 0:
            address = 0x80 + col; // Line 1 base address
            break;
        case 1:
            address = 0xC0 + col; // Line 2 base address
            break;
        default:
            address = 0x80 + col; // Default to first line
    }

    LCD_Command(address);
}

// Write null-terminated string to LCD
void LCD_WriteString(char *str) {
    while(*str) {
        LCD_WriteChar(*str++);
        LCD_DelayUs(200); // Slight gap between characters
    }
}

// Write string starting at (row, col)
void LCD_WriteStringXY(uint8_t row, uint8_t col, char *str) {
    LCD_SetCursor(row, col);
    LCD_WriteString(str);
}

// Print formatted text (like printf for LCD)
void LCD_Print(char *format, ...) {
    char buffer[32];
    va_list args;

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    LCD_WriteString(buffer);
}

/* ================== CUSTOM CHARACTER SUPPORT ==================
 * Allows defining up to 8 custom 5x8 characters in CGRAM.
 * Used in this project for rocket and flame animation.
 * ============================================================= */

void LCD_CreateChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x07; // Only locations 0–7 are valid
    LCD_Command(0x40 | (location << 3)); // Set CGRAM address

    for (int i = 0; i < 8; i++) {
        LCD_WriteChar(charmap[i]); // Write 8-byte pattern
    }
}
