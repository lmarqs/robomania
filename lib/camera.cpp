#include "camera.h"

#if defined(ESP32) && __has_include(<esp_camera.h>)
#include <Arduino.h>
#include <esp_camera.h>

#define CAMERA_MODEL_AI_THINKER
#include "camera_pins.h"
#include "esp32_ledc.h"
#if ESP_ARDUINO_VERSION_MAJOR >= 3
#include <esp32-hal-periman.h>
#include <soc/soc_caps.h>
#endif

// The camera's XCLK runs on an LEDC timer and channel that its driver sets up through ESP-IDF, unseen by the core.
static const uint32_t XCLK_HZ = 8000000;

#if ESP_ARDUINO_VERSION_MAJOR < 3
static bool reserveClock(camera_config_t& config) {
  const int8_t timer = esp32LedcReserveTimer();
  if (timer < 0) {
    return false;
  }
  config.ledc_timer = (ledc_timer_t)timer;
  config.ledc_channel = (ledc_channel_t)(timer * 2);
  return true;
}

static void releaseClock(const camera_config_t& config) { esp32LedcReleaseTimer(config.ledc_timer); }
#else
// Core 3 counts a timer as used only when a pin it attached runs on it, so the XCLK pin is attached first, on the last
// channel of the low-speed group the camera driver uses, and the driver then sets the same timer and channel again.
static bool reserveClock(camera_config_t& config) {
#ifdef SOC_LEDC_SUPPORT_HS_MODE
  const uint8_t channel = SOC_LEDC_CHANNEL_NUM * 2 - 1;
#else
  const uint8_t channel = SOC_LEDC_CHANNEL_NUM - 1;
#endif
  if (!ledcAttachChannel(XCLK_GPIO_NUM, XCLK_HZ, 1, channel)) {
    return false;
  }
  const ledc_channel_handle_t* bus = (ledc_channel_handle_t*)perimanGetPinBus(XCLK_GPIO_NUM, ESP32_BUS_TYPE_LEDC);
  config.ledc_timer = (ledc_timer_t)bus->timer_num;
  config.ledc_channel = (ledc_channel_t)(channel % SOC_LEDC_CHANNEL_NUM);
  return true;
}

static void releaseClock(const camera_config_t&) { ledcDetach(XCLK_GPIO_NUM); }
#endif

bool EspCamera::begin() {
  if (esp_camera_sensor_get()) {
    return true;
  }

  camera_config_t config = {};
  if (!reserveClock(config)) {
    Serial.println("No LEDC timer left for the camera clock");
    return false;
  }
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = XCLK_HZ;
  config.pixel_format = PIXFORMAT_JPEG;

  config.jpeg_quality = 10;
  config.fb_count = 1;
  config.fb_location = CAMERA_FB_IN_DRAM;
  config.frame_size = FRAMESIZE_QVGA;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  if (psramFound()) {
    config.fb_count = 2;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    releaseClock(config);
    return false;
  }

  sensor_t* sensor = esp_camera_sensor_get();

  if (sensor->id.PID == OV3660_PID) {
    sensor->set_vflip(sensor, 1);
    sensor->set_brightness(sensor, 1);
    sensor->set_saturation(sensor, -2);
  }

#if defined(CAMERA_MODEL_M5STACK_WIDE) || defined(CAMERA_MODEL_M5STACK_ESP32CAM)
  sensor->set_vflip(sensor, 1);
  sensor->set_hmirror(sensor, 1);
#endif

#if defined(CAMERA_MODEL_ESP32S3_EYE)
  sensor->set_vflip(sensor, 1);
#endif

  return true;
}

void EspCamera::capture(EspCameraPictureReader reader) {
  camera_fb_t* fb = esp_camera_fb_get();

  if (!fb) {
    Serial.println("Frame buffer could not be acquired");
    return;
  }

  reader(fb);

  esp_camera_fb_return(fb);

  fb = NULL;
}
#endif
