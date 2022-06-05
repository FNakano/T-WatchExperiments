#include "NetworkNoScreen.h"
/* WiFi settings change ssid and password 
 * according to your local setup.
 */
const char *ssid            = "NameOfNetworkTP";
const char *password        = "0123456789";

/* NTP (Network Time Protocol) settings */
const char *ntpServer       = "pool.ntp.org";  // NTP server name
const long  gmtOffset_sec   = -3*3600;         // timezone offset in seconds
const int   daylightOffset_sec = 0;            // daylight saving time offset


void wiFiOff () 
/* Code do turn Wifi off.
 * based on: https://www.mischianti.org/2021/03/06/esp32-practical-power-saving-manage-wifi-and-cpu-1/
*/
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    adc_power_off();
}

int wiFiOn () {
    int nWiFiProbe = 0;
    int MAX_WIFIPROBE = 20;
    // turn ESP32 WiFi on and connect to Access Point
    Serial.printf("Trying to connect to %s \n", ssid);

    adc_power_on();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        //tft->print(".");
        if (nWiFiProbe>MAX_WIFIPROBE) {
            wiFiOff();
            return WIFI_FAILED_TO_CONNECT;
        }
        nWiFiProbe++;
    }
    Serial.println(" CONNECTED");
    return WIFI_CONNECTED;
}

int setClockNTP() 
/*
 * Turn radio (WiFi) on, connect to AP, requests
 * current time from NTP, store time into RTC, 
 * turn radio off.
 */
{
    TTGOClass *ttgo = TTGOClass::getWatch();    // pointer to watch internals
    PCF8563_Class *rtc = ttgo->rtc;             // pointer to RTC (real-time clock) 

    if (wiFiOn()) {
      //init, get current time from NTP server and store it in ESP32 variable.
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
          Serial.println("Failed to obtain time.");
          wiFiOff();
          return WIFI_NTP_FAILED;
      }
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      rtc->syncToRtc(); // write current time into watch RTC (PCF8563)
      return WIFI_NTP_CLOCK_SET;
    }
    wiFiOff();
    return WIFI_NTP_FAILED;
}
