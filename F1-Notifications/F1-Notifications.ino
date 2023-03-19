/*******************************************************************
    An Arduino Project for notifiying you of the start time of
    upcoming F1 sessions in your local timezone.

    Tested on an ESP32.

    If you find what I do useful and would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/

    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Twitter: https://twitter.com/witnessmenow
 *******************************************************************/

// ----------------------------
// Standard Libraries
// ----------------------------

#include <WiFi.h>

// ----------------------------
// Additional Libraries - each one of these will need to be installed.
// ----------------------------

#include <ArduinoJson.h>
// Library used for parsing Json from the API responses

// Search for "Arduino Json" in the Arduino Library manager
// https://github.com/bblanchon/ArduinoJson

#include <ezTime.h>
// Library used for getting the time and converting session time
// to users timezone

// Search for "ezTime" in the Arduino Library manager
// https://github.com/ropg/ezTime

// -------------------------------------
// ------- Replace the following! ------
// -------------------------------------

//How the time will be displayed, see here for more info: https://github.com/ropg/ezTime#datetime
String timeFormat = "l, d-M-Y H:i"; // Friday, 17-Mar-2023 00:30


String timeZone = "Europe/London"; //seems to be something wrong with Europe/Dublin

// Wifi network station credentials
char ssid[] = "ssid";     // your network SSID (name)
char password[] = "password"; // your network key

// -------------------------------------

// races.h is a file made up of this JSON file from F1Calendar.com
// https://github.com/sportstimes/f1/blob/main/_db/f1/2023.json
const char *racesJson =
#include "races.h"
  ;

Timezone myTZ;

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

void printConvertedTime(const char* sessionName, const char* sessionStartTime) {

  String timeStr = getConvertedTime(sessionStartTime);
  Serial.print(sessionName);
  Serial.print(": ");
  Serial.println(timeStr);

}

String getConvertedTime(const char* sessionStartTime) {
  struct tm tm = {0};
  // Parse date from UTC and convert to an epoch
  strptime(sessionStartTime, "%Y-%m-%dT%H:%M:%S", &tm);
  time_t sessionEpoch = mktime(&tm);
  
  return myTZ.dateTime(sessionEpoch, UTC_TIME, timeFormat);
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Waiting for time sync");

  waitForSync();

  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  myTZ.setLocation(timeZone);
  Serial.print(timeZone);
  Serial.print(F(":     "));
  Serial.println(myTZ.dateTime());
  Serial.println("-------------------------");


  DynamicJsonDocument doc(12288);

  DeserializationError error = deserializeJson(doc, racesJson);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  JsonArray races = doc["races"];

  int racesAmount = races.size();
  time_t timeNow = UTC.now();
  for (int i = 0; i < racesAmount; i++) {
    const char* races_name = races[i]["name"];
    JsonObject races_sessions = races[i]["sessions"];
    const char* race_sessions_gp = races_sessions["gp"]; // "2023-03-05T15:00:00Z"

    struct tm tm = {0};
    char buf[100];

    // Convert to tm struct
    //Sample format: 2023-03-17T13:30:00Z
    strptime(race_sessions_gp, "%Y-%m-%dT%H:%M:%S", &tm);

    time_t gpStartEpoch = mktime(&tm);

    if (timeNow < gpStartEpoch) {
      printRaceTimes(races_name, races_sessions);
      return;
    }

  }


}

void loop() {
  // put your main code here, to run repeatedly:

}
