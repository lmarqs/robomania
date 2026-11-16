#pragma once

#ifdef ESP32
#include <Arduino.h>
#include <FS.h>
#ifndef CONFIG_HTTPD_WS_SUPPORT
#define CONFIG_HTTPD_WS_SUPPORT 1
#endif
#include <esp_http_server.h>

using Esp32WebServerFrameHandler = std::function<void(httpd_ws_frame_t* frame)>;
using Esp32WebServerBodyHandler = std::function<void(uint8_t* buf, size_t len)>;

class Esp32WebServerRequest {
 private:
  httpd_req_t* req;

 public:
  Esp32WebServerRequest(httpd_req_t* req);

  int method();

  esp_err_t frame(Esp32WebServerFrameHandler handler);

  esp_err_t body(Esp32WebServerBodyHandler handler);
};

class Esp32WebServerResponse {
 private:
  httpd_req_t* req;
  esp_err_t err;

 public:
  Esp32WebServerResponse(httpd_req_t* req);

  esp_err_t error();

  void reset();

  void fail();

  void fail(esp_err_t err);

  bool isConnected();

  void setStatus(const char* status);

  void setType(const char* type);

  void setContentLength(size_t len);

  void setHeader(const char* key, const char* value);

  void send(const char* status, const char* type, const uint8_t* buf, size_t len);

  void send(const char* status, const char* type, FS fs, const char* path);

  void send(const char* buf);

  void send(const uint8_t* buf, size_t len);

  void send(httpd_ws_frame_t* buf);
};

using Esp32WebServerHandler = std::function<void(Esp32WebServerRequest* req, Esp32WebServerResponse* res)>;

class Esp32WebServer {
 private:
  httpd_handle_t httpd = NULL;

 public:
  /** Starts the server on `port`. Returns false when it cannot start, such as when the port is taken. */
  bool begin(int port);

  void on(const char* uri, httpd_method_t method, Esp32WebServerHandler handler);
};
#endif
