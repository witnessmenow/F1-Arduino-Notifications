# F1-Arduino-Notifications

An ESP32 project to display and notify of when F1 races are and what time the sessions start at in your local timezone.

![image](https://github.com/witnessmenow/F1-Arduino-Notifications/assets/1562562/ddc0c493-4931-4c45-a793-b55f95eb355a)

![image](https://github.com/witnessmenow/F1-Arduino-Notifications/assets/1562562/383d98ef-5887-4b78-b792-485d60d4a772)

![image](https://github.com/witnessmenow/F1-Arduino-Notifications/assets/1562562/3bcfdb33-3c6f-4777-8bc6-21c821566a0f)

This project can flashed directly from a webpage, so can be setup in minutes!

## Features

- Connects to your Wifi to automatically fetch data about the next race
- When the race is more than a week away, it will display the date of the next race (with an image on the "Cheap Yellow Display")
- When it's race week, it will display the start time of all the sessions in your local timezone
- It will send you notification on Telegram on the Monday of race week with the start times.

This project is a Work in Progress!

## Help Support what I do!

[If you enjoy my work, please consider becoming a Github sponsor!](https://github.com/sponsors/witnessmenow/)

## Hardware Required

This project is designed to make use of basically ready to go hardware, so is very easy to get up and running

Currently this project runs on two types of hardware:

### "Cheap Yellow Display" (CYD)

An ESP32 With Built in 320x240 LCD with Touch Screen (ESP32-2432S028R), buy from wherever works out cheapest for you:

- [Aliexpress\*](https://s.click.aliexpress.com/e/_DkSpIjB)
- [Aliexpress\*](https://s.click.aliexpress.com/e/_DkcmuCh)
- [Aliexpress](https://www.aliexpress.com/item/1005004502250619.htm)
- [Makerfabs](https://www.makerfabs.com/sunton-esp32-2-8-inch-tft-with-touch.html)

### Matrix panel

It's built to work with the [ESP32 Trinity](https://github.com/witnessmenow/ESP32-Trinity), an open source board I created for controlling Hub75 Matrix panels, but it will does work with any ESP32 that breaks out enough pins.

The display it uses is a 64x64 HUB75 Matrix Panel.

All the parts can be purchased from Makerfabs.com:

- [ESP32 Trinity](https://www.makerfabs.com/esp32-trinity.html)
- [64 x 64 Matrix Panel](https://www.makerfabs.com/64x64-rgb-led-matrix-3mm-pitch.html)
- Optional: [5V Power Supply](https://www.makerfabs.com/5v-6a-ac-dc-power-adapter-with-cable.html) - You can alternatively use a USB-C power supply

\* = Affilate Link

### BYOD (Bring your own display)

I've tried to design this project to be modular and have abstracted the display code behind an interface, so it should be pretty easy to get it up and running with a different type of display.

## Project Setup

These steps will only need to be run once.

### Step 1 - Telegram Setup (Optional)

To get notified about the races, you will need some things on Telegram.

- Install Telegram and setup an account.
- Search for a user called "botFather" and follow instructions to create a new bot. Keep note of this bot token
- Start the chat with the created bot.
- Search for a user called "myIdBot" and follow instructions to get your chat ID. Keep note of the chatID.

### Step 2 - Flash the Project

This project can be flashed directly from your browser [here](https://witnessmenow.github.io/F1-Arduino-Notifications/) (Chrome & Edge only)

- For the "Cheap Yellow Display" (CYD):
  - If your CYD has one USB port, Click the "CYD" button
  - If it has two USB ports, Click the "CYD2USB" button
- For ESP32 Trinity/Matrix panel
  - Click the "Matrix" button

This webflash code is automatically built from the main branch of this repo, so it will always be up to date.

Note: If you want to program this project yourself without the webflash, follow the **Code** steps below.

### Step 3 - Adding your Wifi, Timezone and Telegram Details

In order to enter your wifi details, the project will host it's own wifi network. Connect to it using your phone.

- SSID: f1Thing
- Password: nomikey1

You should be automatically redirected to the config page.

- Click Config
- Enter your WIfi details
- Enter your **Time Zone** - More info [here](https://github.com/ropg/ezTime#setlocation)
- (Optional) Enter your **Bot Token** that you retrieved at the earlier step.
- (Optional) Enter your **Chat ID** that you retrieved at the earlier step.
- You can leave the other options
- Click save

Note: If you ever need to get back into WiFiManager, click reset button twice.

The project should now be setup and start displaying the next race details!

## Code

If you want to program this project manually, there are two options

### PlatformIO

PlatformIO is the easiest way to code this project.

In the [platformio.ini](platformio.ini), there are several environments defined for the different boards

| Environment | Description                                                                                                                  |
| ----------- | ---------------------------------------------------------------------------------------------------------------------------- |
| env:cyd     | For the [Cheap Yellow Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display)                                   |
| env:cyd2usb | For the Cheap Yellow Display with two USB ports                                                                              |
| env:trinity | For the [ESP32 Trinity](https://github.com/witnessmenow/ESP32-Trinity) (or generic ESP32 wired to the matrix panel the same) |

When you select the environment, it will automatically install the right libraries and set the configurations in the code.

### Arduino IDE

If you want to use the Arduino IDE, you will need to do the following to get it workin

The following libraries need to be installed for this project to work:

| Library Name/Link                                                                      | Purpose                                    | Library manager                 |
| -------------------------------------------------------------------------------------- | ------------------------------------------ | ------------------------------- |
| [WifiManager - By Tzapu](https://github.com/tzapu/WiFiManager)                         | Captive portal for configuring the WiFi    | Yes ("WifiManager"              |
| [ESP_DoubleResetDetector](https://github.com/khoih-prog/ESP_DoubleResetDetector)       | Detecting double pressing the reset button | Yes ("ESP_DoubleResetDetector") |
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson)                                | For parsing JSON                           | Yes ("Arduino Json")            |
| [ezTime](https://github.com/ropg/ezTime)                                               | For handling timezones                     | Yes ("eztime")                  |
| [UniversalTelegramBot](https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot) | Telegram bots for your ESP                 | Yes ("UniversalTelegramBot")    |
| [FileFetcher](https://github.com/witnessmenow/file-fetcher-arduino)                    | For fetching files/images from the web     | No, download from Github        |

### Cheap Yellow Display Specific libraries

| Library Name/Link                              | Purpose                         | Library manager  |
| ---------------------------------------------- | ------------------------------- | ---------------- |
| [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | For controlling the LCD Display | Yes ("tft_espi") |
| [PNGdec](https://github.com/bitbank2/PNGdec)   | For decoding png images         | Yes ("PNGdec")   |

### Matrix Panel Specific libraries

| Library Name/Link                                                                                 | Purpose                          | Library manager          |
| ------------------------------------------------------------------------------------------------- | -------------------------------- | ------------------------ |
| [ESP32-HUB75-MatrixPanel-I2S-DMA](https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA) | For controlling the LED Matrix   | Yes ("ESP32 MATRIX DMA") |
| [Adafruit GFX library](https://github.com/adafruit/Adafruit-GFX-Library)                          | Dependancy of the Matrix library | Yes ("Adafruit GFX")     |

### Cheap Yellow Display Display Config

The CYD version of the project makes use of [TFT_eSPI library by Bodmer](https://github.com/Bodmer/TFT_eSPI).

TFT_eSPI is configured using a "User_Setup.h" file in the library folder, you will need to replace this file with the one from the CYD repo.

- CYD (CYD with single USB): https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/blob/main/DisplayConfig/User_Setup.h
- CYD2USB (CYD with 2 USB): https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display/blob/main/DisplayConfig/CYD2USB/User_Setup.h

### Display Selection

At the top of the `F1-Notifications.ino` file, there is a section labeled "Display Type", follow the instructions there for how to enable the different displays.

By default it will use the Cheap Yellow Display

Note: CYD and CYD2USB both use the `YELLOW_DISPLAY` define.
