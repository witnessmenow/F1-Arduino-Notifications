// races.h is a file made up of this JSON file from F1Calendar.com
// https://github.com/sportstimes/f1/blob/main/_db/f1/2023.json
const char *racesJson =
#include "races.h"
  ;

time_t nextRaceStartUtc;

Timezone myTZ;
F1Config rl_f1Config;

void raceLogicSetup(F1Config f1Config){
  rl_f1Config = f1Config;
}

String getConvertedTime(const char* sessionStartTime) {
  struct tm tm = {0};
  // Parse date from UTC and convert to an epoch
  strptime(sessionStartTime, "%Y-%m-%dT%H:%M:%S", &tm);
  time_t sessionEpoch = mktime(&tm);

  return myTZ.dateTime(sessionEpoch, UTC_TIME, rl_f1Config.timeFormat);
}

void printConvertedTime(const char* sessionName, const char* sessionStartTime) {

  String timeStr = getConvertedTime(sessionStartTime);
  Serial.print(sessionName);
  Serial.print(": ");
  Serial.println(timeStr);

}

void printRaceTimes(const char* raceName, JsonObject races_sessions) {
  Serial.print("Next Race: ");
  Serial.println(raceName);

  printConvertedTime("FP1", races_sessions["fp1"].as<const char*>());
  if (races_sessions.containsKey("sprint")) {
    //Is a sprint Weekend, order is different
    printConvertedTime("Qualifying", races_sessions["qualifying"].as<const char*>());
    printConvertedTime("FP2", races_sessions["fp2"].as<const char*>());
    printConvertedTime("Sprint", races_sessions["sprint"].as<const char*>());

  } else {
    printConvertedTime("FP2", races_sessions["fp2"].as<const char*>());
    printConvertedTime("FP3", races_sessions["fp3"].as<const char*>());
    printConvertedTime("Qualifying", races_sessions["qualifying"].as<const char*>());
  }

  printConvertedTime("Race", races_sessions["gp"].as<const char*>());

}

String createTelegramMessageString(const char* raceName, JsonObject races_sessions) {
  String message = "Next Race: ";
  message += raceName;
  message += "\n";
  message += "---------------------\n";

  message += "FP1: ";
  message += getConvertedTime(races_sessions["fp1"].as<const char*>());
  message += "\n";

  if (races_sessions.containsKey("sprint")) {
    //Is a sprint Weekend, order is different
    message += "Qualifying: ";
    message += getConvertedTime(races_sessions["qualifying"].as<const char*>());
    message += "\n";

    message += "FP2: ";
    message += getConvertedTime(races_sessions["fp2"].as<const char*>());
    message += "\n";

    message += "Sprint: ";
    message += getConvertedTime(races_sessions["sprint"].as<const char*>());
    message += "\n";

  } else {
    message += "FP2: ";
    message += getConvertedTime(races_sessions["fp2"].as<const char*>());
    message += "\n";

    message += "FP3: ";
    message += getConvertedTime(races_sessions["fp3"].as<const char*>());
    message += "\n";

    message += "Qualifying: ";
    message += getConvertedTime(races_sessions["qualifying"].as<const char*>());
    message += "\n";

  }

  message += "Race: ";
  message += getConvertedTime(races_sessions["gp"].as<const char*>());
  message += "\n";
  return message;
}

bool sendNotificationOfNextRace(UniversalTelegramBot *bot, int offset) {
  DynamicJsonDocument doc(12288);

  DeserializationError error = deserializeJson(doc, racesJson);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return false;
  }
  JsonArray races = doc["races"];

  int racesAmount = races.size();

  const char* races_name = races[offset]["name"];
  JsonObject races_sessions = races[offset]["sessions"];


  printRaceTimes(races_name, races_sessions);

  Serial.print("Sending message to ");
  Serial.println(rl_f1Config.chatId);
  
  return bot->sendPhoto(rl_f1Config.chatId, "https://i.imgur.com/q3qsfSi.png", createTelegramMessageString(races_name, races_sessions));
  
}

bool getNextRace(int &offset, bool &notificationSent) {

  StaticJsonDocument<112> filter;

  JsonObject filter_races_0 = filter["races"].createNestedObject();
  filter_races_0["name"] = true;
  filter_races_0["location"] = true;
  filter_races_0["round"] = true;
  filter_races_0["circuitImage"] = true;
  filter_races_0["sessions"] = true;

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
    char buf[100];

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
      printRaceTimes(races_name, races_sessions);
      return newRace;
    }

  }

  return false;

}

time_t getNotifyTime() {

  time_t t = nextRaceStartUtc - (6 * SECS_PER_DAY);
  // Probably should make this smarter so it's not sending notifications in the middle of the night!
  return t;
}
