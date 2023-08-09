// Font file is stored in SPIFFS
#include "FS.h"

#include "button.h"
#include "button_red.h"
#include "button_blue.h"
#include "button_red_2.h"
#include "button_purple.h"
#include "button7.h"
#include "button8.h"
#include "button9.h"
#include "button_black.h"
#include "button_red3.h"

// Graphics and font library
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();  // Invoke library
int page;

char dai1;
char dai1a;
char dai2;
char dai2a;
char dai3;
char dai3a;
char dai4;
char dai4a;
char des;
char arr;

#define CALIBRATION_FILE "/calibrationData"
#define ELEMENTS_NUM 5 /**< カンマ区切りデータの項目数 */
#define REPEAT_CAL false

// Keypad start position, key sizes and spacing --------
#define KEY_X 40 // Centre of key
#define KEY_Y 96
#define KEY_W 62 // Width and height
#define KEY_H 30
#define KEY_SPACING_X 18 // X and Y gap
#define KEY_SPACING_Y 20
#define KEY_TEXTSIZE 1   // Font size multiplier

// Using two fonts since numbers are nice when bold
#define LABEL1_FONT &FreeSansOblique12pt7b // Key label font 1
#define LABEL2_FONT &FreeSansBold12pt7b    // Key label font 2

// Numeric display box size and location
#define DISP_X 1
#define DISP_Y 10
#define DISP_W 200
#define DISP_H 50
#define DISP_TSIZE 3
#define DISP_TCOLOR TFT_CYAN

// Number length, buffer for storing it and character index
#define NUM_LEN 5
char numberBuffer[NUM_LEN + 1] = "";
uint8_t numberIndex = 0;
char sern[6] = "";
// We have a status line for messages
#define STATUS_X 120 // Centred on this
#define STATUS_Y 65
// Create 15 keys for the keypad
char keyLabel[30][8] = {"New", "Del", "Send", "1", "2", "3", "4", "5", "6", "7", "8", "9",  "0", " " , "A", "B", "C", "D", "E", "F", "G", "H", "K", "M", "P", "R", "S", "T", "Y", "Z"};
uint16_t keyColor[30] = {TFT_RED, TFT_DARKGREY, TFT_DARKGREEN, TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE,
                         TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE, TFT_BLUE,

                        };

// Invoke the TFT_eSPI button class and create all the button objects
TFT_eSPI_Button key[30];
//------------------------------------------------------------------------------------------

/**
   @brief       受信済み文字列を格納する配列
*/
static String elements[ELEMENTS_NUM];
static int received_elements_num = 0; /**< 受信済み文字列の数 */

void setup(void) {
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;

  Serial.begin(115200);
  Serial.println("starting");
  Serial.setTimeout(5);        // タイムアウトの時間を100msに変更}

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
  tft.fillScreen((0xFFFF));
  page_1();
}

void loop() {
  receive();
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
    if (pressed) {
      if ((x > 35) && (x < 205)) {
        if ((y > 135) && (y < 180)) {
          page_3();
        }
      }
    }
    if (pressed) {
      if ((x > 35) && (x < 205)) {
        if ((y > 255) && (y < 310)) {
          page_4();
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
  if (page == '3') { //first page function touch screen
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
    if ((x > 155) && (x < 455)) {
      if ((y > 60) && (y < 105)) {
        page_11();
      }
    }

    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 270) && (y < 320)) {
          page_1();
        }
      }
    }
  }
  if (page == '4') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 0) && (x < 480)) {
        if ((y > 0) && (y < 320)) {
          page_1();
        }
      }
    }
  }
    if (page == '11') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    //   int pressed = tft.getTouch(&x, &y);

    uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

    // Pressed will be set true is there is a valid touch on the screen
    bool pressed = tft.getTouch(&t_x, &t_y);

    // / Check if any key coordinate boxes contain the touch coordinates
    for (uint8_t b = 0; b < 30; b++) {
      if (pressed && key[b].contains(t_x, t_y)) {
        key[b].press(true);  // tell the button it is pressed
      } else {
        key[b].press(false);  // tell the button it is NOT pressed
      }
    }

    // Check if any key has changed state
    for (uint8_t b = 0; b < 30; b++) {

      if (b < 3) tft.setFreeFont(LABEL1_FONT);
      else tft.setFreeFont(LABEL2_FONT);

      if (key[b].justReleased()) key[b].drawButton();     // draw normal

      if (key[b].justPressed()) {
        key[b].drawButton(true);  // draw invert

        // if a numberpad button, append the relevant # to the numberBuffer
        if (b >= 3) {
          if (numberIndex < NUM_LEN) {
            numberBuffer[numberIndex] = keyLabel[b][0];
            numberIndex++;
            numberBuffer[numberIndex] = 0; // zero terminate
          }
          // status(""); // Clear the old status
        }

        // Del button, so delete last char
        if (b == 1) {
          numberBuffer[numberIndex] = 0;
          if (numberIndex > 0) {
            numberIndex--;
            numberBuffer[numberIndex] = 0;//' ';
          }
          //  status(""); // Clear the old status
        }

        if (b == 2) {
          // status("Sent value to serial port");
          Serial.println(numberBuffer);
          char* sern  = numberBuffer;
        }
        // we dont really check that the text field makes sense
        // just try to call
        if (b == 0) {
          // status("Value cleared");
          numberIndex = 0; // Reset index to 0
          numberBuffer[numberIndex] = 0; // Place null in buffer
        }

        // Update the number display field
        tft.setTextDatum(TL_DATUM);        // Use top left corner as text coord datum
        tft.setFreeFont(&FreeSans18pt7b);  // Choose a nicefont that fits box
        tft.setTextColor(DISP_TCOLOR);     // Set the font colour

        // Draw the string, the value returned is the width in pixels
        int xwidth = tft.drawString(numberBuffer, DISP_X + 4, DISP_Y + 12);

        // Now cover up the rest of the line up by drawing a black rectangle.  No flicker this way
        // but it will not work with italic or oblique fonts due to character overlap.
        tft.fillRect(DISP_X + 4 + xwidth, DISP_Y + 1, DISP_W - xwidth - 5, DISP_H - 2, TFT_BLACK);

        delay(10); // UI debouncing
      }
    }
    if (pressed) {
      if ((t_x > 230) && (t_x < 355)) {
        if ((t_y > 10) && (t_y < 60)) {
          page_3();
        }
      }
    }
  }
}
void receive() {
  String line;              // 受信文字列
  unsigned int beginIndex;  // 要素の開始位置

  // シリアルモニタやProcessingから"AB,C,DEF,12,3,45,A1,2B,-1,+127"のように
  // ELEMENTS_NUM個の文字列の間にカンマを付けて送る
  // 送信側の改行設定は「LFのみ」にすること
  // シリアル通信で1行（改行コードまで）読み込む
  line = Serial.readStringUntil('\n');

  beginIndex = 0;
  for (received_elements_num = 0; received_elements_num < ELEMENTS_NUM; received_elements_num++) {
    // 最後の要素ではない場合
    if (received_elements_num != (ELEMENTS_NUM - 1)) {
      // 要素の開始位置から，カンマの位置を検索する
      unsigned int endIndex;
      endIndex = line.indexOf(',', beginIndex);
      // カンマが見つかった場合
      if (endIndex != -1) {
        // 文字列を切り出して配列に格納する
        elements[received_elements_num] = line.substring(beginIndex, endIndex);
        // 要素の開始位置を更新する
        beginIndex = endIndex + 1;
      }
      // カンマが見つからなかった場合はfor文を中断する
      else {
        break;
      }
    }
    // 最後の要素の場合
    else {
      elements[received_elements_num] = line.substring(beginIndex);
    }
    dai1a = elements[0].toInt();
    if ( (dai1a == 1) && (dai1 != dai1a))  {
      if (page == '2') {
        tft.setTextColor(TFT_WHITE, TFT_RED);
        tft.drawString("  開    開    開    開    開    開  ", 120, 200);
      }
      dai1 = dai1a;
    }
    if ((dai1a == 0) && (dai1 != dai1a)) {
      if (page == '2') {
        tft.setTextColor(TFT_BLACK, TFT_WHITE);
        tft.drawString("  閉    閉    閉    閉    閉    閉  ", 120, 200);
      }
      dai1 = dai1a;
    }
    dai2a = elements[1].toInt();
    dai3a = elements[2].toInt();
    dai4a = elements[3].toInt();
    if ((dai2a > 0) && (dai2 != dai2a)) {
      dai2 = dai2a;
      if (page == '2') {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (dai2 == 29) {
          tft.drawString("   渋谷        ", 100, 38);
        }
        if (dai2 == 28) {
          tft.drawString("  表参道       ", 100, 38);
        }
        if (dai2 == 27) {
          tft.drawString("  外苑前       ", 100, 38);
        }
        if (dai2 == 26) {
          tft.drawString("青山一丁目  ", 100, 38);
        }
        if (dai2 == 25) {
          tft.drawString(" 赤坂見附    ", 100, 38);
        }
        if (dai2 == 24) {
          tft.drawString(" 溜池山王     ", 100, 38);
        }
        if (dai2 == 23) {
          tft.drawString("  虎ノ門       ", 100, 38);
        }
        if (dai2 == 22) {
          tft.drawString("   新橋        ", 100, 38);
        }
        if (dai2 == 21) {
          tft.drawString("   銀座        ", 100, 38);
        }
        if (dai2 == 20) {
          tft.drawString("   京橋        ", 100, 38);
        }
        if (dai2 == 19) {
          tft.drawString("  日本橋       ", 100, 38);
        }
        if (dai2 == 18) {
          tft.drawString("  三越前       ", 100, 38);
        }
        if (dai2 == 17) {
          tft.drawString("   神田        ", 100, 38);
        }
        if (dai2 == 16) {
          tft.drawString("  末広町       ", 100, 38);
        }
        if (dai2 == 15) {
          tft.drawString("上野広小路     ", 100, 38);
        }
        if (dai2 == 14) {
          tft.drawString("   上野        ", 100, 38);
        }
        if (dai2 == 13) {
          tft.drawString("  稲荷町       ", 100, 38);
        }
        if (dai2 == 12) {
          tft.drawString("  田原町       ", 100, 38);
        }
        if (dai2 == 11) {
          tft.drawString("   浅草        ", 100, 38);
        }
      }
    }
    if ((dai3a > 0) && (dai2 != dai3a) ) {
      dai2 = dai3a;
      if (page == '2') {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (dai2 == 29) {
          tft.drawString("   渋谷        ", 100, 38);
        }
        if (dai2 == 28) {
          tft.drawString("  表参道       ", 100, 38);
        }
        if (dai2 == 27) {
          tft.drawString("  外苑前       ", 100, 38);
        }
        if (dai2 == 26) {
          tft.drawString("青山一丁目     ", 100, 38);
        }
        if (dai2 == 25) {
          tft.drawString(" 赤坂見附      ", 100, 38);
        }
        if (dai2 == 24) {
          tft.drawString(" 溜池山王      ", 100, 38);
        }
        if (dai2 == 23) {
          tft.drawString("  虎ノ門       ", 100, 38);
        }
        if (dai2 == 22) {
          tft.drawString("   新橋        ", 100, 38);
        }
        if (dai2 == 21) {
          tft.drawString("   銀座        ", 100, 38);
        }
        if (dai2 == 20) {
          tft.drawString("   京橋        ", 100, 38);
        }
        if (dai2 == 19) {
          tft.drawString("  日本橋       ", 100, 38);
        }
        if (dai2 == 18) {
          tft.drawString("  三越前       ", 100, 38);
        }
        if (dai2 == 17) {
          tft.drawString("   神田        ", 100, 38);
        }
        if (dai2 == 16) {
          tft.drawString("  末広町       ", 100, 38);
        }
        if (dai2 == 15) {
          tft.drawString("上野広小路   ", 100, 38);
        }
        if (dai2 == 14) {
          tft.drawString("   上野        ", 100, 38);
        }
        if (dai2 == 13) {
          tft.drawString("  稲荷町       ", 100, 38);
        }
        if (dai2 == 12) {
          tft.drawString("  田原町       ", 100, 38);
        }
        if (dai2 == 11) {
          tft.drawString("   浅草        ", 100, 38);
        }
      }
    }
    if ((dai4a > 0) && (dai4 != dai4a) ) {
      dai4 = dai4a;
      if (page == '2') {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (dai4 == 26) {
          tft.drawString("   渋谷    ", 200, 8);
        }
        if (dai4 == 8) {
          tft.drawString("   浅草    ", 200, 8);
        }
      }
    }
  }
}
void page_1() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.loadFont("yu-gothic-28");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("項目キーを選択して下さい。", 10  , 10);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.drawString(" メニュー", 360  , 10);
  tft.pushImage(35, 75, 170, 45, button);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("  状態表示  ", 50  , 85);
  tft.pushImage(35, 135, 170, 45, button);
  tft.drawString("  着番設定  ", 50  , 145);
  tft.pushImage(35, 195, 170, 45, button);
  tft.pushImage(35, 255, 170, 45, button);
  tft.drawString("  画面消去  ", 50  , 265);
  tft.pushImage(275, 75, 170, 45, button);
  tft.drawString("  地点補正  ", 285  , 85);
  tft.pushImage(275, 135, 170, 45, button);
  tft.drawString("  前灯点滅  ", 285  , 145);
  tft.pushImage(275, 195, 170, 45, button);
  tft.pushImage(275, 255, 170, 45, button_red);
  tft.drawString("  故障記録  ", 285  , 265);
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
  tft.drawString("運行:", 10, 7);
  tft.drawString("行先:", 170, 7);
  tft.drawString(numberBuffer, 80, 7);
  tft.drawString("地点 :    ", 10, 37);

  if (dai2 == 29) {
    tft.drawString("   渋谷        ", 100, 38);
  }
  if (dai2 == 28) {
    tft.drawString("  表参道       ", 100, 38);
  }
  if (dai2 == 27) {
    tft.drawString("  外苑前       ", 100, 38);
  }
  if (dai2 == 26) {
    tft.drawString("青山一丁目  ", 100, 38);
  }
  if (dai2 == 25) {
    tft.drawString(" 赤坂見附    ", 100, 38);
  }
  if (dai2 == 24) {
    tft.drawString(" 溜池山王     ", 100, 38);
  }
  if (dai2 == 23) {
    tft.drawString("  虎ノ門       ", 100, 38);
  }
  if (dai2 == 22) {
    tft.drawString("   新橋        ", 100, 38);
  }
  if (dai2 == 21) {
    tft.drawString("   銀座        ", 100, 38);
  }
  if (dai2 == 20) {
    tft.drawString("   京橋        ", 100, 38);
  }
  if (dai2 == 19) {
    tft.drawString("  日本橋       ", 100, 38);
  }
  if (dai2 == 18) {
    tft.drawString("  三越前       ", 100, 38);
  }
  if (dai2 == 17) {
    tft.drawString("   神田        ", 100, 38);
  }
  if (dai2 == 16) {
    tft.drawString("  末広町       ", 100, 38);
  }
  if (dai2 == 15) {
    tft.drawString("上野広小路     ", 100, 38);
  }
  if (dai2 == 14) {
    tft.drawString("   上野        ", 100, 38);
  }
  if (dai2 == 13) {
    tft.drawString("  稲荷町       ", 100, 38);
  }
  if (dai2 == 12) {
    tft.drawString("  田原町       ", 100, 38);
  }
  if (dai2 == 11) {
    tft.drawString("   浅草        ", 100, 38);
  }

  if (dai4 == 26) {
    tft.drawString("   渋谷    ", 200, 8);
  }
  if (dai4 == 8) {
    tft.drawString("   浅草    ", 200, 8);
  }

  tft.drawString("終端戸締 :  1 側", 10, 67);
  tft.drawString("車内 :   ", 10, 100);
  tft.drawString(" 1      2      3      4      5      6", 135, 165);
  tft.drawString(" 1側扉 ", 25, 200);
  tft.drawString(" 2側扉 ", 25, 235);
  if (dai1 == 1) {
    tft.setTextColor(TFT_WHITE, TFT_RED);
    tft.drawString("  開    開    開    開    開    開  ", 120, 200);
  }
  if (dai1 == 0) {
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawString("  閉    閉    閉    閉    閉    閉  ", 120, 200);
  }
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.drawString("  閉    閉    閉    閉    閉    閉  ", 120, 235);
  // Set the font colour to be white with a black background, set text size multiplier to 1
  page = '2';//change the state page into 1
}
void page_3() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-21");
  tft.drawString("各項目を設定し、起動を押して", 15  , 5);
  tft.drawString("下さい。", 15  , 30);
  tft.loadFont("yu-gothic-28");
  tft.setTextColor( TFT_BLACK);
  tft.drawString("運行設定", 365  , 10);
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(35, 60, 115, 45, button7);
  tft.pushImage(35, 110, 115, 45, button7);
  tft.pushImage(35, 160, 115, 45, button7);
  tft.pushImage(35, 210, 115, 45, button7);

  tft.drawRect(35, 60, 115, 45, TFT_WHITE);
  tft.drawRect(35, 110, 115, 45, TFT_WHITE);
  tft.drawRect(35, 160, 115, 45, TFT_WHITE);
  tft.drawRect(35, 210, 115, 45, TFT_WHITE);

  tft.loadFont("yu-gothic-21");
  tft.drawString("運 行 番 号", 45  , 75);
  tft.drawString(" 行       先", 45  , 125);
  tft.drawString(" 地       点", 45  , 175);
  tft.drawString("着 番 設 定", 45  , 225);

  tft.pushImage(160, 60, 290, 45, button8);
  tft.pushImage(160, 110, 290, 45, button8);
  tft.pushImage(160, 160, 290, 45, button8);
  tft.pushImage(160, 210, 140, 45, button9);
  tft.pushImage(310, 210, 140, 45, button9);

  tft.drawRect(160, 60, 290, 45, TFT_WHITE);
  tft.drawRect(160, 110, 290, 45, TFT_WHITE);
  tft.drawRect(160, 160, 290, 45, TFT_WHITE);
  tft.drawRect(160, 210, 140, 45, TFT_WHITE);
  tft.drawRect(310, 210, 140, 45, TFT_WHITE);

  tft.setTextColor(TFT_WHITE);
  tft.drawString(numberBuffer, 230, 75);
  if (des == 1) {
    tft.drawString(" 渋      谷 ", 200  , 125);
  }
  if (des == 2) {
    tft.drawString(" 浅       草", 200  , 125);
  }
   if (des == 3) {
    tft.drawString(" 上       野", 200  , 125);
  }
  if (des == 4) {
    tft.drawString(" 方  南  町 ", 200  , 125);
  }
  if (des == 5) {
    tft.drawString(" 赤 坂 見 附  ", 200  , 125);
  }
  if (des == 6) {
    tft.drawString(" 荻     窪 ", 200  , 125);
  }
  if (des == 7) {
    tft.drawString(" 池     袋 ", 200  , 125);
  }
  if (des == 8) {
    tft.drawString(" 回     送 ", 200  , 125);
  }  
  if (arr == 1) {
    tft.drawString("1  側", 200, 225);
  }
  if (arr == 2) {
    tft.drawString("2  側", 200, 225);
  }
  if (arr == 3) {
    tft.drawString("不　定", 200, 225);
  }
  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_black);
  tft.pushImage(242, 275, 115, 45, button_purple);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("起      動", 265, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 385, 290);
  page = '3';//change the state page into 1
}
void page_4() {
  tft.fillScreen(TFT_BLACK);
  page = '4';//change the state page into 1
}
void page_11() {
  drawKeypad();


  page = '11';//change the state page into 1
}

void drawKeypad()
{
  tft.fillScreen(TFT_BLACK);
  // Draw the keys
  for (uint8_t row = 0; row < 5; row++) {
    for (uint8_t col = 0; col < 6; col++) {
      uint8_t b = col + row * 6;

      if (b < 3) tft.setFreeFont(LABEL1_FONT);
      else tft.setFreeFont(LABEL2_FONT);

      key[b].initButton(&tft, KEY_X + col * (KEY_W + KEY_SPACING_X),
                        KEY_Y + row * (KEY_H + KEY_SPACING_Y), // x, y, w, h, outline, fill, text
                        KEY_W, KEY_H, TFT_WHITE, keyColor[b], TFT_WHITE,
                        keyLabel[b], KEY_TEXTSIZE);
      key[b].drawButton();
    }
  }
  tft.fillRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_BLACK);
  tft.drawRect(DISP_X, DISP_Y, DISP_W, DISP_H, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-28");
  tft.setTextColor( TFT_BLACK);
  tft.drawString("運行番号", 365  , 10);
  tft.pushImage(230, 10, 115, 45, button_purple);
  tft.drawString("起      動", 240, 20);
}

// -------------------------------------------------------------------------
// List files in ESP8266 or ESP32 SPIFFS memory
// -------------------------------------------------------------------------
void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

#ifdef ESP32
  listDir(SPIFFS, "/", true);
#else
  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 25 - fileName.length(); // Tabulate nicely
    if (spaces < 0) spaces = 1;
    while (spaces--) Serial.print(" ");
    fs::File f = dir.openFile("r");
    Serial.print(f.size()); Serial.println(" bytes");
    yield();
  }
  Serial.println(line);
#endif
  Serial.println();
  delay(1000);
}

#ifdef ESP32
void listDir(fs::FS & fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  fs::File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  fs::File file = root.openNextFile();
  while (file) {

    if (file.isDirectory()) {
      Serial.print("DIR : ");
      String fileName = file.name();
      Serial.print(fileName);
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      String fileName = file.name();
      Serial.print("  " + fileName);
      int spaces = 32 - fileName.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      String fileSize = (String) file.size();
      spaces = 8 - fileSize.length(); // Tabulate nicely
      if (spaces < 1) spaces = 1;
      while (spaces--) Serial.print(" ");
      Serial.println(fileSize + " bytes");
    }

    file = root.openNextFile();
  }
}
#endif
// -------------------------------------------------------------------------
