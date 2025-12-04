\# 🔧 Hardware Configuration



This document describes the complete hardware configuration and GPIO pin mapping

for the \*\*Multi-Purpose Digital Clock using STM32F446RE\*\*.



All pins are configured using \*\*STM32CubeMX\*\*, and the labels used here directly

match the definitions in `main.h`.



---



\## 🧠 Microcontroller Details



\- \*\*Board:\*\* STM32F446RE (NUCLEO-F446RE)

\- \*\*Core:\*\* ARM Cortex-M4F

\- \*\*Operating Voltage:\*\* 3.3 V

\- \*\*System Clock:\*\* HSI + PLL  

\- \*\*RTC Clock Source:\*\* LSE (32.768 kHz crystal)



---



\## 📟 16×2 LCD (HD44780 Compatible) — 4-Bit Mode



| LCD Signal | STM32 Pin | GPIO Port | Description |

|-----------|----------|-----------|-------------|

| RS | PA10 | GPIOA | Register Select (Command/Data) |

| EN | PB3 | GPIOB | Enable pin (Data latch) |

| D4 | PB5 | GPIOB | Data line D4 |

| D5 | PB4 | GPIOB | Data line D5 |

| D6 | PB10 | GPIOB | Data line D6 |

| D7 | PA8 | GPIOA | Data line D7 |

| VSS | GND | — | Ground |

| VDD | +5 V | — | LCD Power |

| V0 | Potentiometer | — | LCD Contrast Control |



✅ LCD is driven in \*\*4-bit parallel mode\*\* to reduce GPIO usage.



---



\## 🔘 Push Button Configuration



| Function | STM32 Pin | GPIO Port | Configuration |

|--------|----------|-----------|---------------|

| Mode Change | PB9 | GPIOB | Input, Pull-Up |

| Start / Stop / Select | PB8 | GPIOB | Input, Pull-Up |

| Reset / Increment | PC0 | GPIOC | Input, Pull-Up |



\- Buttons are \*\*Active-LOW\*\*

\- Software debouncing implemented using `HAL\_GetTick()`



---



\## ⏱ Real-Time Clock (RTC) Configuration



| Parameter | Value |

|---------|-------|

| Peripheral | Internal RTC |

| Clock Source | LSE (32.768 kHz crystal) |

| Time Format | 24-Hour |

| Backup Domain | Supported (VBAT capable) |



The RTC operates independently of the main CPU clock and maintains time across resets.



---



\## 🖥 STM32CubeMX Pin Configuration Snapshot



The following image shows the STM32CubeMX pin configuration used in this project.



!\[STM32 CubeMX Configuration](media/stm\_config.png)



---



\## ✅ Notes



\- All GPIO macros are defined in `main.h`

\- Logical abstractions are handled in `parallel\_lcd.h`

\- Any pin reassignment can be done entirely via STM32CubeMX

&nbsp; without modifying application or driver logic



---



📁 Related files:

\- `main.h`

\- `parallel\_lcd.h`

\- `hardwareconfig.md`



