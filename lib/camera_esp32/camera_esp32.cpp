
#include <Arduino.h>
#include "camera_esp32.h"
#include "fd_forward.h"

camera_config_t config;
#define FILE_PHOTO "/photo.jpg"

void configInitCamera()
{
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG
  // Select lower framesize if the camera doesn't support PSRAM
  if (psramFound())
  {
    config.frame_size = FRAMESIZE_QVGA; //FRAMESIZE_+QVGA|CIF|VGA|SVGA|XG
    config.jpeg_quality = 5;           //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  else
  {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK)
  {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

void takeSavePhoto(String path)
{
  // Take Picture with Camera
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("Camera capture failed");
    return;
  }
  // Save picture to microSD card
  fs::FS &fs = SD_MMC;
  File file = fs.open(path.c_str(), FILE_WRITE);
  if (!file)
  {
    Serial.println("Failed to open file in writing mode");
  }
  else
  {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  file.close();
  // return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
}

void capturePhotoSaveSpiffs()
{

  mtmn_config_t mtmn_config = {0};
  int detections = 0;

  mtmn_config = mtmn_init_config();

  camera_fb_t *fb = NULL; // pointer
  bool ok = 0;            // Boolean indicating if the picture has been taken correctly

  do
  {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb)
    {
      Serial.println("Camera capture failed");
      return;
    }

    dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);

    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file)
    {
      Serial.println("Failed to open file in writing mode");
    }
    else
    {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);

    box_array_t *boxes = face_detect(image_matrix, &mtmn_config);

    if (boxes != NULL)
    {
      detections = detections + 1;
      Serial.printf("Faces detected %d times \n", detections);
    }

    dl_matrix3du_free(image_matrix);

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS);
  } while (!ok);
}