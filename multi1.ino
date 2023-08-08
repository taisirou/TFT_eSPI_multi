
#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
int page;

#define CALIBRATION_FILE "/calibrationData"

void setup(void) {
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;

  Serial.begin(115200);
  Serial.println("starting");

  tft.init();

  tft.setRotation(3);
  tft.fillScreen((0xFFFF));

  tft.setCursor(20, 0, 2);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);  tft.setTextSize(1);
  tft.println("calibration run");

  // check file system
  if (!SPIFFS.begin()) {
    Serial.println("formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    File f = SPIFFS.open(CALIBRATION_FILE, "r");
    if (f) {
      if (f.readBytes((char *)calibrationData, 14) == 14)
        calDataOK = 1;
      f.close();
    }
  }
  if (calDataOK) {
    // calibration data valid
    tft.setTouch(calibrationData);
  } else {
    // data not valid. recalibrate
    tft.calibrateTouch(calibrationData, TFT_WHITE, TFT_RED, 15);
    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calibrationData, 14);
      f.close();
    }
  }
  //  tft.fillScreen((0xFFFF));

   page_1();

}

void loop() {
  if (page == '1') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);

    if (tft.getTouch(&x, &y)) {

      tft.setCursor(150, 5, 2);
      tft.printf("x: %i     ", x);
      tft.setCursor(150, 20, 2);
      tft.printf("y: %i    ", y);
      color += 155;
    }
    if (pressed) {
      if ((x > 35) && (x < 150)) {
        if ((y > 75) && (y < 120)) {
          page_2();
        }
      }
    }
  }
  if (page == '2') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);

    if (tft.getTouch(&x, &y)) {

      tft.setCursor(150, 5, 2);
      tft.printf("x: %i     ", x);
      tft.setCursor(150, 20, 2);
      tft.printf("y: %i    ", y);

      tft.drawPixel(x, y, color);
      color += 155;
    }

    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 280) && (y < 315)) {
          page_1();
        }
      }
    }
  }
}
void page_1() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 375, 50, TFT_WHITE);
  tft.fillRect(375, 0, 480, 50, TFT_YELLOW);

  tft.fillRect(35, 75, 170, 45, TFT_GREEN); //stroke line
  tft.loadFont("msgothic-36");
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString("     状態     ", 35  , 85);
  tft.fillRect(35, 135, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(35, 195, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(35, 255, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(275, 75, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(275, 135, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(275, 195, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(275, 255, 170, 45, TFT_RED); //stroke line
  page = '1';//change the state page into 1

}
void page_2() {
  tft.setCursor(0, 0);
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 35, 375, 35, TFT_WHITE);
  tft.drawLine(0, 65, 375, 65, TFT_WHITE);
  tft.drawRect(0, 95, 480, 280, TFT_WHITE);
  tft.loadFont("msgothic-36");
  tft.fillRect(375, 0, 480, 95, TFT_YELLOW);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.drawString("状態", 390  , 30);
  tft.fillRect(20, 195, 95, 70, TFT_DARKGREEN);
  tft.fillRect(115, 195, 350, 70, TFT_WHITE);
  tft.fillRect(0, 275, 480, 45, TFT_WHITE);
  tft.fillRect(0, 280, 105, 35, TFT_CYAN);
  tft.fillRect(125, 280, 105, 35, TFT_CYAN);
  tft.fillRect(250, 280, 105, 35, TFT_CYAN);
  tft.fillRect(375, 280, 105, 35, TFT_CYAN);

  tft.loadFont("msgothic-28");
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("運行 : 21  行先 : 荻窪", 10, 7);
  tft.drawString("地点 :     池袋", 10, 37);
  tft.drawString("終端戸締 :  1 側", 10, 67);
  tft.drawString("車内 :   ", 10, 100);
  tft.drawString(" 1      2      3      4      5      6", 135, 165);
  tft.drawString(" 1側扉 ", 25, 200);
  tft.drawString(" 2側扉 ", 25, 235);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.drawString("  閉    閉    閉    閉    閉    閉  ", 120, 200);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("  閉    閉    閉    閉    閉    閉  ", 120, 235);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  page = '2';//change the state page into 1
}
