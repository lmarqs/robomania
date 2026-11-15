#include <host_test.h>
#include <line_reader.h>
#include <serial_settings.h>

#include <cmath>
#include <string>
#include <vector>

/** Keeps what was saved in memory, so a test can see each save and load it back into a new SerialSettings. */
class MemoryStore : public SettingsStore {
 public:
  std::vector<std::string> texts = std::vector<std::string>(SETTINGS_FIELDS_MAX);
  std::vector<float> numbers = std::vector<float>(SETTINGS_FIELDS_MAX, NAN);
  std::vector<bool> held = std::vector<bool>(SETTINGS_FIELDS_MAX, false);
  int saves = 0;

  void load(SerialSettings& settings) override {
    for (uint8_t i = 0; i < settings.count(); i++) {
      if (!held[i]) {
        continue;
      }
      if (settings.field(i).kind == SettingKind::Number) {
        settings.restoreNumber(i, numbers[i]);
      } else {
        settings.restoreText(i, texts[i].c_str());
      }
    }
  }

  bool save(const SerialSettings& settings, uint8_t field) override {
    saves++;
    held[field] = true;
    if (settings.field(field).kind == SettingKind::Number) {
      numbers[field] = settings.number(field);
    } else {
      texts[field] = settings.text(field);
    }
    return true;
  }
};

enum Field : uint8_t { NAME, PASSWORD, OFFSET, BAND_LOW, BAND_HIGH, LIMIT, FIELD_COUNT };

static const SettingField FIELDS[FIELD_COUNT] = {
    SettingField::text("name", 1, 32, "robot"),          SettingField::secret("password", 8, 63, "default-pass"),
    SettingField::number("offset", -30.0f, 30.0f, 1.5f), SettingField::number("low", 0.0f, 1.0f, 0.2f),
    SettingField::number("high", 0.0f, 1.0f, 0.8f),      SettingField::number("limit", 0.0f, 90.0f, 45.0f, true),
};

/** Refuses a low above the high, or a high below the low, against the other's current value. */
static const char* lowBelowHigh(const SerialSettings& settings, uint8_t field, float value) {
  const float low = field == BAND_LOW ? value : settings.number(BAND_LOW);
  const float high = field == BAND_HIGH ? value : settings.number(BAND_HIGH);
  return low <= high ? nullptr : "order";
}

struct Fixture {
  MemoryStore store;
  SerialSettings settings{FIELDS, FIELD_COUNT, store, lowBelowHigh};
  char reply[SETTINGS_REPLY_MAX];

  Fixture() { settings.begin(); }

  std::string run(const char* line, const char* refuseSets = nullptr) {
    settings.handle(line, reply, sizeof(reply), refuseSets);
    return reply;
  }
};

void test_get_lists_defaults_with_the_secret_unset(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING(
      "cfg name=robot\ncfg password=unset\ncfg offset=1.5\ncfg low=0.2\ncfg high=0.8\ncfg limit=45\ncfg end\n",
      f.run("cfg get").c_str());
}

void test_set_stores_saves_and_shows_the_value(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg ok name\n", f.run("cfg set name my robot").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg ok password\n", f.run("cfg set password 12345678").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg ok offset\n", f.run("cfg set offset -2.125").c_str());
  TEST_ASSERT_TRUE(f.store.saves == 3);
  TEST_ASSERT_EQUAL_STRING(
      "cfg name=my robot\ncfg password=set\ncfg offset=-2.125\ncfg low=0.2\ncfg high=0.8\ncfg limit=45\ncfg end\n",
      f.run("cfg get").c_str());
}

void test_setting_the_same_value_saves_once(void) {
  Fixture f;
  f.run("cfg set name robot");
  f.run("cfg set name robot");
  f.run("cfg set low 0.2");
  f.run("cfg set low 0.2");
  TEST_ASSERT_TRUE(f.store.saves == 2);
}

void test_a_value_equal_to_the_default_is_stored(void) {
  Fixture f;
  f.run("cfg set name robot");
  TEST_ASSERT_TRUE(f.settings.isStored(NAME));
}

void test_text_limits_give_length(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg err name length\n", f.run("cfg set name ").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err name length\n", f.run(("cfg set name " + std::string(33, 'x')).c_str()).c_str());
  TEST_ASSERT_FALSE(f.settings.isStored(NAME));
}

void test_secret_limits_give_too_short_and_too_long(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg err password too short\n", f.run("cfg set password 1234567").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err password too long\n",
                           f.run(("cfg set password " + std::string(64, 'x')).c_str()).c_str());
}

void test_numbers_refuse_what_is_not_a_finite_decimal(void) {
  Fixture f;
  const char* lines[] = {"cfg set offset ",    "cfg set offset abc", "cfg set offset 0x10", "cfg set offset nan",
                         "cfg set offset inf", "cfg set offset 1 ",  "cfg set offset 1e99"};
  for (const char* line : lines) {
    TEST_ASSERT_EQUAL_STRING("cfg err offset number\n", f.run(line).c_str());
  }
  TEST_ASSERT_TRUE(f.store.saves == 0);
}

void test_numbers_outside_their_limits_give_range(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg err offset range\n", f.run("cfg set offset 30.5").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg ok offset\n", f.run("cfg set offset -30").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err limit range\n", f.run("cfg set limit 0").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg ok limit\n", f.run("cfg set limit 90").c_str());
}

void test_the_check_refuses_after_the_limits(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg err low order\n", f.run("cfg set low 0.9").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err low range\n", f.run("cfg set low 2").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg ok high\n", f.run("cfg set high 1").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg ok low\n", f.run("cfg set low 0.9").c_str());
}

void test_refused_sets_name_the_reason_before_the_key_is_checked(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg err name armed\n", f.run("cfg set name x", "armed").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err nope armed\n", f.run("cfg set nope x", "armed").c_str());
  TEST_ASSERT_TRUE(f.run("cfg get", "armed").find("cfg end\n") != std::string::npos);
  TEST_ASSERT_TRUE(f.store.saves == 0);
}

void test_unknown_key_and_syntax_errors(void) {
  Fixture f;
  TEST_ASSERT_EQUAL_STRING("cfg err nope unknown\n", f.run("cfg set nope 1").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err - syntax\n", f.run("cfg").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err - syntax\n", f.run("cfg set name").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err - syntax\n", f.run("cfg set  x").c_str());
  TEST_ASSERT_EQUAL_STRING("cfg err - syntax\n", f.run("cfg get extra").c_str());
  const std::string longLine = "cfg set name " + std::string(LineReader::CAPACITY, 'x');
  TEST_ASSERT_EQUAL_STRING("cfg err - syntax\n", f.run(longLine.c_str()).c_str());
}

void test_other_lines_are_not_the_protocols(void) {
  Fixture f;
  TEST_ASSERT_FALSE(SerialSettings::isLine("cfgx"));
  TEST_ASSERT_FALSE(SerialSettings::isLine("kp 1"));
  TEST_ASSERT_TRUE(SerialSettings::isLine("cfg"));
  TEST_ASSERT_TRUE(SerialSettings::isLine("cfg get"));
  TEST_ASSERT_EQUAL_STRING("", f.run("kp 1").c_str());
}

void test_restart_asks_for_a_restart_with_no_reply(void) {
  Fixture f;
  TEST_ASSERT_TRUE(f.settings.handle("cfg restart", f.reply, sizeof(f.reply)) == SettingsAction::Restart);
  TEST_ASSERT_EQUAL_STRING("", f.reply);
  TEST_ASSERT_TRUE(f.settings.handle("cfg get", f.reply, sizeof(f.reply)) == SettingsAction::None);
}

void test_begin_loads_what_the_store_holds_and_drops_what_breaks_the_limits(void) {
  MemoryStore store;
  store.held[NAME] = true;
  store.texts[NAME] = "kept";
  store.held[PASSWORD] = true;
  store.texts[PASSWORD] = "short";
  store.held[OFFSET] = true;
  store.numbers[OFFSET] = 99.0f;
  store.held[BAND_LOW] = true;
  store.numbers[BAND_LOW] = NAN;
  store.held[BAND_HIGH] = true;
  store.numbers[BAND_HIGH] = 0.5f;
  SerialSettings settings(FIELDS, FIELD_COUNT, store);
  settings.begin();
  TEST_ASSERT_EQUAL_STRING("kept", settings.text(NAME));
  TEST_ASSERT_FALSE(settings.isStored(PASSWORD));
  TEST_ASSERT_EQUAL_STRING("default-pass", settings.text(PASSWORD));
  TEST_ASSERT_FALSE(settings.isStored(OFFSET));
  TEST_ASSERT_FLOAT_WITHIN(0.0f, 1.5f, settings.number(OFFSET));
  TEST_ASSERT_FALSE(settings.isStored(BAND_LOW));
  TEST_ASSERT_FLOAT_WITHIN(0.0f, 0.5f, settings.number(BAND_HIGH));
}

void test_a_saved_value_survives_a_restart(void) {
  MemoryStore store;
  {
    SerialSettings settings(FIELDS, FIELD_COUNT, store);
    settings.begin();
    char reply[SETTINGS_REPLY_MAX];
    settings.handle("cfg set limit 12.5", reply, sizeof(reply));
  }
  SerialSettings settings(FIELDS, FIELD_COUNT, store);
  settings.begin();
  TEST_ASSERT_FLOAT_WITHIN(0.0f, 12.5f, settings.number(LIMIT));
}

void test_restore_refuses_the_wrong_kind(void) {
  Fixture f;
  TEST_ASSERT_FALSE(f.settings.restoreNumber(NAME, 1.0f));
  TEST_ASSERT_FALSE(f.settings.restoreText(OFFSET, "1"));
  TEST_ASSERT_FALSE(f.settings.restoreText(FIELD_COUNT, "x"));
}

void test_a_short_reply_buffer_stays_terminated(void) {
  Fixture f;
  char reply[10];
  f.settings.handle("cfg get", reply, sizeof(reply));
  TEST_ASSERT_EQUAL_STRING("cfg name=", reply);
}

int main(int, char**) {
  UNITY_BEGIN();
  RUN_TEST(test_get_lists_defaults_with_the_secret_unset);
  RUN_TEST(test_set_stores_saves_and_shows_the_value);
  RUN_TEST(test_setting_the_same_value_saves_once);
  RUN_TEST(test_a_value_equal_to_the_default_is_stored);
  RUN_TEST(test_text_limits_give_length);
  RUN_TEST(test_secret_limits_give_too_short_and_too_long);
  RUN_TEST(test_numbers_refuse_what_is_not_a_finite_decimal);
  RUN_TEST(test_numbers_outside_their_limits_give_range);
  RUN_TEST(test_the_check_refuses_after_the_limits);
  RUN_TEST(test_refused_sets_name_the_reason_before_the_key_is_checked);
  RUN_TEST(test_unknown_key_and_syntax_errors);
  RUN_TEST(test_other_lines_are_not_the_protocols);
  RUN_TEST(test_restart_asks_for_a_restart_with_no_reply);
  RUN_TEST(test_begin_loads_what_the_store_holds_and_drops_what_breaks_the_limits);
  RUN_TEST(test_a_saved_value_survives_a_restart);
  RUN_TEST(test_restore_refuses_the_wrong_kind);
  RUN_TEST(test_a_short_reply_buffer_stays_terminated);
  return UNITY_END();
}
