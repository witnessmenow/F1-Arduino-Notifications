#ifndef RACELOGIC_H
#define RACELOGIC_H

#define RACE_FILE_NAME "/races.json"

#define RACE_JSON_URL "https://raw.githubusercontent.com/sportstimes/f1/main/_db/f1/2023.json"

time_t nextRaceStartUtc;

Timezone myTZ;
F1Config rl_f1Config;

void raceLogicSetup(F1Config f1Config) {
  rl_f1Config = f1Config;
}

String getConvertedTime(const char* sessionStartTime, const char* timeFormat = "") {
  struct tm tm = {0};
  // Parse date from UTC and convert to an epoch
  strptime(sessionStartTime, "%Y-%m-%dT%H:%M:%S", &tm);
  time_t sessionEpoch = mktime(&tm);

  String timeFormatStr = rl_f1Config.timeFormat;
  if (timeFormat[0] != 0) {
    timeFormatStr = String(timeFormat);
  }
  return myTZ.dateTime(sessionEpoch, UTC_TIME, timeFormatStr);
}

void printConvertedTime(const char* sessionName, const char* sessionStartTime) {

  String timeStr = getConvertedTime(sessionStartTime, "");
  Serial.print(sessionName);
  Serial.print(": ");
  Serial.println(timeStr);

}

const char* sessionCodeToString(const char* sessionCode) {
  if (strcmp(sessionCode, "fp1") == 0)
  {
    return "FP1: ";
  } else if (strcmp(sessionCode, "fp2") == 0)
  {
    return "FP2: ";
  } else if (strcmp(sessionCode, "fp3") == 0)
  {
    return "FP3: ";
  } else if (strcmp(sessionCode, "qualifying") == 0)
  {
    return "Qualifying: ";
  } else if (strcmp(sessionCode, "sprint") == 0)
  {
    return "Sprint: ";
  } else if (strcmp(sessionCode, "sprintQualifying") == 0)
  {
    return "Sprint Qualifying: ";
  } else if (strcmp(sessionCode, "gp") == 0)
  {
    return "Race: ";
  }

  return "UNKNOWN";
}

void printRaceTimes(const char* raceName, JsonObject races_sessions) {
  Serial.print("Next Race: ");
  Serial.println(raceName);

  for (JsonPair kv : races_sessions) {
    printConvertedTime(sessionCodeToString(kv.key().c_str()),
                       kv.value().as<const char*>());
  }

}

String createTelegramMessageString(const char* raceName, JsonObject races_sessions) {
  String message = "Next Race: ";
  message += raceName;
  message += "\n";
  message += "---------------------\n";

  for (JsonPair kv : races_sessions) {
    String sessionName = String(sessionCodeToString(kv.key().c_str()));
    message += sessionName;
    message += getConvertedTime(kv.value().as<const char*>(), "");
    message += "\n";
  }
  return message;
}

bool sendNotificationOfNextRace(UniversalTelegramBot *bot, int offset) {

  StaticJsonDocument<112> filter;

  JsonObject filter_races_0 = filter["races"].createNestedObject();
  filter_races_0["name"] = true;
  filter_races_0["location"] = true;
  filter_races_0["round"] = true;
  filter_races_0["circuitImage"] = true;
  filter_races_0["sessions"] = true;

  File racesJson = SPIFFS.open(RACE_FILE_NAME);
  DynamicJsonDocument doc(12288);

  DeserializationError error = deserializeJson(doc, racesJson, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    racesJson.close();
    return false;
  }
  JsonArray races = doc["races"];

  const char* races_name = races[offset]["name"];
  JsonObject races_sessions = races[offset]["sessions"];


  printRaceTimes(races_name, races_sessions);

  Serial.print("Sending message to ");
  Serial.println(rl_f1Config.chatId);
  racesJson.close();
  return bot->sendPhoto(rl_f1Config.chatId, "https://i.imgur.com/q3qsfSi.png", createTelegramMessageString(races_name, races_sessions));

}

int fetchRaceJson(FileFetcher fileFetcher) {
  // In this example I reuse the same filename
  // over and over
  if (SPIFFS.exists(RACE_FILE_NAME) == true)
  {
    Serial.println("Removing existing image");
    SPIFFS.remove(RACE_FILE_NAME);
  }

  fs::File f = SPIFFS.open(RACE_FILE_NAME, "w+");
  if (!f)
  {
    Serial.println("file open failed");
    return -1;
  }

  bool gotFile = fileFetcher.getFile(RACE_JSON_URL, &f);

  // Make sure to close the file!
  f.close();

  return gotFile;
}

bool getNextRace(int &offset, bool &notificationSent, F1Display* f1Display) {

  StaticJsonDocument<112> filter;

  JsonObject filter_races_0 = filter["races"].createNestedObject();
  filter_races_0["name"] = true;
  filter_races_0["location"] = true;
  filter_races_0["round"] = true;
  filter_races_0["circuitImage"] = true;
  filter_races_0["sessions"] = true;

  File racesJson = SPIFFS.open(RACE_FILE_NAME);
  DynamicJsonDocument doc(12288);

  DeserializationError error = deserializeJson(doc, racesJson, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return false;
  }
  JsonArray races = doc["races"];

  int racesAmount = races.size();
  time_t timeNow = UTC.now();
  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());
  for (int i = offset; i < racesAmount; i++) {
    const char* races_name = races[i]["name"];
    JsonObject races_sessions = races[i]["sessions"];
    const char* race_sessions_gp = races_sessions["gp"]; // "2023-03-05T15:00:00Z"

    struct tm tm = {0};

    // Convert to tm struct
    //Sample format: 2023-03-17T13:30:00Z
    strptime(race_sessions_gp, "%Y-%m-%dT%H:%M:%S", &tm);

    nextRaceStartUtc = mktime(&tm);
    if (timeNow < nextRaceStartUtc) {
      bool newRace = false;
      if (i > offset) {
        offset = i;
        notificationSent = false;
        Serial.println("New Race");
        newRace = true;
      } else {
        Serial.println("Same Race as before");
      }
      f1Display->printRaceToScreen(races_name, races_sessions);
      printRaceTimes(races_name, races_sessions);
      racesJson.close();
      return newRace;
    }

  }
  racesJson.close();
  return false;

}

time_t getNotifyTime() {

  time_t t = nextRaceStartUtc - (6 * SECS_PER_DAY);
  // Probably should make this smarter so it's not sending notifications in the middle of the night!
  return t;
}

#endif
