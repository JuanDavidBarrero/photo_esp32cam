#include "SD_functions.h"

#define FILE_PHOTO "/photo.jpg"


void initMicroSDCard()
{
  // Start Micro SD card
  Serial.println("Starting SD Card");
  if (!SD_MMC.begin())
  {
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if (cardType == CARD_NONE)
  {
    Serial.println("No SD Card attached");
    return;
  }
}


void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("an error has occured with SPIFFS please check");
    while (true)
    {
    }
  }
  Serial.println("SPIFFS mounted successfully !!");
}

bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}
