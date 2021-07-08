#include <Arduino.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include <camera_esp32.h>


unsigned int pictureNumber = 0;


void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //
  Serial.begin(115200);
  Serial.print("Initializing the camera module...");
  configInitCamera();
  Serial.println("Ok!");
  Serial.print("Initializing the MicroSD card module... ");
  initMicroSDCard();
}

void loop()
{
  String path = "/picture" + String(pictureNumber) + ".jpg";
  Serial.printf("Picture file name: %s\n", path.c_str());
  takeSavePhoto(path);
  pictureNumber++;
  delay(10000);
}