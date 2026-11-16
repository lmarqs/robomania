#include "esp32_web_server.h"

#ifdef ESP32
#include <Arduino.h>
#include <FS.h>
#include <esp_http_server.h>

Esp32WebServerRequest::Esp32WebServerRequest(httpd_req_t* req) : req(req) {}

int Esp32WebServerRequest::method() { return req->method; }

esp_err_t Esp32WebServerRequest::frame(Esp32WebServerFrameHandler handler) {
  esp_err_t ret;

  httpd_ws_frame_t frame;

  memset(&frame, 0, sizeof(httpd_ws_frame_t));

  ret = httpd_ws_recv_frame(req, &frame, 0);

  if (ret != ESP_OK) {
    return ret;
  }

  if (!frame.len) {
    handler(&frame);
    return ret;
  }

  uint8_t* buf = (uint8_t*)calloc(1, frame.len + 1);

  if (!buf) {
    return ESP_ERR_NO_MEM;
  }

  frame.payload = buf;

  ret = httpd_ws_recv_frame(req, &frame, frame.len);

  if (ret == ESP_OK) {
    handler(&frame);
  }

  free(buf);
  return ret;
}

esp_err_t Esp32WebServerRequest::body(Esp32WebServerBodyHandler handler) {
  uint8_t* buf = (uint8_t*)calloc(1, req->content_len + 1);

  if (!buf) {
    return ESP_ERR_NO_MEM;
  }

  // httpd_req_recv() returns what has arrived so far, which can be less than the whole body.
  size_t received = 0;
  while (received < req->content_len) {
    const int ret = httpd_req_recv(req, (char*)buf + received, req->content_len - received);
    if (ret <= 0) {
      free(buf);
      return ESP_FAIL;
    }
    received += ret;
  }

  handler(buf, req->content_len);

  free(buf);

  return ESP_OK;
}


Esp32WebServerResponse::Esp32WebServerResponse(httpd_req_t* req) : req(req), err(ESP_OK) {}

esp_err_t Esp32WebServerResponse::error() { return err; }

void Esp32WebServerResponse::reset() { err = ESP_OK; }

void Esp32WebServerResponse::fail() { err = ESP_FAIL; }

void Esp32WebServerResponse::fail(esp_err_t reason) { err = reason; }

bool Esp32WebServerResponse::isConnected() { return err == ESP_OK; }

void Esp32WebServerResponse::setStatus(const char* status) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_status(req, status);
}

void Esp32WebServerResponse::setType(const char* type) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_type(req, type);
}

void Esp32WebServerResponse::setContentLength(size_t len) { req->content_len = len; }

void Esp32WebServerResponse::setHeader(const char* key, const char* value) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_set_hdr(req, key, value);
}

void Esp32WebServerResponse::send(const char* status, const char* type, const uint8_t* buf, size_t len) {
  this->setStatus(status);

  this->setType(type);

  err = httpd_resp_send(req, (char*)buf, len);
}

void Esp32WebServerResponse::send(const char* status, const char* type, FS fs, const char* path) {
  this->setStatus(status);

  this->setType(type);

  File file = fs.open(path, "r");

  this->setContentLength(file.size());

  uint8_t buf[128];

  size_t len;

  while (file.available() && err == ESP_OK) {
    len = file.read(buf, 128);

    this->send(buf, len);
  }

  this->send(NULL, 0);

  file.close();
}

void Esp32WebServerResponse::send(const uint8_t* buf, size_t len) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_send_chunk(req, (char*)buf, len);
}

void Esp32WebServerResponse::send(const char* buf) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_resp_send_chunk(req, buf, strlen(buf));
}

void Esp32WebServerResponse::send(httpd_ws_frame_t* frame) {
  if (err != ESP_OK) {
    return;
  }

  err = httpd_ws_send_frame(req, frame);
}

bool Esp32WebServer::begin(int port) {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  config.server_port = port;
  config.ctrl_port = 32000 + port;

  return httpd_start(&httpd, &config) == ESP_OK;
}

void Esp32WebServer::on(const char* uri, httpd_method_t method, Esp32WebServerHandler handler) {
  httpd_uri_t config;

  config.uri = uri;
  config.method = method;

  config.handler = [](httpd_req_t* r) -> esp_err_t {
    Esp32WebServerRequest req(r);
    Esp32WebServerResponse res(r);

    Esp32WebServerHandler* handler = static_cast<Esp32WebServerHandler*>(r->user_ctx);

    (*handler)(&req, &res);

    return res.error();
  };

  config.user_ctx = new Esp32WebServerHandler(handler);

  config.is_websocket = true;

  config.handle_ws_control_frames = false;

  config.supported_subprotocol = NULL;

  httpd_register_uri_handler(httpd, &config);
}
#endif
