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

#include <WiFiClientSecure.h>

#include <FS.h>
#include "SPIFFS.h"

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

#include <UniversalTelegramBot.h>
// Library used to send Telegram Message

// Search for "Universal Telegram" in the Arduino Library manager
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

// ----------------------------
// Internal includes
// ----------------------------

#include "config.h"

#include "raceLogic.h"


// -------------------------------------
// ------- Replace the following! ------
// -------------------------------------

// Wifi network station credentials
char ssid[] = "SSID";     // your network SSID (name)
char password[] = "password"; // your network key

// -------------------------------------

WiFiClientSecure secured_client;
UniversalTelegramBot bot(botToken, secured_client);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  // Initialise SPIFFS, if this fails try .begin(true)
  // NOTE: I believe this formats it though it will erase everything on
  // spiffs already! In this example that is not a problem.
  // I have found once I used the true flag once, I could use it
  // without the true flag after that.
  bool spiffsInitSuccess = SPIFFS.begin(false) || SPIFFS.begin(true);
  if (!spiffsInitSuccess)
  {
    Serial.println("SPIFFS initialisation failed!");
    while (1)
      yield(); // Stay here twiddling thumbs waiting
  }
  Serial.println("\r\nInitialisation done.");

  fetchConfigFile();

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


  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  //sendNotificationOfNextRace(&bot, roundOffset);

}

void sendNotification() {
  currentRaceNotification = sendNotificationOfNextRace(&bot, roundOffset);
  if (!currentRaceNotification) {
    //Notificaiton failed, raise event again
    setEvent( sendNotification, getNotifyTime() );
  } else {

    saveConfigFile();
  }
}

void loop() {
  if (minuteChanged()) {
    bool newRace = getNextRace(roundOffset, currentRaceNotification);
    if (newRace) {
      saveConfigFile();
    }
    if (!currentRaceNotification) {
      //we have never notified about this race yet, so we'll raise an event
      setEvent( sendNotification, getNotifyTime() );
      Serial.print("Raised event for: ");
      Serial.println(myTZ.dateTime(getNotifyTime(), UTC_TIME, timeFormat));
    }
  }
  events();
}

