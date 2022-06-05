#include "PowerMgmtNoScreen.h"
// Deep sleep and wakeup functions.
// WiFi power management is in NetworkNoScreen.ino file.

void goDeepSleep() {
  /*
   * Set wakeup through touchscreen and go deep sleep.
  */
    TTGOClass *ttgo = TTGOClass::getWatch();    // pointer to watch internals
    PCF8563_Class *rtc = ttgo->rtc;             // pointer to RTC (real-time clock) 
  /*
   * Turn screen and other hardware off (except ESP32), 
   * set touchscreen as the only wakeup source,
   * set ESP32 to deep sleep.
  */
    // Set  touchscreen sleep
    ttgo->displaySleep();

    /*
    In TWatch2019/ Twatch2020V1, touch reset is not connected to ESP32,
    so it cannot be used. Set the touch to sleep,
    otherwise it will not be able to wake up.
    Only by turning off the power and powering on the touch again will the touch be working mode
    // ttgo->displayOff();
    */

    ttgo->powerOff();  // contains the same code below (??)

    //Set all channel power off (turn sensors and devices other than ESP32 off)
    ttgo->power->setPowerOutPut(AXP202_LDO3, false);
    ttgo->power->setPowerOutPut(AXP202_LDO4, false);
    ttgo->power->setPowerOutPut(AXP202_LDO2, false);
    ttgo->power->setPowerOutPut(AXP202_EXTEN, false);
    ttgo->power->setPowerOutPut(AXP202_DCDC2, false);

    // TOUCH SCREEN  Wakeup source
    esp_sleep_enable_ext1_wakeup(GPIO_SEL_38, ESP_EXT1_WAKEUP_ALL_LOW);

    // PEK KEY hardware button interrupt handling
    /* esp_sleep_enable_ext1_wakeup(GPIO_SEL_35, ESP_EXT1_WAKEUP_ALL_LOW);  // uncomment to wake watch up from hardware button press
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irq = true;
    }, FALLING);
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true); */
    //!Clear IRQ unprocessed  first
    ttgo->power->clearIRQ();
    // enable touch interrupt (maybe wake up ESP32 is the only possible action)
    pinMode(TOUCH_INT, INPUT);
    esp_deep_sleep_start();  // Put ESP32 to deep sleep
}
