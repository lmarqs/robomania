#include "serial_settings.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "line_reader.h"

/** Writes lines into a caller's buffer and keeps it terminated, dropping what does not fit. */
class Reply {
 public:
  Reply(char* buffer, size_t size) : buffer_(buffer), size_(size) {
    if (size_ > 0) {
      buffer_[0] = '\0';
    }
  }

  void append(const char* format, ...) __attribute__((format(printf, 2, 3))) {
    if (used_ + 1 >= size_) {
      return;
    }
    va_list args;
    va_start(args, format);
    const int written = vsnprintf(buffer_ + used_, size_ - used_, format, args);
    va_end(args);
    if (written > 0) {
      used_ += static_cast<size_t>(written);
      if (used_ >= size_) {
        used_ = size_ - 1;
      }
    }
  }

 private:
  char* buffer_;
  size_t size_;
  size_t used_ = 0;
};

static bool isText(const SettingField& spec) { return spec.kind != SettingKind::Number; }

/** The reason a text of this length is refused, or nullptr when the field accepts it. */
static const char* textRefusal(const SettingField& spec, size_t length) {
  if (spec.kind == SettingKind::Text) {
    return (length < spec.minLength || length > spec.maxLength) ? "length" : nullptr;
  }
  if (length < spec.minLength) {
    return "too short";
  }
  return length > spec.maxLength ? "too long" : nullptr;
}

static bool inRange(const SettingField& spec, float value) {
  return (spec.lowExclusive ? value > spec.low : value >= spec.low) && value <= spec.high;
}

/** Reads a decimal number that fills the whole text, without the spaces, hex, "nan" and "inf" strtof would take. */
static bool parseNumber(const char* text, float& value) {
  if (text[0] == '\0' || strspn(text, "0123456789+-.eE") != strlen(text)) {
    return false;
  }
  char* end = nullptr;
  const float parsed = strtof(text, &end);
  if (*end != '\0' || !isfinite(parsed)) {
    return false;
  }
  value = parsed == 0.0f ? 0.0f : parsed;
  return true;
}

SerialSettings::SerialSettings(const SettingField* fields, uint8_t count, SettingsStore& store, SettingCheck check)
    : fields(fields),
      fieldCount(count < SETTINGS_FIELDS_MAX ? count : SETTINGS_FIELDS_MAX),
      store(store),
      check(check) {}

void SerialSettings::begin() { store.load(*this); }

bool SerialSettings::isLine(const char* line) {
  return strncmp(line, "cfg", 3) == 0 && (line[3] == '\0' || line[3] == ' ');
}

uint8_t SerialSettings::count() const { return fieldCount; }

const SettingField& SerialSettings::field(uint8_t field) const { return fields[field]; }

bool SerialSettings::isStored(uint8_t field) const { return values[field].stored; }

const char* SerialSettings::text(uint8_t field) const {
  return values[field].stored ? values[field].text : fields[field].defaultText;
}

float SerialSettings::number(uint8_t field) const {
  return values[field].stored ? values[field].number : fields[field].defaultNumber;
}

bool SerialSettings::restoreText(uint8_t field, const char* value) {
  if (field >= fieldCount) {
    return false;
  }
  const SettingField& spec = fields[field];
  const size_t length = strnlen(value, spec.maxLength + 1);
  if (!isText(spec) || spec.maxLength > SETTING_TEXT_MAX || textRefusal(spec, length) != nullptr) {
    return false;
  }
  Value& slot = values[field];
  memset(slot.text, 0, sizeof(slot.text));
  memcpy(slot.text, value, length);
  slot.stored = true;
  return true;
}

bool SerialSettings::restoreNumber(uint8_t field, float value) {
  if (field >= fieldCount) {
    return false;
  }
  const SettingField& spec = fields[field];
  if (isText(spec) || !isfinite(value) || !inRange(spec, value)) {
    return false;
  }
  values[field].number = value;
  values[field].stored = true;
  return true;
}

static void answerSyntaxError(Reply& reply) { reply.append("cfg err - syntax\n"); }

static void answerGet(const SerialSettings& settings, Reply& reply) {
  for (uint8_t i = 0; i < settings.count(); i++) {
    const SettingField& spec = settings.field(i);
    switch (spec.kind) {
      case SettingKind::Text:
        reply.append("cfg %s=%s\n", spec.key, settings.text(i));
        break;
      case SettingKind::Secret:
        reply.append("cfg %s=%s\n", spec.key, settings.isStored(i) ? "set" : "unset");
        break;
      case SettingKind::Number:
        reply.append("cfg %s=%g\n", spec.key, static_cast<double>(settings.number(i)));
        break;
    }
  }
  reply.append("cfg end\n");
}

static bool findField(const SerialSettings& settings, const char* key, size_t keyLength, uint8_t& field) {
  for (uint8_t i = 0; i < settings.count(); i++) {
    const char* name = settings.field(i).key;
    if (strlen(name) == keyLength && strncmp(name, key, keyLength) == 0) {
      field = i;
      return true;
    }
  }
  return false;
}

SettingsAction SerialSettings::handle(const char* line, char* reply, size_t n, const char* refuseSets) {
  Reply out(reply, n);
  if (!isLine(line)) {
    return SettingsAction::None;
  }
  if (strlen(line) >= LineReader::CAPACITY - 1) {
    answerSyntaxError(out);
    return SettingsAction::None;
  }

  const char* args = line + 3;
  if (strcmp(args, " get") == 0) {
    answerGet(*this, out);
    return SettingsAction::None;
  }
  if (strcmp(args, " restart") == 0) {
    return SettingsAction::Restart;
  }
  if (strncmp(args, " set ", 5) != 0) {
    answerSyntaxError(out);
    return SettingsAction::None;
  }

  // What follows "cfg set " is the key, one space, then the value, which may hold spaces or be empty.
  const char* key = args + 5;
  const char* separator = strchr(key, ' ');
  if (separator == nullptr || separator == key) {
    answerSyntaxError(out);
    return SettingsAction::None;
  }
  const size_t keyLength = static_cast<size_t>(separator - key);
  if (refuseSets != nullptr) {
    out.append("cfg err %.*s %s\n", static_cast<int>(keyLength), key, refuseSets);
    return SettingsAction::None;
  }

  uint8_t field;
  if (!findField(*this, key, keyLength, field)) {
    out.append("cfg err %.*s unknown\n", static_cast<int>(keyLength), key);
    return SettingsAction::None;
  }

  const SettingField& spec = fields[field];
  const char* value = separator + 1;
  Value& slot = values[field];
  bool changed;
  if (isText(spec)) {
    const char* refusal = textRefusal(spec, strlen(value));
    if (refusal != nullptr) {
      out.append("cfg err %s %s\n", spec.key, refusal);
      return SettingsAction::None;
    }
    changed = !slot.stored || strcmp(slot.text, value) != 0;
    if (changed) {
      restoreText(field, value);
    }
  } else {
    float number;
    if (!parseNumber(value, number)) {
      out.append("cfg err %s number\n", spec.key);
      return SettingsAction::None;
    }
    if (!inRange(spec, number)) {
      out.append("cfg err %s range\n", spec.key);
      return SettingsAction::None;
    }
    const char* refusal = check != nullptr ? check(*this, field, number) : nullptr;
    if (refusal != nullptr) {
      out.append("cfg err %s %s\n", spec.key, refusal);
      return SettingsAction::None;
    }
    changed = !slot.stored || slot.number != number;
    if (changed) {
      restoreNumber(field, number);
    }
  }

  out.append("cfg ok %s\n", spec.key);
  if (changed) {
    store.save(*this, field);
  }
  return SettingsAction::None;
}
