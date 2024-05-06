#include "display.h"
#include "getImage.h"
#include "util.h"

#include <TFT_eSPI.h>
// A library for interfacing with LCD displays
//
// Can be installed from the library manager (Search for "TFT_eSPI")
// https://github.com/Bodmer/TFT_eSPI

#include <PNGdec.h>
// For decoding png files
//
// Can be installed from the library manager (Search for "PNGdec")
// https://github.com/bitbank2/PNGdec

// -------------------------------
// Putting this stuff outside the class because
// I can't easily pass member functions in as callbacks for pngdec

// -------------------------------

#define SESSION_TEXT_SIZE 4

TFT_eSPI tft = TFT_eSPI();
PNG png;

fs::File myfile;

void *myOpen(const char *filename, int32_t *size)
{
  myfile = SPIFFS.open(filename);
  *size = myfile.size();
  return &myfile;
}
void myClose(void *handle)
{
  if (myfile)
    myfile.close();
}
int32_t myRead(PNGFILE *handle, uint8_t *buffer, int32_t length)
{
  if (!myfile)
    return 0;
  return myfile.read(buffer, length);
}
int32_t mySeek(PNGFILE *handle, int32_t position)
{
  if (!myfile)
    return 0;
  return myfile.seek(position);
}

void PNGDraw(PNGDRAW *pDraw)
{
  uint16_t usPixels[320];

  png.getLineAsRGB565(pDraw, usPixels, PNG_RGB565_BIG_ENDIAN, 0xffffffff);
  tft.pushImage(0, pDraw->y, pDraw->iWidth, 1, usPixels);
}

class CheapYellowDisplay : public F1Display
{
public:
  void displaySetup()
  {

    Serial.println("cyd display setup");
    setWidth(320);
    setHeight(240);

    // Start the tft display and set it to black
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    state = unset;
  }

  void displayPlaceHolder(const char *raceName, JsonObject races_sessions)
  {

    if (!isSameRace(raceName) || state != placeholder)
    {
      setRaceName(raceName);
      int imageFileStatus = getImage(raceName);
      if (imageFileStatus)
      {
        int imageDisplayStatus = displayImage(TRACK_IMAGE);
        if (imageDisplayStatus == PNG_SUCCESS)
        {
          // Image is displayed
          tft.setTextColor(TFT_WHITE, TFT_BLACK);
          int yPos = 215;
          String gpStartDateStr = String(getConvertedTime(races_sessions["gp"], "M d"));
          String displayMessage = String(convertRaceName(raceName)) + " | " + gpStartDateStr;
          tft.drawCentreString(displayMessage, screenCenterX, yPos, 4);
          state = placeholder;
          return;
        }
      }
      // Failed to display the image
      displayRaceWeek(raceName, races_sessions); // For now
    }

    // if we reach here, the screen doesn't need to be updated
    Serial.println("No need to update display");
  }

  void displayRaceWeek(const char *raceName, JsonObject races_sessions)
  {
    Serial.println("prts");
    tft.fillRect(0, 0, screenWidth, screenHeight, TFT_BLACK);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    int yPos = 5;
    String gpStartDateStr = String(getConvertedTime(races_sessions["gp"], "M d"));
    String displayMessage = String(convertRaceName(raceName)) + " | " + gpStartDateStr;
    tft.drawCentreString(displayMessage, screenCenterX, yPos, 4);

    int yValue = 46;
    for (JsonPair kv : races_sessions)
    {
      printSession(4,
                   yValue,
                   sessionCodeToString(kv.key().c_str()),
                   getConvertedTime(kv.value().as<const char *>()));
      yValue += (SESSION_TEXT_SIZE) * 8;
    }

    state = raceweek;
  }

  int displayImage(char *imageFileUri)
  {
    tft.fillScreen(TFT_BLACK);
    unsigned long lTime = millis();
    lTime = millis();
    Serial.println(imageFileUri);

    int rc = png.open((const char *)imageFileUri, myOpen, myClose, myRead, mySeek, PNGDraw);
    if (rc == PNG_SUCCESS)
    {
      Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
      rc = png.decode(NULL, 0);
      png.close();
    }
    else
    {
      Serial.print("error code: ");
      Serial.println(rc);
    }

    Serial.print("Time taken to decode and display Image (ms): ");
    Serial.println(millis() - lTime);

    return rc;
  }

  void drawWifiManagerMessage(WiFiManager *myWiFiManager)
  {
    Serial.println("Entered Conf Mode");
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawCentreString("Entered Conf Mode:", screenCenterX, 5, 2);
    tft.drawString("Connect to the following WIFI AP:", 5, 28, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(myWiFiManager->getConfigPortalSSID(), 20, 48, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Password:", 5, 64, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString("nomikey1", 20, 82, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.drawString("If it doesn't AutoConnect, use this IP:", 5, 110, 2);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);
    tft.drawString(WiFi.softAPIP().toString(), 20, 128, 2);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  }

private:
  void printSession(int x, int y, const char *sessionName, String sessionStartTime)
  {
    String tempStr = String(sessionName);
    tempStr += " ";
    tempStr += sessionStartTime;
    tft.drawString(tempStr, x, y, SESSION_TEXT_SIZE);
  }
};
