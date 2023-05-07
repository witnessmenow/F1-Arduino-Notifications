
#ifndef F1DISPLAY_H
#define F1DISPLAY_H
class F1Display {
  public:
    virtual void displaySetup() = 0;
    virtual void printRaceToScreen(const char* raceName, JsonObject races_sessions) {
        Serial.println("Base Display Class - Print Race to Screen");
      };
    virtual int displayImage(char *imageFileUri) = 0;

    void setWidth(int w){
      screenWidth = w;
      screenCenterX = screenWidth /2;
    }

    void setHeight(int h){
      screenHeight = h;
    }

  protected:
    int screenWidth;
    int screenHeight;
    int screenCenterX;
};
#endif
