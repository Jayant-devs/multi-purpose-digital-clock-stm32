\## Software Architecture



\- main.c

&nbsp; - Infinite loop

&nbsp; - Mode-based state machine



\- handleButtons()

&nbsp; - Delegates to mode-specific handlers



\- displayClock()

\- displayStopwatch()

\- displaySettings()



\## State Machine

MODE\_CLOCK → MODE\_STOPWATCH → MODE\_SETTINGS → MODE\_CLOCK



\## Timing

\- RTC → real time

\- HAL\_GetTick() → stopwatch



