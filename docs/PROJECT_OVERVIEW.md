\# Multi-Purpose Digital Clock – STM32F446RE



\## What this project does

\- RTC-based digital clock (HH:MM:SS)

\- Stopwatch with millisecond precision

\- Time setting (hours/minutes) via buttons

\- 16x2 LCD UI with animations



\## Hardware Used

\- STM32F446RE (Nucleo)

\- 16x2 LCD (HD44780, 4-bit mode)

\- 3 Push Buttons (MODE, START/STOP, RESET)

\- 10k Potentiometer (LCD contrast)

\- Optional: LDR, buzzer, LEDs



\## Software Stack

\- STM32CubeIDE + HAL

\- RTC with LSE (32.768 kHz)

\- Custom LCD driver (parallel\_lcd.c)



\## Operating Modes

1\. Clock Mode – RTC display

2\. Stopwatch Mode – HAL\_GetTick()

3\. Settings Mode – Time adjust + Save



\## How to run

1\. Power via USB

2\. After splash screen → Clock mode

3\. MODE → Stopwatch → Settings



