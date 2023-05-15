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
// Display type
// ---------------------------

// This project currently supports the following displays
// (Uncomment the required #define)

// 1. Cheap yellow display (Using TFT-eSPI library)
//#define YELLOW_DISPLAY 

// 2. Matrix Displays (Like the ESP32 Trinity)
//#define MATRIX_DISPLAY

// If no defines are set, it will default to CYD
#if !defined(YELLOW_DISPLAY) && !defined(MATRIX_DISPLAY)
#define YELLOW_DISPLAY // Default to Yellow Display for display type
#endif


// ----------------------------
// Library Defines - Need to be defined before library import
// ----------------------------

#define ESP_DRD_USE_SPIFFS      true

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

#include <WiFiManager.h>
// Captive portal for configuring the WiFi

// If installing from the library manager (Search for "WifiManager")
// https://github.com/tzapu/WiFiManager

#include <ESP_DoubleResetDetector.h>
// A library for checking if the reset button has been pressed twice
// Can be used to enable config mode
// Can be installed from the library manager (Search for "ESP_DoubleResetDetector")
//https://github.com/khoih-prog/ESP_DoubleResetDetector

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

#include <FileFetcher.h>
// Library used to get files or images

// Not on library manager yet
// https://github.com/witnessmenow/file-fetcher-arduino

// ----------------------------
// Internal includes
// ----------------------------

#include "githubCert.h"

#include "display.h"

#include "config.h"

#include "raceLogic.h"

#include "wifiManager.h"

// ----------------------------
// Display Handling Code
// ----------------------------

#if defined YELLOW_DISPLAY

#include "cheapYellowLCD.h"
CheapYellowDisplay cyd;
F1Display* f1Display = &cyd;

#elif defined MATRIX_DISPLAY

#include "matrixDisplay.h"
MatrixDisplay matrixDisplay;
F1Display* f1Display = &matrixDisplay;

#endif
// ----------------------------

WiFiClientSecure secured_client;
UniversalTelegramBot bot("", secured_client);

F1Config f1Config;


FileFetcher fileFetcher(secured_client);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);

  f1Display->displaySetup();

  bool forceConfig = false;

  drd = new DoubleResetDetector(DRD_TIMEOUT, DRD_ADDRESS);
  if (drd->detectDoubleReset()) {
    Serial.println(F("Forcing config mode as there was a Double reset detected"));
    forceConfig = true;
  }

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

  if (!f1Config.fetchConfigFile()) {
    // Failed to fetch config file, need to launch Wifi Manager
    forceConfig = true;
  }

  setupWiFiManager(forceConfig, f1Config, f1Display);
  raceLogicSetup(f1Config);
  bot.updateToken(f1Config.botToken);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  //WiFi.mode(WIFI_STA);
  //WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  secured_client.setCACert(github_server_cert);
  while (fetchRaceJson(fileFetcher) != 1) {
    Serial.println("failed to get Race Json");
    Serial.println("will try again in 10 seconds");
    delay(1000 * 10);
  }

  Serial.println("Fetched races.json File");


  Serial.println("Waiting for time sync");

  waitForSync();

  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  myTZ.setLocation(f1Config.timeZone);
  Serial.print(f1Config.timeZone);
  Serial.print(F(":     "));
  Serial.println(myTZ.dateTime());
  Serial.println("-------------------------");

  //sendNotificationOfNextRace(&bot, f1Config.roundOffset);

}

bool notificaitonEventRaised = false;

void sendNotification() {
  // Cause it could be set to the image one
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  Serial.println("Sending notifcation");
  f1Config.currentRaceNotification = sendNotificationOfNextRace(&bot, f1Config.roundOffset);
  if (!f1Config.currentRaceNotification) {
    //Notificaiton failed, raise event again
    Serial.println("Notfication failed");
    setEvent( sendNotification, getNotifyTime() );
  } else {
    notificaitonEventRaised = false;
    f1Config.saveConfigFile();
  }
}

bool first = true;

int minuteCounter = 0;

void loop() {
  drd->loop();
  if (first || minuteChanged()) {
    minuteCounter ++;
    first = false;
    bool newRace = getNextRace(f1Config.roundOffset, f1Config.currentRaceNotification, f1Display);
    if (newRace) {
      f1Config.saveConfigFile();
    }
    if (!f1Config.currentRaceNotification && !notificaitonEventRaised) {
      //we have never notified about this race yet, so we'll raise an event
      setEvent( sendNotification, getNotifyTime() );
      notificaitonEventRaised = true;
      Serial.print("Raised event for: ");
      Serial.println(myTZ.dateTime(getNotifyTime(), UTC_TIME, f1Config.timeFormat));
    }
  }

  if (minuteCounter >= 60) {
    secured_client.setCACert(github_server_cert);
    while (fetchRaceJson(fileFetcher) != 1) {
      Serial.println("failed to get Race Json");
      Serial.println("will try again in 10 seconds");
      delay(1000 * 10);
    }
    minuteCounter = 0;
  }
  events();
}
