# TTGO T-Watch Experiments

## Presentation

Hi!

My intention is to document and reference experiences/experiments programming TTGO T-Watch V.3 using Arduino IDE.

## Tools and libraries

- Arduino IDE vers. 1.8.16
- TTGO T-Watch Library with (default) LVGL vers. 7.4
- ESP32 board Arduino Library vers. 1.0.6

## Conclusions

### Difference between .INO .C and .CPP in multifile sketches in Arduino IDE - ESP32

It seems that .INO are merged and compiled as one large file (*static and external keywords behavior indicate it*). .C files are standard C files (*objects are not allowed*), .CPP are standard C++ files.(Experiments to be presented).

### LVGL and FreeRTOS might not be compatible.

LVGL saves a lot of programmer's work. It handles display devices, touchscreens, display text, image and buttons and handles events, allowing the programmer to build a GUI on the watch.

FreeRTOS is necessary and popular to build multithreaded applications such as web servers on small devices like ESP32.

Unfortunately LVGL documentation indicate that there might be issues when executing it along with FreeRTOS:

From LVGL 7/8 Documentation (https://docs.lvgl.io/master/porting/os.html?highlight=thread%20safe#operating-system-and-interrupts):

> LVGL is not thread-safe by default.

From LVGL 7/8, FreeRTOS is marked as *TODO* https://docs.lvgl.io/master/get-started/os/freertos.html#freertos

## References

1. [TTGO T-Watch library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library). I downloaded it on 2022 May 6th. There is an interesting commit about faster startup on May 10th. 
2. [Markus Wobish blogger blog](https://markus-wobisch.blogspot.com/2021/03/lilygo-ttgo-t-watch-2020-getting.html). I first read it circa 2022 May 7th.
3. [Dan Geiger at Instructables](https://www.instructables.com/Lilygo-T-Watch-2020-Arduino-Framework/). I first read it circa 2022 May 7th.
4. [LVGL V.7.11 documentation](https://docs.lvgl.io/7.11/). I am consulting it from (circa) 2022 May 12th.
