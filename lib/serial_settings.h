#pragma once

#include <stddef.h>
#include <stdint.h>

/** How a setting's value is kept and shown: text, text that `cfg get` hides, or a number. */
enum class SettingKind : uint8_t { Text, Secret, Number };

/**
 * One value a sketch lets the serial port change, with its limits and the default it runs with while the board stores
 * none. A text holds `minLength` to `maxLength` characters; a number lies from `low` to `high`, with `low` itself
 * refused when `lowExclusive`.
 */
struct SettingField {
  const char* key;
  SettingKind kind;
  size_t minLength, maxLength;
  float low, high;
  bool lowExclusive;
  const char* defaultText;
  float defaultNumber;

  static constexpr SettingField text(const char* key, size_t minLength, size_t maxLength, const char* defaultText) {
    return {key, SettingKind::Text, minLength, maxLength, 0.0f, 0.0f, false, defaultText, 0.0f};
  }
  static constexpr SettingField secret(const char* key, size_t minLength, size_t maxLength, const char* defaultText) {
    return {key, SettingKind::Secret, minLength, maxLength, 0.0f, 0.0f, false, defaultText, 0.0f};
  }
  static constexpr SettingField number(const char* key, float low, float high, float defaultNumber,
                                       bool lowExclusive = false) {
    return {key, SettingKind::Number, 0, 0, low, high, lowExclusive, nullptr, defaultNumber};
  }
};

/** The most fields one SerialSettings holds. */
static const uint8_t SETTINGS_FIELDS_MAX = 8;
/** The longest text a field may hold. */
static const size_t SETTING_TEXT_MAX = 63;
/** The room SerialSettings::handle() needs for its reply. */
static const size_t SETTINGS_REPLY_MAX = 768;

class SerialSettings;

/** Where a SerialSettings keeps its values across restarts, such as a board's flash. */
class SettingsStore {
 public:
  /** Puts every value it holds into `settings`, through SerialSettings::restoreText() and restoreNumber(). */
  virtual void load(SerialSettings& settings) = 0;
  /** Keeps the value `settings` holds for `field`. Returns false when it could not. */
  virtual bool save(const SerialSettings& settings, uint8_t field) = 0;
};

/**
 * A refusal reason for a number a set would store in `field`, after the field's own limits passed, or nullptr to accept
 * it. It sees the other fields' values through `settings`.
 */
using SettingCheck = const char* (*)(const SerialSettings& settings, uint8_t field, float value);

/** What the sketch does after a line: nothing, or restart the board so new values take effect. */
enum class SettingsAction { None, Restart };

/**
 * The settings a sketch lets the serial port change, each stored on the board or falling back to its field's default,
 * and the `cfg` protocol the website speaks to change them:
 *
 *   cfg set <key> <value>  ->  "cfg ok <key>" or "cfg err <key> <reason>"; the value is the rest of the line.
 *                              A changed value, or one stored for the first time, is saved through the store.
 *   cfg get                ->  "cfg <key>=<value>" per field in order, a secret as "set" or "unset", then "cfg end"
 *   cfg restart            ->  an empty reply and SettingsAction::Restart
 *   anything else          ->  "cfg err - syntax"
 *
 * A line as long as LineReader keeps, which it may have cut short, is a syntax error too. A text shows as it is, a
 * secret as whether it is stored, and a number with %g, which round-trips a value typed with three decimals.
 *
 * The reasons, in the order a set checks them: the one handle() was given to refuse every set; "unknown", no such key;
 * for a text, "length" outside its limits; for a secret, "too short" or "too long"; for a number, "number" when the
 * value is not a finite decimal, "range" outside its limits, then whatever the SettingCheck returns. A refused set
 * leaves the stored value as it was.
 */
class SerialSettings {
 public:
  /** `fields` must outlive the object; past SETTINGS_FIELDS_MAX, the rest are ignored. */
  SerialSettings(const SettingField* fields, uint8_t count, SettingsStore& store, SettingCheck check = nullptr);

  /** Loads what the store holds; a value outside its field's limits stays absent. */
  void begin();

  /** Whether a line belongs to the protocol: it is "cfg" or starts with "cfg ". */
  static bool isLine(const char* line);

  /**
   * Runs one line and writes the reply, each line ending in '\n', into `reply`, up to `n` bytes (SETTINGS_REPLY_MAX
   * is enough). A line that is not the protocol's leaves the reply empty. While `refuseSets` is not null, every set
   * is refused with it as the reason, such as while saving would stall a control loop.
   */
  SettingsAction handle(const char* line, char* reply, size_t n, const char* refuseSets = nullptr);

  uint8_t count() const;
  const SettingField& field(uint8_t field) const;
  bool isStored(uint8_t field) const;

  /** The value the sketch runs with: the stored one, else the field's default. */
  const char* text(uint8_t field) const;
  float number(uint8_t field) const;

  /**
   * Puts a value read back from a store. A value outside its field's limits is refused and the field stays absent,
   * so a corrupt entry cannot reach the sketch. Returns whether the value was kept.
   */
  bool restoreText(uint8_t field, const char* value);
  bool restoreNumber(uint8_t field, float value);

 private:
  struct Value {
    char text[SETTING_TEXT_MAX + 1];
    float number;
    bool stored;
  };

  const SettingField* fields;
  uint8_t fieldCount;
  SettingsStore& store;
  SettingCheck check;
  Value values[SETTINGS_FIELDS_MAX] = {};
};
