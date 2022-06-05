/*
 * Based on Renzo Mischianti's FtpServer esp8266 and esp32 with SPIFFS
 * https://www.mischianti.org/2020/02/08/ftp-server-on-esp8266-and-esp32
 *
 */

#include <WiFi.h>
#include "SPIFFS.h"

#include <SimpleFTPServer.h>

#include "NetworkNoScreen.h"

#include "FTPServerNoScreen.h"  // To include this file here is not mandatory

FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial

void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace){
  switch (ftpOperation) {
    case FTP_CONNECT:
      Serial.println(F("FTP: Connected!"));
      break;
    case FTP_DISCONNECT:
      Serial.println(F("FTP: Disconnected!"));
      break;
    case FTP_FREE_SPACE_CHANGE:
      Serial.printf("FTP: Free space change, free %u of %u!\n", freeSpace, totalSpace);
      break;
    default:
      break;
  }
};
void _transferCallback(FtpTransferOperation ftpOperation, const char* name, unsigned int transferredSize){
  switch (ftpOperation) {
    case FTP_UPLOAD_START:
      Serial.println(F("FTP: Upload start!"));
      break;
    case FTP_UPLOAD:
      Serial.printf("FTP: Upload of file %s byte %u\n", name, transferredSize);
      break;
    case FTP_TRANSFER_STOP:
      Serial.println(F("FTP: Finish transfer!"));
      break;
    case FTP_TRANSFER_ERROR:
      Serial.println(F("FTP: Transfer error!"));
      break;
    default:
      break;
  }

  /* FTP_UPLOAD_START = 0,
   * FTP_UPLOAD = 1,
   *
   * FTP_DOWNLOAD_START = 2,
   * FTP_DOWNLOAD = 3,
   *
   * FTP_TRANSFER_STOP = 4,
   * FTP_DOWNLOAD_STOP = 4,
   * FTP_UPLOAD_STOP = 4,
   *
   * FTP_TRANSFER_ERROR = 5,
   * FTP_DOWNLOAD_ERROR = 5,
   * FTP_UPLOAD_ERROR = 5
   */
};

bool FTPServerStarted = false;

void startFTP(){
  if (wiFiOn()) {

    /////FTP Setup, ensure SPIFFS is started before ftp;  /////////
    
    if (SPIFFS.begin(true)) {
        ftpSrv.setCallback(_callback);
        ftpSrv.setTransferCallback(_transferCallback);
  
        Serial.println("SPIFFS opened!");
        ftpSrv.begin("esp8266","esp8266");    //username, password for ftp.   (default 21, 50009 for PASV)
        FTPServerStarted=true;
    }    
  }
}

void closeFTPandSPIFFSandWiFi() {
  FTPServerStarted=false;
  ftpSrv.end(); // https://github.com/xreef/SimpleFTPServer/blob/master/FtpServer.h linha 507
  SPIFFS.end(); // https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#end
  wiFiOff();
}

/*  The alternatives on how to merge
 *  This causes some sort of question:
 *  LVGL needs its event_handler function
 *  inside loop function (as in the main
 *  file: MyLvglWatch-20220602).
 *  One could say "what if ftpSrv.handleFTP()
 *  is called from inside a lvgl task?"
 *  I think it may cause some unexpected
 *  behavior (e.g. freeze lvgl, reboot watch,...
 *  I decided to create a function to be
 *  inserted in loop().
 *  
*/

bool FTPTestAndHandle(void){  // to insert into loop() function
  if (FTPServerStarted)
    ftpSrv.handleFTP();        //make sure in loop you call handleFTP()!!  
  return FTPServerStarted;
}
