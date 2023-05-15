#include "display.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
// This is the library for interfacing with the display

// Can be installed from the library manager (Search for "ESP32 MATRIX DMA")
// https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA


// ----------------------------
// Dependency Libraries - each one of these will need to be installed.
// ----------------------------

// Adafruit GFX library is a dependency for the matrix Library
// Can be installed from the library manager
// https://github.com/adafruit/Adafruit-GFX-Library

// -------------------------------------
// -------   Matrix Config   ------
// -------------------------------------

const int panelResX = 64;      // Number of pixels wide of each INDIVIDUAL panel module.
const int panelResY = 64;     // Number of pixels tall of each INDIVIDUAL panel module.
const int panel_chain = 1;      // Total number of panels chained one to another

//#define ENABLE_DOUBLE_BUFFER 1 // This is a good example to show the difference the
// double buffer makes, it doesn't flash as much
// comment this out to test without it

MatrixPanel_I2S_DMA *dma_display = nullptr;

uint16_t myBLACK = dma_display->color565(0, 0, 0);
uint16_t myWHITE = dma_display->color565(255, 255, 255);
uint16_t myRED = dma_display->color565(255, 0, 0);
uint16_t myGREEN = dma_display->color565(0, 255, 0);
uint16_t myBLUE = dma_display->color565(0, 0, 255);

class MatrixDisplay: public F1Display {
  public:

    void displaySetup() {

      Serial.println("matrix display setup");
      setWidth(panelResX * panel_chain);
      setHeight(panelResY);

      HUB75_I2S_CFG mxconfig(
        panelResX,   // module width
        panelResY,   // module height
        panel_chain    // Chain length
      );

      // If you are using a 64x64 matrix you need to pass a value for the E pin
      // The trinity connects GPIO 18 to E.
      // This can be commented out for any smaller displays (but should work fine with it)
      mxconfig.gpio.e = 18;

#ifdef ENABLE_DOUBLE_BUFFER
      // This is how you enable the double buffer.
      // Double buffer can help with animation heavy projects
      mxconfig.double_buff = true;
#endif


      // May or may not be needed depending on your matrix
      // Example of what needing it looks like:
      // https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA/issues/134#issuecomment-866367216
      mxconfig.clkphase = false;

      // Some matrix panels use different ICs for driving them and some of them have strange quirks.
      // If the display is not working right, try this.
      //mxconfig.driver = HUB75_I2S_CFG::FM6126A;

      dma_display = new MatrixPanel_I2S_DMA(mxconfig);
      dma_display->begin();
    }

    void printRaceToScreen(const char* raceName, JsonObject races_sessions) {


      const char* raceNameChanged = updateRaceName(raceName);
      if (isRaceWeek(races_sessions["gp"])) {
        // It's race week!
        dma_display->fillScreen(myBLACK);
        dma_display->setTextSize(1);     // size 2 == 16 pixels high
        dma_display->setTextWrap(false); // N.B!! Don't wrap at end of line

        int16_t xOne, yOne;
        uint16_t w, h;

        // This method updates the variables with what width (w) and height (h)
        // the give text will have.

        dma_display->getTextBounds(raceNameChanged, 0, 0, &xOne, &yOne, &w, &h);

        int xPosition = screenCenterX - w / 2;
        dma_display->setTextColor(myBLUE);
        dma_display->setCursor(xPosition, 2);
        dma_display->print(raceNameChanged);

        int yValue = 12;
        for (JsonPair kv : races_sessions) {
          printSession( yValue,
                        matrixSessionCodeToString(kv.key().c_str()),
                        getConvertedTime(kv.value().as<const char*>(), "H:i"));
          yValue += 10;
        }
      } else {
        // Not yet race week
        dma_display->fillScreen(myBLACK);
        dma_display->setTextSize(1);     // size 2 == 16 pixels high
        dma_display->setTextWrap(false); // N.B!! Don't wrap at end of line

        int16_t xOne, yOne;
        uint16_t w, h;

        // This method updates the variables with what width (w) and height (h)
        // the give text will have.
        dma_display->getTextBounds("Next Race:", 0, 0, &xOne, &yOne, &w, &h);
        int xPosition = screenCenterX - w / 2;
        dma_display->setTextColor(myGREEN);
        dma_display->setCursor(xPosition, 2);
        dma_display->print("Next Race:");
        
        
        

        // This method updates the variables with what width (w) and height (h)
        // the give text will have.

        dma_display->getTextBounds(raceNameChanged, 0, 0, &xOne, &yOne, &w, &h);

        xPosition = screenCenterX - w / 2;
        dma_display->setTextColor(myBLUE);
        dma_display->setCursor(xPosition, 10);
        dma_display->print(raceNameChanged);

        printSession( 20,
                      "GP:",
                        getConvertedTime(races_sessions["gp"], "M d"));
        
      }


    }

    int displayImage(char *imageFileUri) {
      return 0;
    }

    void drawWifiManagerMessage(WiFiManager *myWiFiManager) {
      Serial.println("Entered Conf Mode");
      dma_display->fillScreen(myBLACK);
      dma_display->setTextSize(1);     // size 1 == 8 pixels high
      dma_display->setTextWrap(false);
      dma_display->setTextColor(myBLUE);
      dma_display->setCursor(0, 0);
      dma_display->print(myWiFiManager->getConfigPortalSSID());

      dma_display->setTextWrap(true);
      dma_display->setTextColor(myRED);
      dma_display->setCursor(0, 8);
      dma_display->print(WiFi.softAPIP());
    }

  private:

    const char* matrixSessionCodeToString(const char* sessionCode) {
      if (strcmp(sessionCode, "fp1") == 0)
      {
        return "FP1:";
      } else if (strcmp(sessionCode, "fp2") == 0)
      {
        return "FP2:";
      } else if (strcmp(sessionCode, "fp3") == 0)
      {
        return "FP3:";
      } else if (strcmp(sessionCode, "qualifying") == 0)
      {
        return "Qual:";
      } else if (strcmp(sessionCode, "sprintQualifying") == 0)
      {
        return "Sp Q:";
      } else if (strcmp(sessionCode, "sprint") == 0)
      {
        return "Spr:";
      } else if (strcmp(sessionCode, "gp") == 0)
      {
        return "Race:";
      }

      return "UNKNOWN";
    }

    const char* updateRaceName(const char* sessionCode){
      if (strcmp(sessionCode, "Emilia Romagna Grand Prix") == 0)
      {
        return "Monza";
      }

      return sessionCode;
    }

    void printSession(int y, const char* sessionName, String sessionStartTime) {

      // Print Session Name on the left
      dma_display->setTextColor(myRED);
      dma_display->setCursor(1, y);
      dma_display->print(sessionName);

      Serial.println(sessionName);

      // Print time on the right

      int16_t xOne, yOne;
      uint16_t w, h;

      // This method updates the variables with what width (w) and height (h)
      // the give text will have.
      Serial.println(sessionStartTime);
      dma_display->getTextBounds(sessionStartTime, 0, 0, &xOne, &yOne, &w, &h);

      int xPosition = screenWidth - w;

      Serial.println(xPosition);

      dma_display->setCursor(xPosition, y);
      dma_display->print(sessionStartTime);

    }

};
