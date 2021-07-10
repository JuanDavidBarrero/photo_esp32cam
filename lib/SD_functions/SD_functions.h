#ifndef SD_functions_h
#define SD_functions_h

#include <Arduino.h>
#include "FS.h"
#include "SD_MMC.h"
#include <SPIFFS.h>

void initMicroSDCard();

void initSPIFFS();

bool checkPhoto( fs::FS &fs );

#endif /* SD_functions_h */