#include <TFT_eSPI.h>

/*******************************************************************
    All code to do with displaying on the display

 *******************************************************************/

int screenWidth = 320;
int screenHeight = 240;
int screenCenterX = screenWidth / 2;

TFT_eSPI tft = TFT_eSPI();

void displaySetup() {
  // Start the tft display and set it to black
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void PNGDraw(PNGDRAW *pDraw)
{
  uint16_t usPixels[320];

  png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(0, pDraw->y, pDraw->iWidth, 1, usPixels);
}

fs::File myfile;

void * myOpen(const char *filename, int32_t *size) {
  myfile = SPIFFS.open(filename);
  *size = myfile.size();
  return &myfile;
}
void myClose(void *handle) {
  if (myfile) myfile.close();
}
int32_t myRead(PNGFILE  *handle, uint8_t *buffer, int32_t length) {
  if (!myfile) return 0;
  return myfile.read(buffer, length);
}
int32_t mySeek(PNGFILE  *handle, int32_t position) {
  if (!myfile) return 0;
  return myfile.seek(position);
}

int displayImage(char *imageFileUri) {
  tft.fillScreen(TFT_BLACK);
  unsigned long lTime = millis();
  lTime = millis();
  Serial.println(imageFileUri);
  int rc = png.open((const char *) imageFileUri, myOpen, myClose, myRead, mySeek, PNGDraw);
  if (rc == PNG_SUCCESS) {
    Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
    rc = png.decode(NULL, 0);
    png.close();
  } else {
    Serial.print("error code: ");
    Serial.println(rc);
  }

  Serial.print("Time taken to decode and display Image (ms): ");
  Serial.println(millis() - lTime);

  return rc;
}

void printSession(int x, int y, const char* sessionName, String sessionStartTime) {

  String tempStr = String(sessionName);
  tempStr += ": ";
  tempStr += sessionStartTime;
  tft.drawString(tempStr, x, y, 2);
}

void printRaceToScreen(const char* raceName, JsonObject races_sessions, String (*getConvertedTime)(const char* sessionStartTime)) {

  tft.fillRect(0, 0, screenWidth, screenHeight, TFT_BLACK);
  String tempStr = "Next Race: ";
  tempStr += String(raceName);

  tft.drawString(tempStr, 5, 5, 2);

  printSession(5, 21, "FP1", getConvertedTime(races_sessions["fp1"].as<const char*>()));
  if (races_sessions.containsKey("sprint")) {
    //Is a sprint Weekend, order is different
    printSession(5, 37, "Qualifying", getConvertedTime(races_sessions["qualifying"].as<const char*>()));
    printSession(5, 53, "FP2", getConvertedTime(races_sessions["fp2"].as<const char*>()));
    printSession(5, 69, "Sprint", getConvertedTime(races_sessions["sprint"].as<const char*>()));

  } else {
    printSession(5, 37, "FP2", getConvertedTime(races_sessions["fp2"].as<const char*>()));
    printSession(5, 53, "FP3", getConvertedTime(races_sessions["fp3"].as<const char*>()));
    printSession(5, 69, "Qualifying", getConvertedTime(races_sessions["qualifying"].as<const char*>()));
  }

  printSession(5, 85, "Race", getConvertedTime(races_sessions["gp"].as<const char*>()));
}


//void drawWifiManagerMessage(WiFiManager *myWiFiManager){
//  Serial.println("Entered Conf Mode");
//  tft.fillScreen(TFT_BLACK);
//  tft.setTextColor(TFT_WHITE, TFT_BLACK);
//  tft.drawCentreString("Entered Conf Mode:", screenCenterX, 5, 2);
//  tft.drawString("Connect to the following WIFI AP:", 5, 28, 2);
//  tft.setTextColor(TFT_BLUE, TFT_BLACK);
//  tft.drawString(myWiFiManager->getConfigPortalSSID(), 20, 48, 2);
//  tft.setTextColor(TFT_WHITE, TFT_BLACK);
//  tft.drawString("Password:", 5, 64, 2);
//  tft.setTextColor(TFT_BLUE, TFT_BLACK);
//  tft.drawString("thing123", 20, 82, 2);
//  tft.setTextColor(TFT_WHITE, TFT_BLACK);
//
//  tft.drawString("If it doesn't AutoConnect, use this IP:", 5, 110, 2);
//  tft.setTextColor(TFT_BLUE, TFT_BLACK);
//  tft.drawString(WiFi.softAPIP().toString(), 20, 128, 2);
//  tft.setTextColor(TFT_WHITE, TFT_BLACK);
//
//}
