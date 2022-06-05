/* Based on MyLvglWatch-20220529-d.ino.
 *  Created header files and include in the files 
 *  they are necessary. Example:
 *  Header file "DateTimeScreen.h" is included into
 *  ExampleScreen.cpp because ExampleScreen has a
 *  button to activate (go back to) DateTimeScreen.
 *  Freeze this version. The next one (20220503) 
 *  will explore FTP server and SPIFFS.
 */

#include "config.h" // choose watch version

#include "DateTimeScreen.h"
#include "NetworkNoScreen.h"
#include "PowerMgmtNoScreen.h"

/* flags to manage sleep and wake up. */
bool irq=false;         // indicates that a hardware button interrupt occured

TTGOClass *ttgo;    // after setup() execution, is a pointer to watch internals
PCF8563_Class *rtc; // after setup() execution, is a pointer to RTC (real-time clock) 

void setup()
{
  // backend setup
    Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    rtc = ttgo->rtc;

    // PEK KEY hardware button interrupt handling to set date and time.
    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irq = true;
    }, FALLING);
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
    //!Clear IRQ unprocessed  first
    ttgo->power->clearIRQ();

//  start screen 
    ttgo->lvgl_begin();
    ttgo->bl->adjust(20); /* minimum brightness */
    ttgo->openBL();   // check this. Looks like this function draws the screen or the screen is drawn in frame buffers by the task_handler.
    buildWatchFace();
    updateScreenData();

}

void loop()
{
  /* Hardware interrupts are treated here. LVGL interrupts (events) are treated in
   * LGVL way (event callback/register/loop)  
  */
    if (irq) { // hardware button interrupt response
        irq=false; // clear flag
        /* */
        ttgo->power->readIRQ();  // get the IRQ source
        if (ttgo->power->isPEKShortPressIRQ()) { // hardware button short press
            setClockNTP(); // adjust watch date and time according to NTP.
            // Clean power chip irq status
            ttgo->power->clearIRQ();
        }
    }
/* to reduce power consumption */
/* inspired in batmandial example */
    // Set 20MHz operating speed to reduce power consumption
    // setCpuFrequencyMhz(20);  // it slows down screen update. Change CPU freq. works.
    // looks it is rendering the screen twice. I think its caused by lv_scr_load(scrWatch)
    // in DateTimeScreen.ino
    uint32_t tInactive= lv_disp_get_inactive_time(NULL);
    if (tInactive>3000) { // boot time is aprox. 3s
        setCpuFrequencyMhz(10); // to save power
    } else {
        setCpuFrequencyMhz(240); // maximum power
    }
    if (tInactive>10000) goDeepSleep();  // came back.
    lv_task_handler();
    delay(5);
}
