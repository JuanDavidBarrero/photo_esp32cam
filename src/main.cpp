#include <Arduino.h>

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"

#include <ESPAsyncWebServer.h>

#include <camera_esp32.h>

const char *ssid = "Daniela25";
const char *password = "19632528";

AsyncWebServer server(80);

boolean takeNewPhoto = false;

#define FILE_PHOTO "/photo.jpg"

unsigned int pictureNumber = 0;

void initWifi();
void initSPIFFS();

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //
  Serial.begin(115200);

  initWifi();
  initSPIFFS();

  Serial.print("Initializing the camera module...");
  configInitCamera();
  Serial.println("Ok!");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  server.on("/capture", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              takeNewPhoto = true;
              request->send_P(200, "text/plain", "Taking Photo");
            });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, FILE_PHOTO, "image/jpg", false); });

  server.begin();
}

void loop()
{
  if (takeNewPhoto) {
    capturePhotoSaveSpiffs();
    takeNewPhoto = false;
  }
  delay(1);
}

void initWifi()
{
  uint8_t count = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi . ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    if (count > 5)
    {
      ESP.restart();
    }
    count++;
  }
  Serial.println(WiFi.localIP());
}

