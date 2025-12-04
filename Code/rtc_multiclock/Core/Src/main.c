/* USER CODE BEGIN Header */
/**
 * * @brief Multi-Purpose Digital Clock using STM32F446RE
 *
 * Features:
 *  - RTC based 24-hour digital clock with battery backup
 *  - Stopwatch using system tick (HAL_GetTick)
 *  - Interactive time-setting mode using push buttons
 *  - 16x2 LCD display (4-bit parallel mode)
 *  - Startup animation using LCD custom characters
 *
 * Software Design:
 *  - Implemented using a finite state machine (FSM)
 *  - Modes: CLOCK → STOPWATCH → SETTINGS
 *  - Modular driver-based structure for LCD handling
 *
 * Author: Jayant Chopra (24UEC114)
 * Department: ECE, LNMIIT
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "parallel_lcd.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* ================= GLOBAL APPLICATION VARIABLES =================
 * RTC structures store time and date maintained by the STM32 RTC
 * peripheral, allowing time persistence across resets.
 *
 * Stopwatch variables rely on system tick counting to provide
 * high-resolution timing independent of the RTC.
 * ================================================================= */

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */

// Current real-time clock values fetched from hardware RTC
RTC_TimeTypeDef currentTime;
RTC_DateTypeDef currentDate;

// Stopwatch timing variables
uint32_t stopwatchStartTime = 0; // Start timestamp in milliseconds
uint32_t stopwatchElapsed = 0;   // Elapsed time in milliseconds
uint8_t stopwatchRunning = 0;    // Stopwatch state flag
/**
 * @brief Application display modes (Finite State Machine)
 *
 * MODE_CLOCK     : Normal time display using RTC
 * MODE_STOPWATCH : Stopwatch operation mode
 * MODE_SETTINGS  : User configuration for time setting
 */

typedef enum {
    MODE_CLOCK,
    MODE_STOPWATCH,
    MODE_SETTINGS
} DisplayMode_t;
/**
 * @brief Time setting sub-modes
 *
 * SET_HOURS   : Adjust hours field
 * SET_MINUTES : Adjust minutes field
 * SET_SAVE    : Commit time to RTC and exit settings
 */

typedef enum {
    SET_HOURS,
    SET_MINUTES,
    SET_SAVE
} SettingMode_t;

DisplayMode_t currentMode = MODE_CLOCK;
SettingMode_t settingMode = SET_HOURS;
uint8_t settingBlink = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);

/* ================= FUNCTION PROTOTYPES =================
 * Display functions handle LCD visualization for each mode.
 * Button handlers implement debounced user interaction.
 * ====================================================== */

/* USER CODE BEGIN PFP */
void displayClock(void);
void displayStopwatch(void);
void displaySettings(void);
void handleStopwatch(void);
void updateDisplay(void);
void handleButtons(void);
void handleModeButton(void);
void handleStopwatchButtons(void);
void handleSettingsButtons(void);
void playRocketAnimation(void);  // ADD THIS LINE
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

/**
 * @brief Application entry point
 *
 * Performs system initialization, plays startup animation,
 * and continuously executes the main application loop.
 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	// Initialize HAL library and system tick timer
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  // Configure system clocks and RTC oscillator
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  // Initialize LCD in 4-bit mode
  // Display startup animation and system status

  LCD_Init();

    // Play the rocket launch splash screen

    playRocketAnimation();
    HAL_Delay(2000); // Hold the "SYSTEM ONLINE" message

    // Continue with your original startup message or go straight to the clock
    LCD_Clear();
  LCD_WriteStringXY(0, 0, "Multi-Purpose");
  LCD_WriteStringXY(1, 0, "Clock Ready!");
  HAL_Delay(2000);
  LCD_Clear();
    //start the RTC so time begins from 00:00:00 here
    MX_RTC_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // ================= MAIN APPLICATION LOOP =================
  // Handles input, updates stopwatch, and refreshes display
  // =========================================================

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//	  LCD_Clear();
//
//	      if(HAL_GPIO_ReadPin(reset_pin_GPIO_Port, reset_pin_Pin) == GPIO_PIN_RESET) {
//	          LCD_WriteStringXY(0, 0, "RESET: PRESSED");
//	      } else {
//	          LCD_WriteStringXY(0, 0, "RESET: RELEASED");
//	      }
//
//	      if(HAL_GPIO_ReadPin(start_stop_pin_GPIO_Port, start_stop_pin_Pin) == GPIO_PIN_RESET) {
//	          LCD_WriteStringXY(1, 0, "START: PRESSED");
//	      } else {
//	          LCD_WriteStringXY(1, 0, "START: RELEASED");
//	      }
//
//	      HAL_Delay(200);
	  handleButtons();

	  	          // Handle stopwatch timing
	  	          if(currentMode == MODE_STOPWATCH) {
	  	              handleStopwatch();
	  	          }

	  	          // Update display
	  	          updateDisplay();
	  	          HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_D6_Pin|LCD_EN_Pin|LCD_D5_Pin|LCD_D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_D7_Pin|LCD_RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : reset_pin_Pin */
  GPIO_InitStruct.Pin = reset_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(reset_pin_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_D6_Pin LCD_EN_Pin LCD_D5_Pin LCD_D4_Pin */
  GPIO_InitStruct.Pin = LCD_D6_Pin|LCD_EN_Pin|LCD_D5_Pin|LCD_D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_D7_Pin LCD_RS_Pin */
  GPIO_InitStruct.Pin = LCD_D7_Pin|LCD_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : start_stop_pin_Pin mode_pin_Pin */
  GPIO_InitStruct.Pin = start_stop_pin_Pin|mode_pin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
// Display clock mode
//Fetches time from RTC and formats it for a 16x2 LCD.
void displayClock(void) {
    char buffer[17];
    // Fetch current time and date from RTC hardware

    HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BIN);

    // Changed to exactly 16 characters
    snprintf(buffer, sizeof(buffer), "T:%02d:%02d:%02d ",
             currentTime.Hours, currentTime.Minutes, currentTime.Seconds);
    LCD_WriteStringXY(0, 0, buffer);

    // Second line
    LCD_WriteStringXY(1, 0, "MODE>SW>SET");
}

// Display stopwatch mode
/**
 * @brief Displays stopwatch time and status
 *
 * Stopwatch time is derived from system tick counter
 * for millisecond-level resolution.
 */

void displayStopwatch(void) {
    char buffer[17];
    uint32_t hours, minutes, seconds;

    // Convert milliseconds to time components
    seconds = stopwatchElapsed / 1000;
    minutes = seconds / 60;
    hours = minutes / 60;

    seconds %= 60;
    minutes %= 60;

    // Display stopwatch on first line
    snprintf(buffer, sizeof(buffer), "SW: %02lu:%02lu:%02lu",
             hours, minutes, seconds);
    LCD_WriteStringXY(0, 0, buffer);

    // Display status and controls on second line - FIXED
    if(stopwatchRunning) {
        snprintf(buffer, sizeof(buffer), "Reset Mode Stop");
    } else {
        snprintf(buffer, sizeof(buffer), "Reset Mode Start");
    }
    LCD_WriteStringXY(1, 0, buffer);
}

// Update display based on current mode
void updateDisplay(void) {
    LCD_Clear();

    switch(currentMode) {
        case MODE_CLOCK:
            displayClock();
            break;
        case MODE_STOPWATCH:
            displayStopwatch();
            break;
        case MODE_SETTINGS:
            displaySettings();
            break;
    }
}

// Handle mode button (PB9)
/**
 * @brief Handles mode switching using MODE button
 *
 * Cycles through CLOCK → STOPWATCH → SETTINGS modes.
 * Software debouncing is implemented using HAL_GetTick().
 */

void handleModeButton(void) {
    static uint32_t lastPress = 0;
    // Debounce logic to prevent multiple triggers

    if(HAL_GPIO_ReadPin(mode_pin_GPIO_Port, mode_pin_Pin) == GPIO_PIN_RESET) {
        if(HAL_GetTick() - lastPress > 300) {
            if(currentMode == MODE_CLOCK) {
                currentMode = MODE_STOPWATCH;
            } else if(currentMode == MODE_STOPWATCH) {
                currentMode = MODE_SETTINGS;
                // Initialize settings with current time
                HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);
                settingMode = SET_HOURS;
            } else {
                currentMode = MODE_CLOCK;
            }
            LCD_Clear();
            lastPress = HAL_GetTick();
        }
    }
}

// Handle stopwatch control buttons
void handleStopwatchButtons(void) {
    static uint32_t lastStartStop = 0;
    static uint32_t lastReset = 0;

    if(HAL_GPIO_ReadPin(start_stop_pin_GPIO_Port, start_stop_pin_Pin) == GPIO_PIN_RESET) {
        if(HAL_GetTick() - lastStartStop > 300) {
            if(!stopwatchRunning) {
                stopwatchStartTime = HAL_GetTick() - stopwatchElapsed;
                stopwatchRunning = 1;
            } else {
                stopwatchRunning = 0;
            }
            lastStartStop = HAL_GetTick();
        }
    }

    if(HAL_GPIO_ReadPin(reset_pin_GPIO_Port, reset_pin_Pin) == GPIO_PIN_RESET) {
        if(HAL_GetTick() - lastReset > 300 && !stopwatchRunning) {
            stopwatchElapsed = 0;
            lastReset = HAL_GetTick();
        }
    }
}
/**
 * @brief Handles user input in SETTINGS mode
 *
 * START/STOP button switches between hour/minute/save fields.
 * RESET button increments selected field or confirms save.
 */

void handleSettingsButtons(void) {
    static uint32_t lastSelectPress = 0;
    static uint32_t lastIncrementPress = 0;

    // SELECT button (START/STOP) - Change setting field
    if(HAL_GPIO_ReadPin(start_stop_pin_GPIO_Port, start_stop_pin_Pin) == GPIO_PIN_RESET) {
        if(HAL_GetTick() - lastSelectPress > 300) {
            if(settingMode == SET_HOURS) {
                settingMode = SET_MINUTES;
            } else if(settingMode == SET_MINUTES) {
                settingMode = SET_SAVE;
            } else {
                settingMode = SET_HOURS;
            }
            lastSelectPress = HAL_GetTick();
        }
    }

    // INCREMENT button (RESET) - Increase value or save
    if(HAL_GPIO_ReadPin(reset_pin_GPIO_Port, reset_pin_Pin) == GPIO_PIN_RESET) {
        if(HAL_GetTick() - lastIncrementPress > 300) {
            if(settingMode == SET_HOURS) {
                currentTime.Hours = (currentTime.Hours + 1) % 24;
            } else if(settingMode == SET_MINUTES) {
                currentTime.Minutes = (currentTime.Minutes + 1) % 60;
            } else if(settingMode == SET_SAVE) {
                // Save to RTC
                currentTime.TimeFormat = RTC_HOURFORMAT_24;
                currentTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
                currentTime.StoreOperation = RTC_STOREOPERATION_RESET;
                HAL_RTC_SetTime(&hrtc, &currentTime, RTC_FORMAT_BIN);

                // Return to clock mode
                currentMode = MODE_CLOCK;
            }
            lastIncrementPress = HAL_GetTick();
        }
    }
}
// Main button handler
void handleButtons(void) {
    handleModeButton();

    if(currentMode == MODE_STOPWATCH) {
        handleStopwatchButtons();
    } else if(currentMode == MODE_SETTINGS) {
        handleSettingsButtons();
    }
}

// Handle stopwatch control buttons
void handleStopwatch(void) {
    if(stopwatchRunning) {
        stopwatchElapsed = HAL_GetTick() - stopwatchStartTime;
    }
}
// Display settings mode
/**
 * @brief Displays time-setting interface on LCD
 *
 * Selected field blinks at 500ms interval for user feedback.
 * Brackets indicate the currently editable field.
 */

void displaySettings(void) {
    char buffer[17];
    // Software-controlled blink for selected field

    // Blink every 500ms
    static uint32_t lastBlink = 0;
    static uint8_t blinkState = 1;
    if (HAL_GetTick() - lastBlink > 500) {
        blinkState = !blinkState;
        lastBlink = HAL_GetTick();
    }

    // First line: Setting instruction
    switch (settingMode) {
        case SET_HOURS:
            LCD_WriteStringXY(0, 0, "Set Hours:      "); // padded to clear line
            break;
        case SET_MINUTES:
            LCD_WriteStringXY(0, 0, "Set Minutes:    ");
            break;
        case SET_SAVE:
            LCD_WriteStringXY(0, 0, "Save Time?      ");
            LCD_WriteStringXY(1, 0, "Press INC to save");
            return; // fine — we only update static screen here
    }

    // Second line: Time display with stable brackets
    if (settingMode == SET_HOURS) {
        if (blinkState)
            snprintf(buffer, sizeof(buffer), "[%02d]:%02d  ", currentTime.Hours, currentTime.Minutes);
        else
            snprintf(buffer, sizeof(buffer), " %02d :%02d  ", currentTime.Hours, currentTime.Minutes);
    } else if (settingMode == SET_MINUTES) {
        if (blinkState)
            snprintf(buffer, sizeof(buffer), " %02d:[%02d] ", currentTime.Hours, currentTime.Minutes);
        else
            snprintf(buffer, sizeof(buffer), " %02d : %02d  ", currentTime.Hours, currentTime.Minutes);
    }

    LCD_WriteStringXY(1, 0, buffer);
}
// 1. Define the pixel data for the rocket and flames (4 custom characters)
/* ================= LCD CUSTOM CHARACTERS =================
 * Custom CGRAM characters used for startup animation.
 * Adds visual feedback and improves system aesthetics.
 * ========================================================= */

// Character 0: The top of the rocket
uint8_t rocket_top[8] = {
    0b00100, //   *
    0b01110, //  ***
    0b01110, //  ***
    0b01110, //  ***
    0b01110, //  ***
    0b01110, //  ***
    0b01110, //  ***
    0b00000
};

// Character 1: The bottom of the rocket with fins
uint8_t rocket_bottom[8] = {
    0b01110, //  ***
    0b01110, //  ***
    0b11111, // *****
    0b11111, // *****
    0b11111, // *****
    0b10101, // * * *
    0b10101, // * * *
    0b00000
};

// Character 2: The first frame of the flame animation
uint8_t flame_1[8] = {
    0b00100, //   *
    0b01110, //  ***
    0b10101, // * * *
    0b01110, //  ***
    0b11111, // *****
    0b01010, //  * *
    0b10101, // * * *
    0b00100
};

// Character 3: The second frame of the flame animation
uint8_t flame_2[8] = {
    0b01110, //  ***
    0b11111, // *****
    0b01110, //  ***
    0b11111, // *****
    0b10101, // * * *
    0b11111, // *****
    0b01110, //  ***
    0b00100
};


// Make sure you have this function in parallel_lcd.c and its prototype in parallel_lcd.h
/*
void LCD_CreateChar(uint8_t location, uint8_t charmap[]) {
    location &= 0x07; // We only have 8 locations 0-7
    LCD_Command(0x40 | (location << 3)); // Set CGRAM Address
    for (int i=0; i<8; i++) {
        LCD_WriteChar(charmap[i]);
    }
}
*/

/**
 * @brief Displays startup rocket animation on LCD
 *
 * Uses LCD CGRAM custom characters for visual effects.
 * Enhances user experience during system startup.
 */

// 2. Create the animation function
void playRocketAnimation(void) {
    // Load the custom characters into the LCD's memory
    LCD_CreateChar(0, rocket_top);
    LCD_CreateChar(1, rocket_bottom);
    LCD_CreateChar(2, flame_1);
    LCD_CreateChar(3, flame_2);

    LCD_Clear();

    // -- Phase 1: Ignition --
    // Place the rocket on the "ground" (bottom line)
    LCD_SetCursor(1, 7); // Center on bottom line
    LCD_WriteChar(1);    // Draw the rocket bottom

    // Flicker the flames for a bit
    for (int i = 0; i < 6; i++) {
        LCD_SetCursor(1, 7);
        LCD_WriteChar(2); // Draw flame 1
        HAL_Delay(80);
        LCD_SetCursor(1, 7);
        LCD_WriteChar(3); // Draw flame 2
        HAL_Delay(80);
    }

    // -- Phase 2: Launch --
    // Frame 1: Full rocket with flames
    LCD_Clear();
    LCD_SetCursor(0, 7); // Top part of rocket on line 0
    LCD_WriteChar(0);
    LCD_SetCursor(1, 7); // Bottom part of rocket on line 1
    LCD_WriteChar(1);
    HAL_Delay(150);

    // Frame 2: Rocket moves up
    LCD_Clear();
    LCD_SetCursor(0, 7); // Full rocket now on line 0
    LCD_WriteChar(0);
    LCD_SetCursor(1, 7); // Flames are now on line 1
    LCD_WriteChar(2);
    HAL_Delay(150);

    // Frame 3: Rocket flies off screen
    LCD_Clear();
    LCD_SetCursor(0, 7); // Only flames remain on line 0
    LCD_WriteChar(3);
    HAL_Delay(150);

    // -- Phase 3: Reveal Text --
    LCD_Clear();
    LCD_WriteStringXY(0, 2, "SYSTEM ONLINE");
    LCD_WriteStringXY(1, 0, "----------------");
}

/* ... rest of your functions like displayClock, handleButtons, etc. ... */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
