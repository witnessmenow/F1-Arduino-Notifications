
// Number of seconds after reset during which a
// subseqent reset will be considered a double reset.
#define DRD_TIMEOUT 10

// RTC Memory Address for the DoubleResetDetector to use
#define DRD_ADDRESS 0

#define WM_F1_TIME_ZONE_LABEL "timeZone"
#define WM_F1_TIME_FORMAT_LABEL "timeFormat"
#define WM_F1_BOT_TOKEN_LABEL "botToken"
#define WM_F1_CHAT_ID_LABEL "chatId"
#define WM_F1_NOTIFCATION_LABEL "notification"

DoubleResetDetector* drd;
F1Display* wm_Display;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  wm_Display->drawWifiManagerMessage(myWiFiManager);
}

void setupWiFiManager(bool forceConfig, F1Config f1Config, F1Display* theDisplay) {
  wm_Display = theDisplay;
  WiFiManager wm;
  //set config save notify callback
  wm.setSaveConfigCallback(saveConfigCallback);
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wm.setAPCallback(configModeCallback);

  WiFiManagerParameter timeZoneParam(WM_F1_TIME_ZONE_LABEL, "Time Zone", f1Config.timeZone.c_str(), 60);
  WiFiManagerParameter timeFormatParam(WM_F1_TIME_FORMAT_LABEL, "Time Format", f1Config.timeFormat.c_str(), 40);
  WiFiManagerParameter telegramBotParam(WM_F1_BOT_TOKEN_LABEL, "Bot Token", f1Config.botToken.c_str(), 60);
  WiFiManagerParameter telegramChatIdParam(WM_F1_CHAT_ID_LABEL, "Chat ID", f1Config.chatId.c_str(), 40);

  char checkBox[] = "type=\"checkbox\"";
  char checkBoxChecked[] = "type=\"checkbox\" checked";
  char* customHtml;

  if (f1Config.currentRaceNotification) {
    customHtml = checkBoxChecked;
  } else {
    customHtml = checkBox;
  }
  WiFiManagerParameter isNotificationSent(WM_F1_NOTIFCATION_LABEL, "Notification Sent", "T", 2, customHtml);

  wm.addParameter(&timeZoneParam);
  wm.addParameter(&timeFormatParam);
  wm.addParameter(&telegramBotParam);
  wm.addParameter(&telegramChatIdParam);
  wm.addParameter(&isNotificationSent);

  if (forceConfig) {
    // IF we forced config this time, lets stop the double reset so it doesn't get stuck in a loop
    drd->stop();
    if (!wm.startConfigPortal("f1Thing", "nomikey1")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  } else {
    if (!wm.autoConnect("f1Thing", "nomikey1")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }

  //save the custom parameters to FS
  if (shouldSaveConfig)
  {

    f1Config.timeZone =  String(timeZoneParam.getValue());
    f1Config.timeFormat =  String(timeFormatParam.getValue());
    f1Config.botToken =  String(telegramBotParam.getValue());
    f1Config.chatId =  String(telegramChatIdParam.getValue());
    f1Config.currentRaceNotification = (strncmp(isNotificationSent.getValue(), "T", 1) == 0);
    
    f1Config.saveConfigFile();
    drd->stop();
    ESP.restart();
    delay(5000);
  }

}
