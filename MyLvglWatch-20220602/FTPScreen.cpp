/*
 * Compilation PASSED.
 * HOOKED TO FTPNOSCREEN
 * HOOKED TO DATETIMESCREEN
 * TOGGLE BUTTON OK
 * NOT TESTED
 * 2022-06-04
 */
#include <WiFi.h>
#include "FTPScreen.h"
#include "DateTimeScreen.h"
#include "FTPServerNoScreen.h"

static lv_obj_t *toggleLabel=NULL;         // toggle button label

static lv_obj_t *dateLabel = NULL;  // label to show date
static lv_obj_t *battLabel = NULL;  // label to show batt
static lv_obj_t *statusLabel = NULL;  // label to show FTP status

static void lv_update_task(struct _lv_task_t *);

void updateScreenData3 () {
    TTGOClass *ttgo = TTGOClass::getWatch();    // pointer to watch internals
    PCF8563_Class *rtc = ttgo->rtc;             // pointer to RTC (real-time clock) 
    lv_label_set_text(dateLabel, rtc->formatDateTime(PCF_TIMEFORMAT_HM));  // display time without seconds
    int battLevel = ttgo->power->getBattPercentage();
    lv_label_set_text_fmt(battLabel, "%3d%s", battLevel,"%");  // display battery level.
    IPAddress ipToPrint = WiFi.localIP();
    lv_label_set_text_fmt(statusLabel, "%u.%u.%u.%u", ipToPrint[0], ipToPrint[1], ipToPrint[2], ipToPrint[3]);  // display IP
}

static void lv_update_task(struct _lv_task_t *) {
  updateScreenData3 ();
}

static void button_event_handler(lv_obj_t *obj, lv_event_t event)
/*
 * Handles touch (click) events on the button in this screen. 
*/
{
    if (event == LV_EVENT_CLICKED) {
      /* this event happens to toggle and std buttons. */
        closeFTPandSPIFFSandWiFi(); // turn FTP server off if exits the screen
        buildWatchFace();
        updateScreenData();
        Serial.printf("Clicked\n"); 
    }
}


static void toggle_event_handler(lv_obj_t *obj, lv_event_t event)
/*
 * Handles touch (click) events on the toggle button in this screen. 
*/
{
    if (event == LV_EVENT_VALUE_CHANGED) {
        Serial.printf("Toggled\n");
        switch (lv_btn_get_state(obj)) {
          case LV_BTN_STATE_CHECKED_RELEASED:
            lv_label_set_text(toggleLabel, "Turn FTP off");                 // set button 2 label text.
            startFTP();
            break;
          case LV_BTN_STATE_RELEASED:
            lv_label_set_text(toggleLabel, "Turn FTP on");                 // set button 2 label text.
            closeFTPandSPIFFSandWiFi();
            break;
        }
    }
}

void buildWatchFace3 () {
/*
 * Build a watch face (a screen) containing one button,
 * one label and one toggle.
 * Click on the button to display current time in the label.
 * Click on the toggle to change its state (color).
 * Events are also logged in the serial monitor.
 * Shows how to create and load screen and container objects
 * It seems slower than getting current screen and modifying it.
 */
    lv_obj_t *scrWatch = lv_obj_create (NULL, NULL);
    static lv_style_t opaqueBackgroundStyle;
    /* styles do not set sizes. */

    lv_style_set_border_color(&opaqueBackgroundStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_border_opa(&opaqueBackgroundStyle, LV_OBJ_PART_MAIN, LV_OPA_100); // opaque
    lv_style_set_bg_color(&opaqueBackgroundStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&opaqueBackgroundStyle, LV_OBJ_PART_MAIN, LV_OPA_100); // opaque

    lv_obj_t *cont = scrWatch;
    lv_obj_add_style(cont, LV_OBJ_PART_MAIN, &opaqueBackgroundStyle);  // apply style

    lv_obj_t *label;  // label - auxiliary pointer to some button's label.

    static lv_style_t buttonStyle;
    lv_style_set_bg_color(&buttonStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    // lv_style_set_bg_opa(&buttonStyle, LV_OBJ_PART_MAIN, LV_OPA_0); // transparent
    lv_style_set_bg_opa(&buttonStyle, LV_OBJ_PART_MAIN, LV_OPA_100); // opaque
    lv_style_set_text_color(&buttonStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_text_font(&buttonStyle, LV_STATE_DEFAULT, &lv_font_montserrat_14);

  // button 1
    lv_obj_t *btn1 = lv_btn_create(cont, NULL);  // create button 1. Its parent is the container
    lv_obj_set_event_cb(btn1, button_event_handler);            // button 1 event handler
    lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, -50);   // button 1 text alignment

    lv_obj_add_style(btn1, LV_OBJ_PART_MAIN, &buttonStyle);  // apply style
    label = lv_label_create(btn1, NULL);                 // create a label inside button 1 and get a pointer to it.
    lv_label_set_text(label, "Exit screen");                  // set button 1 label text

  // button 2 (checkbutton)
    lv_obj_t *btn2 = lv_btn_create(cont, NULL);
    lv_obj_set_event_cb(btn2, toggle_event_handler); 
    lv_obj_align(btn2, NULL, LV_ALIGN_CENTER, 0, 50);
    lv_btn_set_checkable(btn2, true);                    // button 2 is a checkbutton
    lv_btn_toggle(btn2);                                 // set button 2 to be a toggle button: https://docs.lvgl.io/latest/en/html/widgets/btn.html#checkable
                                                         // I suppose button color change is managed transparently
    lv_btn_set_fit2(btn2, LV_FIT_NONE, LV_FIT_TIGHT);

    lv_obj_add_style(btn2, LV_OBJ_PART_MAIN, &buttonStyle);  // apply style
    toggleLabel = lv_label_create(btn2, NULL);                 // create a label inside button 2 and get a pointer to it.
    lv_label_set_text(toggleLabel, "Turn FTP on");                 // set button 2 label text.

    /**/
    static lv_style_t infoStyle;
    lv_style_set_bg_color(&infoStyle, LV_OBJ_PART_MAIN, LV_COLOR_BLACK);
    lv_style_set_bg_opa(&infoStyle, LV_OBJ_PART_MAIN, LV_OPA_100); // opaque
    lv_style_set_text_color(&infoStyle, LV_OBJ_PART_MAIN, LV_COLOR_WHITE);
    lv_style_set_text_font(&infoStyle, LV_STATE_DEFAULT, &lv_font_montserrat_28);

    dateLabel = lv_label_create(cont, NULL);
    lv_obj_add_style(dateLabel, LV_OBJ_PART_MAIN, &infoStyle);  // apply style
    lv_label_set_text(dateLabel, "T-Watch");
    lv_obj_align(dateLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);

    /**/
    battLabel = lv_label_create(cont, NULL);
    lv_obj_add_style(battLabel, LV_OBJ_PART_MAIN, &infoStyle);  // apply style
    lv_label_set_text(battLabel, "T-Watch");
    lv_obj_align(battLabel, NULL, LV_ALIGN_IN_TOP_LEFT, 170, 0);

    /**/
    statusLabel = lv_label_create(cont, NULL);
    lv_obj_add_style(statusLabel, LV_OBJ_PART_MAIN, &infoStyle);  // apply style
    lv_label_set_text(statusLabel, "T-Watch");
    lv_obj_align(statusLabel, NULL, LV_ALIGN_CENTER, 0, 0);

    lv_task_create(lv_update_task, 1000, LV_TASK_PRIO_LOWEST, NULL);

    lv_scr_load(scrWatch); // I think this makes screen render twice.
}
