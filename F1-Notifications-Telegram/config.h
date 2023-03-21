#define F1_CONFIG_JSON "/f1_notification_config.json"

#define F1_TIME_ZONE_LABEL "timeZone"
#define F1_TIME_FORMAT_LABEL "timeFormat"
#define F1_BOT_TOKEN_LABEL "botToken"
#define F1_CHAT_ID_LABEL "chatId"
#define F1_ROUND_OFFSET_LABEL "roundOffset"
#define F1_CURRENT_RACE_NOTIFICATION_LABEL "currentRaceNotification"

//How the time will be displayed, see here for more info: https://github.com/ropg/ezTime#datetime
String timeFormat = "l, d-M-Y H:i"; // Friday, 17-Mar-2023 00:30
String timeZone = "Europe/London"; //seems to be something wrong with Europe/Dublin

// Telegram BOT Token (Get from Botfather)
String botToken = "245123272:AAFMkbubXUds_PZSVUV_rEfphoaMs170k5o";

// Use @myidbot (IDBot) to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
String chatId = "175753396";

int roundOffset = 0;

bool currentRaceNotification = false;

bool fetchConfigFile() {
  if (SPIFFS.exists(F1_CONFIG_JSON)) {
    //file exists, reading and loading
    Serial.println("reading config file");
    File configFile = SPIFFS.open(F1_CONFIG_JSON, "r");
    if (configFile) {
      Serial.println("opened config file");
      StaticJsonDocument<1024> json;
      DeserializationError error = deserializeJson(json, configFile);
      serializeJsonPretty(json, Serial);
      if (!error) {
        Serial.println("\nparsed json");

        if (json.containsKey(F1_TIME_ZONE_LABEL)) {
          timeZone = String(json[F1_TIME_ZONE_LABEL].as<String>());
        }

        if (json.containsKey(F1_TIME_FORMAT_LABEL)) {
          timeFormat = String(json[F1_TIME_FORMAT_LABEL].as<String>());
        }

        if (json.containsKey(F1_BOT_TOKEN_LABEL)) {
          botToken = String(json[F1_BOT_TOKEN_LABEL].as<String>());
        }

        if (json.containsKey(F1_CHAT_ID_LABEL)) {
          chatId = String(json[F1_CHAT_ID_LABEL].as<String>());
        }

        if (json.containsKey(F1_ROUND_OFFSET_LABEL)) {
          roundOffset = json[F1_ROUND_OFFSET_LABEL].as<int>();
        }

        if (json.containsKey(F1_CURRENT_RACE_NOTIFICATION_LABEL)) {
          currentRaceNotification = json[F1_CURRENT_RACE_NOTIFICATION_LABEL].as<bool>();
        }

        return true;

      } else {
        Serial.println("failed to load json config");
        return false;
      }
    }
  } else {
    Serial.println("Config file does not exist");
    return false;
  }
}

bool saveConfigFile() {
  Serial.println(F("Saving config"));
  StaticJsonDocument<1024> json;
  json[F1_TIME_ZONE_LABEL] = timeZone;
  json[F1_TIME_FORMAT_LABEL] = timeFormat;
  json[F1_BOT_TOKEN_LABEL] = botToken;
  json[F1_CHAT_ID_LABEL] = chatId;
  json[F1_ROUND_OFFSET_LABEL] = roundOffset;
  json[F1_CURRENT_RACE_NOTIFICATION_LABEL] = currentRaceNotification;

  File configFile = SPIFFS.open(F1_CONFIG_JSON, "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
    return false;
  }

  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0) {
    Serial.println(F("Failed to write to file"));
    return false;
  }
  configFile.close();
  return true;
}
