#pragma once

#include "serial_settings.h"

#if defined(ESP8266) || defined(ESP32)
/**
 * Keeps settings in the EEPROM, which the sketch has begun with room for size() bytes from `offset`, as one record:
 * `magic` and `version`, then each field in order, a text as `maxLength + 1` bytes padded with zeros and empty while
 * absent, a number as 4 bytes and NaN while absent. A record with another magic or version, such as an empty EEPROM
 * or another firmware's data, holds nothing. Every save writes the whole record and commits it.
 */
class EepromSettingsStore : public SettingsStore {
 private:
  size_t offset;
  uint8_t magic, version;

 public:
  EepromSettingsStore(size_t offset, uint8_t magic, uint8_t version);

  /** The bytes the record of these fields takes. */
  static constexpr size_t size(const SettingField* fields, uint8_t count) {
    return count == 0 ? 2 : size(fields, count - 1) + sizeOf(fields[count - 1]);
  }

  /** The bytes one field takes in the record. */
  static constexpr size_t sizeOf(const SettingField& field) {
    return field.kind == SettingKind::Number ? sizeof(float) : field.maxLength + 1;
  }

  void load(SerialSettings& settings) override;
  bool save(const SerialSettings& settings, uint8_t field) override;
};
#endif

#ifdef ESP32
/**
 * Keeps settings in the ESP32's NVS through Preferences, one entry per field under `name`, at the key `keys` gives
 * for it. NVS keys are at most 15 characters, which a protocol key may exceed, hence keys of their own. It logs a value
 * it could not read or save.
 */
class NvsSettingsStore : public SettingsStore {
 private:
  const char* name;
  const char* const* keys;

 public:
  /** `keys` holds one NVS key per field and must outlive the store. */
  NvsSettingsStore(const char* name, const char* const* keys);

  void load(SerialSettings& settings) override;
  bool save(const SerialSettings& settings, uint8_t field) override;
};
#endif
