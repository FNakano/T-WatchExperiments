#ifndef NETWORKNOSCREEN_H

#define NETWORKNOSCREEN_H

#include "config.h"
#include <WiFi.h>
#include "time.h"
#include "driver/adc.h"   // adc functions. Needed to turn WiFi off.

#define WIFI_FAILED_TO_CONNECT 0
#define WIFI_CONNECTED 1

#define WIFI_NTP_CLOCK_SET 1
#define WIFI_NTP_FAILED 0

void wiFiOff ();
int wiFiOn ();
int setClockNTP();

#endif
