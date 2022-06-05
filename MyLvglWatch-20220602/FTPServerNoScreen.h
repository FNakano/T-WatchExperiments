#ifndef FTPSERVERNOSCREEN_H

#define FTPSERVERNOSCREEN_H

void startFTP();
void closeFTPandSPIFFSandWiFi();
bool FTPTestAndHandle(void);  // to insert into loop() function
// true if FTP Server is on (to avoid going to sleep.

#endif
