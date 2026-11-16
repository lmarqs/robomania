#pragma once

// Only ESP32 chips with a camera interface, such as the ESP32-CAM's, ship the camera driver.
#if defined(ESP32) && __has_include(<esp_camera.h>)
#include <Arduino.h>
#include <esp_camera.h>

using EspCameraPictureReader = std::function<void(camera_fb_t*)>;

class EspCamera {
 public:
  /** Returns false when the camera cannot start: no LEDC timer is left for its clock, or the driver fails to init. */
  bool begin();
  void capture(EspCameraPictureReader reader);
};
#endif
