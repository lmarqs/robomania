#include "settings_store.h"

#if defined(ESP8266) || defined(ESP32)
#include <Arduino.h>
#include <EEPROM.h>
#include <math.h>
#include <string.h>

EepromSettingsStore::EepromSettingsStore(size_t offset, uint8_t magic, uint8_t version) {
  this->offset = offset;
  this->magic = magic;
  this->version = version;
}

void EepromSettingsStore::load(SerialSettings& settings) {
  if (EEPROM.read(offset) != magic || EEPROM.read(offset + 1) != version) {
    return;
  }
  size_t at = offset + 2;
  for (uint8_t i = 0; i < settings.count(); i++) {
    const SettingField& field = settings.field(i);
    if (field.kind == SettingKind::Number) {
      float value = NAN;
      EEPROM.get(at, value);
      settings.restoreNumber(i, value);
    } else if (field.maxLength <= SETTING_TEXT_MAX) {
      char text[SETTING_TEXT_MAX + 1];
      for (size_t c = 0; c <= field.maxLength; c++) {
        text[c] = static_cast<char>(EEPROM.read(at + c));
      }
      if (text[0] != '\0') {
        settings.restoreText(i, text);
      }
    }
    at += sizeOf(field);
  }
}

bool EepromSettingsStore::save(const SerialSettings& settings, uint8_t) {
  EEPROM.write(offset, magic);
  EEPROM.write(offset + 1, version);
  size_t at = offset + 2;
  for (uint8_t i = 0; i < settings.count(); i++) {
    const SettingField& field = settings.field(i);
    if (field.kind == SettingKind::Number) {
      EEPROM.put(at, settings.isStored(i) ? settings.number(i) : NAN);
    } else {
      const char* text = settings.isStored(i) ? settings.text(i) : "";
      const size_t length = strlen(text);
      for (size_t c = 0; c <= field.maxLength; c++) {
        EEPROM.write(at + c, c < length ? static_cast<uint8_t>(text[c]) : 0);
      }
    }
    at += sizeOf(field);
  }
  return EEPROM.commit();
}
#endif

#ifdef ESP32
#include <Preferences.h>

NvsSettingsStore::NvsSettingsStore(const char* name, const char* const* keys) {
  this->name = name;
  this->keys = keys;
}

void NvsSettingsStore::load(SerialSettings& settings) {
  Preferences prefs;
  if (!prefs.begin(name)) {
    log_e("the stored settings are unreadable, so the defaults apply");
    return;
  }
  for (uint8_t i = 0; i < settings.count(); i++) {
    if (!prefs.isKey(keys[i])) {
      continue;
    }
    bool kept;
    if (settings.field(i).kind == SettingKind::Number) {
      kept = settings.restoreNumber(i, prefs.getFloat(keys[i], NAN));
    } else {
      // One byte past the longest text, so a stored text too long for its field reads as too long.
      char text[SETTING_TEXT_MAX + 2];
      text[0] = '\0';
      prefs.getString(keys[i], text, sizeof(text));
      kept = settings.restoreText(i, text);
    }
    if (!kept) {
      log_w("the stored %s is invalid and ignored", settings.field(i).key);
    }
  }
  prefs.end();
}

bool NvsSettingsStore::save(const SerialSettings& settings, uint8_t field) {
  Preferences prefs;
  bool saved = prefs.begin(name);
  if (saved) {
    saved = settings.field(field).kind == SettingKind::Number ? prefs.putFloat(keys[field], settings.number(field)) > 0
                                                              : prefs.putString(keys[field], settings.text(field)) > 0;
    prefs.end();
  }
  if (!saved) {
    log_e("the %s could not be saved", settings.field(field).key);
  }
  return saved;
}
#endif
