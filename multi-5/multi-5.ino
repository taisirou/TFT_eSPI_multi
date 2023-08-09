// RP2040 Font file is stored in SPIFFS
//#define FS_NO_GLOBALS
//#include <FS.h>

//ESP32
#include "FS.h"

#include "button.h"
#include "button_red.h"
#include "button_red_2.h"
#include "button_red3.h"
#include "button_blue.h"
#include "button_black.h"
#include "button_purple.h"
#include "button7.h"
#include "button8.h"
#include "button9.h"
#include "button10.h"
// Graphics and font library
#include <TFT_eSPI.h>
#include <SPI.h>
//#include <SoftwareSerial.h>       // ソフトウェアシリアルのヘッダーファイルを準備
//SoftwareSerial mySerial(4, 5);  // RX, TX（ソフトウェアシリアル）

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
char rou;

#define CALIBRATION_FILE "/calibrationData"
#define ELEMENTS_NUM 3 /**< カンマ区切りデータの項目数 */
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
unsigned char stop[4] = {0xAA, 0x04, 0x00, 0xAE};

void setup(void) {
  uint16_t calibrationData[5];
  uint8_t calDataOK = 0;

//  Serial2.setTX(8);
//  Serial2.setRX(9);
//  Serial2.begin(9600);
//  volume(0x1E);//Volume settings 0x00-0x1E
//  mySerial.begin(9600);   // ソフトウェアシリアル

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
//  listFiles(); // Lists the files so you can see what is in the SPIFFS

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
    if (pressed) {
      if ((x > 35) && (x < 205)) {
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
    if (pressed) {
      if ((x > 275) && (x < 450)) {
        if ((y > 75) && (y < 120)) {
          page_8();
        }
      }
    }
    if (pressed) {
      if ((x > 275) && (x < 450)) {
        if ((y > 135) && (y < 180)) {
          page_6();
        }
      }
    }
  }
  if (page == '2') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 250) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 110) && (x < 240)) {
        if ((y > 250) && (y < 320)) {
          page_3();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 375)) {
        if ((y > 250) && (y < 320)) {
          page_7();
        }
      }
    }
  }
  if (page == '3') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 155) && (x < 455)) {
        if ((y > 60) && (y < 105)) {
          page_11();
        }
      }
    }
    if (pressed) {
      if ((x > 155) && (x < 455)) {
        if ((y > 110) && (y < 155)) {
          page_12();
        }
      }
    }
    if (pressed) {
      if ((x > 155) && (x < 455)) {
        if ((y > 160) && (y < 205)) {
          page_8();
        }
      }
    }
    if (pressed) {
      if ((x > 155) && (x < 455)) {
        if ((y > 210) && (y < 255)) {
          page_13();
        }
      }
    }
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 250) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 110) && (x < 240)) {
        if ((y > 250) && (y < 320)) {
          page_10();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 250) && (y < 320)) {
          page_2();
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
  if (page == '6') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 250) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 110) && (x < 240)) {
        if ((y > 250) && (y < 320)) {
          page_3();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 250) && (y < 320)) {
          page_2();
        }
      }
    }
  }
  if (page == '7') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 250) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 110) && (x < 240)) {
        if ((y > 250) && (y < 320)) {
          page_10();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 250) && (y < 320)) {
          page_2();
        }
      }
    }
  }
  if (page == '8') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 55) && (y < 105)) {
          rou = 1;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 55) && (y < 105)) {
          rou = 2;
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 115) && (y < 160)) {
          rou = 3;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 115) && (y < 160)) {
          rou = 4;
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 170) && (y < 215)) {
          rou = 5;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 170) && (y < 215)) {
          rou = 6;
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 225) && (y < 270)) {
          rou = 7;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 225) && (y < 270)) {
          rou = 0;
        }
      }
    }
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 275) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 275) && (y < 320)) {
          page_3();
        }
      }
    }
  }
  if (page == '10') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 55) && (y < 105)) {
          play(0x01);
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 55) && (y < 105)) {
          play(0x02);
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 115) && (y < 160)) {
          play(0x03);
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 115) && (y < 160)) {
          play(0x04);
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 170) && (y < 215)) {
          play(0x05);
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 225) && (y < 270)) {
 //         Serial2.write(stop, 4); //order play        }
        }
      }
    }
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 275) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 275) && (y < 320)) {
          page_7();
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
  if (page == '12') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 55) && (y < 105)) {
          des = 1;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 55) && (y < 105)) {
          des = 2;
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 115) && (y < 160)) {
          des = 3;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 115) && (y < 160)) {
          des = 4;
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 170) && (y < 215)) {
          des = 5;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 170) && (y < 215)) {
          des = 6;
        }
      }
    }
    if (pressed) {
      if ((x > 45) && (x < 230)) {
        if ((y > 225) && (y < 270)) {
          des = 7;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 465)) {
        if ((y > 225) && (y < 270)) {
          des = 8;
        }
      }
    }
    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 275) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 275) && (y < 320)) {
          page_3();
        }
      }
    }
  }
  if (page == '13') { //first page function touch screen
    uint16_t x, y;
    static uint16_t color;
    int pressed = tft.getTouch(&x, &y);
    if (pressed) {
      if ((x > 45) && (x < 200)) {
        if ((y > 95) && (y < 150)) {
          arr = 1;
        }
      }
    }
    if (pressed) {
      if ((x > 285) && (x < 440)) {
        if ((y > 95) && (y < 150)) {
          arr = 2;
        }
      }
    }
    if (pressed) {
      if ((x > 160) && (x < 320)) {
        if ((y > 195) && (y < 250)) {
          arr = 3;
        }
      }
    }


    if (pressed) {
      if ((x > 0) && (x < 105)) {
        if ((y > 275) && (y < 320)) {
          page_1();
        }
      }
    }
    if (pressed) {
      if ((x > 245) && (x < 355)) {
        if ((y > 275) && (y < 320)) {
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
        if (rou == 0) {
          tft.drawString("          ", 100, 38);
        }
        if (rou == 1) {
          if (dai2 == 2) {
            tft.drawString("   浅草        ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("   業平橋        ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString("   曳舟        ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString("   東向島        ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString("   鐘ヶ淵        ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString("   堀切        ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString("   牛田        ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString("   北千住        ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString("   小菅        ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("   五反野        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("   梅島        ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("   西新井        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   竹ノ塚        ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("   谷塚        ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("   草加        ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   松原団地     ", 100, 38);
          }
          if (dai2 == 18) {
            tft.drawString("   新田        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   蒲生        ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   新越谷        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   越谷        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   北越谷        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("   大袋        ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("   せんげん台    ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("   武里        ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString("   一ノ割       ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("   春日部        ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("   北春日部      ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("   姫宮        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   東武動物公園    ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   和戸        ", 100, 38);
          }
          if (dai2 == 32) {
            tft.drawString("   久喜        ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString("   東武動物公園    ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   杉戸高野台     ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   幸手        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString("   南栗橋        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString("   池袋        ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("   北池袋        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("   下板橋        ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   大山        ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("   中板橋       ", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("   ときわ台     ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   上板橋        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("   東武練馬     ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString("   下赤塚      ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString("   成増        ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString("   和光市      ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   朝霞        ", 100, 38);
          }
          if (dai2 == 54) {
            tft.drawString("   朝霞台       ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString("   志木        ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("   柳瀬川       ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString("   みずほ台     ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString("   鶴瀬       ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   ふじみ野       ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString("   上福岡        ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("   新河岸       ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("   川越        ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("   川越市      ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("   霞ヶ関        ", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString("   鶴ヶ島        ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("   若葉        ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString("   坂戸        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   北坂戸       ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString("   高坂        ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("   東松山        ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString("   森林公園      ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("   つきのわ      ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("   武蔵嵐山      ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString("   小川町      ", 100, 38);
          }
        }
        if (rou == 2) {
          if (dai2 == 2) {
            tft.drawString("   小竹向原      ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("   新桜台       ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString("   練馬        ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString("   池袋        ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString("   椎名町        ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString("   東長崎        ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString("   江古田        ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString("   桜台        ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("   練馬        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("   中村橋        ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("   富士見台        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   練馬高野台      ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("   石神井公園        ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("   大泉学園        ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   保谷     ", 100, 38);
          }
          if (dai2 == 18) {
            tft.drawString("   ひばりヶ丘       ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   東久留米       ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   清瀬        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   秋津        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   所沢        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("   西所沢    ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("   小手指    ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("   狭山ヶ丘    ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString("  武蔵藤沢     ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("  稲荷山公園      ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("   入間市      ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("   仏子        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   元加治     ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   飯能        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   西武新宿     ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   高田馬場        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString("   下落合        ", 100, 38);
          }
          if (dai2 == 38) {
            tft.drawString("   中井        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString(" 新井薬師前   ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString("   沼袋        ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString("   野方        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString("  都立家政      ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("   鷺ノ宮        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("   下井草        ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   井荻        ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("   上井草       ", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("   上石神井     ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   武蔵関        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("   東伏見     ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString("   西武柳沢      ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString("   田無        ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString("   花小金井      ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   小平        ", 100, 38);
          }
          if (dai2 == 54) {
            tft.drawString("   久米川       ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString("   東村山        ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("   所沢       ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString("   航空公園     ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString("   新所沢       ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   入曽       ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString("   狭山市        ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("   新狭山       ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("   南大塚        ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("   本川越      ", 100, 38);
          }

          if (dai2 == 67) {
            tft.drawString("   小平        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   萩山       ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString("   小川        ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("   東大和市    ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString("   玉川上水    ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("   武蔵砂川   ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("  西武立川     ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString("   拝島      ", 100, 38);
          }
        }
        if (rou == 3) {
          if (dai2 == 2) {
            tft.drawString("   和光市     ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString(" 地下鉄成増   ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString(" 地下鉄赤塚    ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString("   平和台      ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString("   氷川台      ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString(" 小竹向原    ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString("  千川       ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString("   要町      ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString("   池袋       ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("  雑司が谷      ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("  西早稲田     ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("   東新宿       ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("  新宿三丁目   ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("  北参道       ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("  明治神宮前     ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("  小竹向原     ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   千川       ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   要町        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   池袋        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("   東池袋      ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("   護国寺       ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("  江戸川橋       ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString("  飯田橋       ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("   市ヶ谷       ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("   麹町       ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("   永田町       ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   桜田門      ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   有楽町       ", 100, 38);
          }
          if (dai2 == 32) {
            tft.drawString(" 銀座一丁目   ", 100, 38);
          }
          if (dai2 == 33) {
            tft.drawString("   新富町      ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString("  月島        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   豊洲        ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   辰巳        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString("   新木場        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString("   表参道      ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString(" 青山一丁目    ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString("   永田町       ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("  半蔵門       ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("   九段下       ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   神保町      ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("   大手町       ", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("   三越前       ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString(" 水天宮前     ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString(" 清澄白河     ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString("   住吉        ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString("  錦糸町      ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString("   押上        ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   曳舟        ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString(" 北千住      ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString(" 南千住       ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString(" 三ノ輪       ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString("   入谷        ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   上野        ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString(" 仲御徒町       ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("  秋葉原       ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString(" 小伝馬町      ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("  人形町       ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("  茅場町       ", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString("  八丁堀       ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("   築地        ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString("  東銀座        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   銀座        ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString("  日比谷       ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("  霞ヶ関        ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString("  神谷町       ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("  六本木       ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("   広尾        ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString(" 恵比寿       ", 100, 38);
          }
          if (dai2 == 75) {
            tft.drawString(" 中目黒        ", 100, 38);
          }
        }
        if (rou == 4) {
          if (dai2 == 2) {
            tft.drawString("  西船橋        ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("   船橋        ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString(" 東船橋        ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString(" 津田沼        ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("   三鷹        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString(" 吉祥寺        ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString(" 西荻窪        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   荻窪        ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString(" 阿佐ヶ谷    ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("  高円寺      ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   中野        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   中野        ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   落合        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString(" 高田馬場  ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString(" 早稲田      ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString(" 神楽坂       ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString(" 飯田橋        ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString(" 九段下       ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString(" 竹橋        ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString(" 大手町        ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("  日本橋        ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("  茅場町        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString(" 門前仲町   ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   木場        ", 100, 38);
          }
          if (dai2 == 32) {
            tft.drawString(" 東陽町        ", 100, 38);
          }
          if (dai2 == 33) {
            tft.drawString(" 南砂町        ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString(" 西葛西        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   葛西        ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   浦安        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString(" 南行徳        ", 100, 38);
          }
          if (dai2 == 38) {
            tft.drawString("   行徳        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString("   妙典        ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString("原木中山   ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString(" 西船橋        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString(" 東海神        ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString(" 飯山満        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("北習志野  ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("船橋日大前 ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("八千代緑が丘", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("八千代中央", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   村上        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("東葉勝田台 ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("  北綾瀬        ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString("   綾瀬        ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString(" 北千住        ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   町屋        ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString("西日暮里   ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString(" 千駄木        ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("   根津        ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("   湯島        ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("新御茶ノ水", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString(" 大手町        ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("二重橋前       ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString(" 日比谷        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("  霞ヶ関        ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString(" 国会議事堂前", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("   赤坂        ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString(" 乃木坂        ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("  表参道        ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString(" 明治神宮前   ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString(" 代々木公園    ", 100, 38);
          }
          if (dai2 == 75) {
            tft.drawString(" 代々木上原   ", 100, 38);
          }
        }
        if (rou == 5) {
          if (dai2 == 2) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("  代官山        ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString("  中目黒       ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString("  祐天寺       ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString(" 学芸大学  ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString(" 都立大学  ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString(" 自由が丘  ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString(" 田園調布  ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString(" 多摩川       ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString(" 新丸子        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("武蔵小杉  ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("  元住吉        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   日吉        ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("   綱島        ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString(" 大倉山        ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   菊名        ", 100, 38);
          }
          if (dai2 == 18) {
            tft.drawString(" 妙蓮寺        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   白楽        ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString(" 東白楽        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   反町        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   横浜        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("  新高島        ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("みなとみらい ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("  馬車道        ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString(" 日本大通     ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("元町・中華街  ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString(" 長津田        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   恩田        ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("こどもの国  ", 100, 38);
          }
          if (dai2 == 33) {
            tft.drawString("   横浜        ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString(" 高島町        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("  桜木町        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString(" 白金高輪        ", 100, 38);
          }
          if (dai2 == 38) {
            tft.drawString(" 白金台        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString("   目黒        ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString(" 不動前        ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString(" 武蔵小山        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString(" 西小山        ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("   洗足        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString(" 大岡山        ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   奥沢        ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString(" 田園調布      ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("池尻大橋  ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString(" 三軒茶屋   ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString(" 駒沢大学    ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString(" 桜新町        ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   用賀        ", 100, 38);
          }
          if (dai2 == 54) {
            tft.drawString(" 二子玉川    ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString(" 二子新地    ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("   高津        ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString(" 溝の口        ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString(" 梶が谷        ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString(" 宮崎台        ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString(" 宮前平        ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("   鷺沼        ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("たまプラーザ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString(" あざみ野    ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("   江田        ", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString(" 市が尾       ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("  藤が丘       ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString("  青葉台       ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   田奈        ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString(" 長津田        ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString(" つくし野    ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString(" すずかけ台   ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("  南町田       ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("  つきみ野      ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString("  中央林間     ", 100, 38);
          }
        }
        if ((dai2 == 29) && (rou == 6)) {
          tft.drawString("   渋谷        ", 100, 38);
        }
        if ((dai2 == 28)  && (rou == 6)) {
          tft.drawString("  表参道       ", 100, 38);
        }
        if ((dai2 == 27) && (rou == 6)) {
          tft.drawString("  外苑前       ", 100, 38);
        }
        if ((dai2 == 26) && (rou == 6)) {
          tft.drawString("青山一丁目     ", 100, 38);
        }
        if ((dai2 == 25) && (rou == 6)) {
          tft.drawString(" 赤坂見附      ", 100, 38);
        }
        if ((dai2 == 24) && (rou == 6)) {
          tft.drawString(" 溜池山王      ", 100, 38);
        }
        if ((dai2 == 23) && (rou == 6)) {
          tft.drawString("  虎ノ門       ", 100, 38);
        }
        if ((dai2 == 22) && (rou == 6)) {
          tft.drawString("   新橋        ", 100, 38);
        }
        if ((dai2 == 21)  && (rou == 6)) {
          tft.drawString("   銀座        ", 100, 38);
        }
        if ((dai2 == 20)  && (rou == 6)) {
          tft.drawString("   京橋        ", 100, 38);
        }
        if ((dai2 == 19)  && (rou == 6)) {
          tft.drawString("  日本橋       ", 100, 38);
        }
        if ((dai2 == 18) && (rou == 6)) {
          tft.drawString("  三越前       ", 100, 38);
        }
        if ((dai2 == 17)  && (rou == 6)) {
          tft.drawString("   神田        ", 100, 38);
        }
        if ((dai2 == 16)  && (rou == 6)) {
          tft.drawString("  末広町       ", 100, 38);
        }
        if ((dai2 == 15)  && (rou == 6)) {
          tft.drawString("上野広小路   ", 100, 38);
        }
        if ((dai2 == 14)  && (rou == 6)) {
          tft.drawString("   上野        ", 100, 38);
        }
        if ((dai2 == 13) && (rou == 6)) {
          tft.drawString("  稲荷町       ", 100, 38);
        }
        if ((dai2 == 12)  && (rou == 6)) {
          tft.drawString("  田原町       ", 100, 38);
        }
        if ((dai2 == 11) && (rou == 6)) {
          tft.drawString("   浅草        ", 100, 38);
        }
      }
    }
    if ((dai3a > 0) && (dai2 != dai3a) ) {
      dai2 = dai3a;
      if (page == '2') {
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        if (rou == 0) {
          tft.drawString("          ", 100, 38);
        }
        if (rou == 1) {
          if (dai2 == 2) {
            tft.drawString("   浅草        ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("   業平橋        ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString("   曳舟        ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString("   東向島        ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString("   鐘ヶ淵        ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString("   堀切        ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString("   牛田        ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString("   北千住        ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString("   小菅        ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("   五反野        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("   梅島        ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("   西新井        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   竹ノ塚        ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("   谷塚        ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("   草加        ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   松原団地     ", 100, 38);
          }
          if (dai2 == 18) {
            tft.drawString("   新田        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   蒲生        ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   新越谷        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   越谷        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   北越谷        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("   大袋        ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("   せんげん台    ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("   武里        ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString("   一ノ割       ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("   春日部        ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("   北春日部      ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("   姫宮        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   東武動物公園    ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   和戸        ", 100, 38);
          }
          if (dai2 == 32) {
            tft.drawString("   久喜        ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString("   東武動物公園    ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   杉戸高野台     ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   幸手        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString("   南栗橋        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString("   池袋        ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("   北池袋        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("   下板橋        ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   大山        ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("   中板橋       ", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("   ときわ台     ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   上板橋        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("   東武練馬     ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString("   下赤塚      ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString("   成増        ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString("   和光市      ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   朝霞        ", 100, 38);
          }
          if (dai2 == 54) {
            tft.drawString("   朝霞台       ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString("   志木        ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("   柳瀬川       ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString("   みずほ台     ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString("   鶴瀬       ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   ふじみ野       ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString("   上福岡        ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("   新河岸       ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("   川越        ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("   川越市      ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("   霞ヶ関        ", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString("   鶴ヶ島        ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("   若葉        ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString("   坂戸        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   北坂戸       ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString("   高坂        ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("   東松山        ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString("   森林公園      ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("   つきのわ      ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("   武蔵嵐山      ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString("   小川町      ", 100, 38);
          }
        }
        if (rou == 2) {
          if (dai2 == 2) {
            tft.drawString("   小竹向原      ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("   新桜台       ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString("   練馬        ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString("   池袋        ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString("   椎名町        ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString("   東長崎        ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString("   江古田        ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString("   桜台        ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("   練馬        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("   中村橋        ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("   富士見台        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   練馬高野台      ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("   石神井公園        ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("   大泉学園        ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   保谷     ", 100, 38);
          }
          if (dai2 == 18) {
            tft.drawString("   ひばりヶ丘       ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   東久留米       ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   清瀬        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   秋津        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   所沢        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("   西所沢    ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("   小手指    ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("   狭山ヶ丘    ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString("  武蔵藤沢     ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("  稲荷山公園      ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("   入間市      ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("   仏子        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   元加治     ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   飯能        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   西武新宿     ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   高田馬場        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString("   下落合        ", 100, 38);
          }
          if (dai2 == 38) {
            tft.drawString("   中井        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString(" 新井薬師前   ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString("   沼袋        ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString("   野方        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString("  都立家政      ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("   鷺ノ宮        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("   下井草        ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   井荻        ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("   上井草       ", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("   上石神井     ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   武蔵関        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("   東伏見     ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString("   西武柳沢      ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString("   田無        ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString("   花小金井      ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   小平        ", 100, 38);
          }
          if (dai2 == 54) {
            tft.drawString("   久米川       ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString("   東村山        ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("   所沢       ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString("   航空公園     ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString("   新所沢       ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   入曽       ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString("   狭山市        ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("   新狭山       ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("   南大塚        ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("   本川越      ", 100, 38);
          }

          if (dai2 == 67) {
            tft.drawString("   小平        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   萩山       ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString("   小川        ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("   東大和市    ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString("   玉川上水    ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("   武蔵砂川   ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("  西武立川     ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString("   拝島      ", 100, 38);
          }
        }
        if (rou == 3) {
          if (dai2 == 2) {
            tft.drawString("   和光市     ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString(" 地下鉄成増   ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString(" 地下鉄赤塚    ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString("   平和台      ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString("   氷川台      ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString(" 小竹向原    ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString("  千川       ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString("   要町      ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString("   池袋       ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("  雑司が谷      ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("  西早稲田     ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("   東新宿       ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("  新宿三丁目   ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("  北参道       ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("  明治神宮前     ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("  小竹向原     ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   千川       ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   要町        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   池袋        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("   東池袋      ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("   護国寺       ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("  江戸川橋       ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString("  飯田橋       ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("   市ヶ谷       ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("   麹町       ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("   永田町       ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   桜田門      ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   有楽町       ", 100, 38);
          }
          if (dai2 == 32) {
            tft.drawString(" 銀座一丁目   ", 100, 38);
          }
          if (dai2 == 33) {
            tft.drawString("   新富町      ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString("  月島        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   豊洲        ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   辰巳        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString("   新木場        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString("   表参道      ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString(" 青山一丁目    ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString("   永田町       ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("  半蔵門       ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("   九段下       ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   神保町      ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("   大手町       ", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("   三越前       ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString(" 水天宮前     ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString(" 清澄白河     ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString("   住吉        ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString("  錦糸町      ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString("   押上        ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   曳舟        ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString(" 北千住      ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString(" 南千住       ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString(" 三ノ輪       ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString("   入谷        ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   上野        ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString(" 仲御徒町       ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("  秋葉原       ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString(" 小伝馬町      ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("  人形町       ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("  茅場町       ", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString("  八丁堀       ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("   築地        ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString("  東銀座        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   銀座        ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString("  日比谷       ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("  霞ヶ関        ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString("  神谷町       ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("  六本木       ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("   広尾        ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString(" 恵比寿       ", 100, 38);
          }
          if (dai2 == 75) {
            tft.drawString(" 中目黒        ", 100, 38);
          }
        }
        if (rou == 4) {
          if (dai2 == 2) {
            tft.drawString("  西船橋        ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("   船橋        ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString(" 東船橋        ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString(" 津田沼        ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString("   三鷹        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString(" 吉祥寺        ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString(" 西荻窪        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   荻窪        ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString(" 阿佐ヶ谷    ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString("  高円寺      ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   中野        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   中野        ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString("   落合        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString(" 高田馬場  ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString(" 早稲田      ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString(" 神楽坂       ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString(" 飯田橋        ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString(" 九段下       ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString(" 竹橋        ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString(" 大手町        ", 100, 38);
          }
          if (dai2 == 28) {
            tft.drawString("  日本橋        ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString("  茅場町        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString(" 門前仲町   ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("   木場        ", 100, 38);
          }
          if (dai2 == 32) {
            tft.drawString(" 東陽町        ", 100, 38);
          }
          if (dai2 == 33) {
            tft.drawString(" 南砂町        ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString(" 西葛西        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("   葛西        ", 100, 38);
          }
          if (dai2 == 36) {
            tft.drawString("   浦安        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString(" 南行徳        ", 100, 38);
          }
          if (dai2 == 38) {
            tft.drawString("   行徳        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString("   妙典        ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString("原木中山   ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString(" 西船橋        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString(" 東海神        ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString(" 飯山満        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString("北習志野  ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("船橋日大前 ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString("八千代緑が丘", 100, 38);
          }
          if (dai2 == 47) {
            tft.drawString("八千代中央", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   村上        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("東葉勝田台 ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("  北綾瀬        ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString("   綾瀬        ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString(" 北千住        ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString("   町屋        ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString("西日暮里   ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString(" 千駄木        ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("   根津        ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString("   湯島        ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("新御茶ノ水", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString(" 大手町        ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("二重橋前       ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString(" 日比谷        ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("  霞ヶ関        ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString(" 国会議事堂前", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString("   赤坂        ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString(" 乃木坂        ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("  表参道        ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString(" 明治神宮前   ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString(" 代々木公園    ", 100, 38);
          }
          if (dai2 == 75) {
            tft.drawString(" 代々木上原   ", 100, 38);
          }
        }
        if (rou == 5) {
          if (dai2 == 2) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 3) {
            tft.drawString("  代官山        ", 100, 38);
          }
          if (dai2 == 4) {
            tft.drawString("  中目黒       ", 100, 38);
          }
          if (dai2 == 5) {
            tft.drawString("  祐天寺       ", 100, 38);
          }
          if (dai2 == 6) {
            tft.drawString(" 学芸大学  ", 100, 38);
          }
          if (dai2 == 7) {
            tft.drawString(" 都立大学  ", 100, 38);
          }
          if (dai2 == 8) {
            tft.drawString(" 自由が丘  ", 100, 38);
          }
          if (dai2 == 9) {
            tft.drawString(" 田園調布  ", 100, 38);
          }
          if (dai2 == 10) {
            tft.drawString(" 多摩川       ", 100, 38);
          }
          if (dai2 == 11) {
            tft.drawString(" 新丸子        ", 100, 38);
          }
          if (dai2 == 12) {
            tft.drawString("武蔵小杉  ", 100, 38);
          }
          if (dai2 == 13) {
            tft.drawString("  元住吉        ", 100, 38);
          }
          if (dai2 == 14) {
            tft.drawString("   日吉        ", 100, 38);
          }
          if (dai2 == 15) {
            tft.drawString("   綱島        ", 100, 38);
          }
          if (dai2 == 16) {
            tft.drawString(" 大倉山        ", 100, 38);
          }
          if (dai2 == 17) {
            tft.drawString("   菊名        ", 100, 38);
          }
          if (dai2 == 18) {
            tft.drawString(" 妙蓮寺        ", 100, 38);
          }
          if (dai2 == 19) {
            tft.drawString("   白楽        ", 100, 38);
          }
          if (dai2 == 20) {
            tft.drawString(" 東白楽        ", 100, 38);
          }
          if (dai2 == 21) {
            tft.drawString("   反町        ", 100, 38);
          }
          if (dai2 == 22) {
            tft.drawString("   横浜        ", 100, 38);
          }
          if (dai2 == 23) {
            tft.drawString("  新高島        ", 100, 38);
          }
          if (dai2 == 24) {
            tft.drawString("みなとみらい ", 100, 38);
          }
          if (dai2 == 25) {
            tft.drawString("  馬車道        ", 100, 38);
          }
          if (dai2 == 26) {
            tft.drawString(" 日本大通     ", 100, 38);
          }
          if (dai2 == 27) {
            tft.drawString("元町・中華街  ", 100, 38);
          }
          if (dai2 == 29) {
            tft.drawString(" 長津田        ", 100, 38);
          }
          if (dai2 == 30) {
            tft.drawString("   恩田        ", 100, 38);
          }
          if (dai2 == 31) {
            tft.drawString("こどもの国  ", 100, 38);
          }
          if (dai2 == 33) {
            tft.drawString("   横浜        ", 100, 38);
          }
          if (dai2 == 34) {
            tft.drawString(" 高島町        ", 100, 38);
          }
          if (dai2 == 35) {
            tft.drawString("  桜木町        ", 100, 38);
          }
          if (dai2 == 37) {
            tft.drawString(" 白金高輪        ", 100, 38);
          }
          if (dai2 == 38) {
            tft.drawString(" 白金台        ", 100, 38);
          }
          if (dai2 == 39) {
            tft.drawString("   目黒        ", 100, 38);
          }
          if (dai2 == 40) {
            tft.drawString(" 不動前        ", 100, 38);
          }
          if (dai2 == 41) {
            tft.drawString(" 武蔵小山        ", 100, 38);
          }
          if (dai2 == 42) {
            tft.drawString(" 西小山        ", 100, 38);
          }
          if (dai2 == 43) {
            tft.drawString("   洗足        ", 100, 38);
          }
          if (dai2 == 44) {
            tft.drawString(" 大岡山        ", 100, 38);
          }
          if (dai2 == 45) {
            tft.drawString("   奥沢        ", 100, 38);
          }
          if (dai2 == 46) {
            tft.drawString(" 田園調布      ", 100, 38);
          }
          if (dai2 == 48) {
            tft.drawString("   渋谷        ", 100, 38);
          }
          if (dai2 == 49) {
            tft.drawString("池尻大橋  ", 100, 38);
          }
          if (dai2 == 50) {
            tft.drawString(" 三軒茶屋   ", 100, 38);
          }
          if (dai2 == 51) {
            tft.drawString(" 駒沢大学    ", 100, 38);
          }
          if (dai2 == 52) {
            tft.drawString(" 桜新町        ", 100, 38);
          }
          if (dai2 == 53) {
            tft.drawString("   用賀        ", 100, 38);
          }
          if (dai2 == 54) {
            tft.drawString(" 二子玉川    ", 100, 38);
          }
          if (dai2 == 55) {
            tft.drawString(" 二子新地    ", 100, 38);
          }
          if (dai2 == 56) {
            tft.drawString("   高津        ", 100, 38);
          }
          if (dai2 == 57) {
            tft.drawString(" 溝の口        ", 100, 38);
          }
          if (dai2 == 58) {
            tft.drawString(" 梶が谷        ", 100, 38);
          }
          if (dai2 == 59) {
            tft.drawString(" 宮崎台        ", 100, 38);
          }
          if (dai2 == 60) {
            tft.drawString(" 宮前平        ", 100, 38);
          }
          if (dai2 == 61) {
            tft.drawString("   鷺沼        ", 100, 38);
          }
          if (dai2 == 62) {
            tft.drawString("たまプラーザ", 100, 38);
          }
          if (dai2 == 63) {
            tft.drawString(" あざみ野    ", 100, 38);
          }
          if (dai2 == 64) {
            tft.drawString("   江田        ", 100, 38);
          }
          if (dai2 == 65) {
            tft.drawString(" 市が尾       ", 100, 38);
          }
          if (dai2 == 66) {
            tft.drawString("  藤が丘       ", 100, 38);
          }
          if (dai2 == 67) {
            tft.drawString("  青葉台       ", 100, 38);
          }
          if (dai2 == 68) {
            tft.drawString("   田奈        ", 100, 38);
          }
          if (dai2 == 69) {
            tft.drawString(" 長津田        ", 100, 38);
          }
          if (dai2 == 70) {
            tft.drawString(" つくし野    ", 100, 38);
          }
          if (dai2 == 71) {
            tft.drawString(" すずかけ台   ", 100, 38);
          }
          if (dai2 == 72) {
            tft.drawString("  南町田       ", 100, 38);
          }
          if (dai2 == 73) {
            tft.drawString("  つきみ野      ", 100, 38);
          }
          if (dai2 == 74) {
            tft.drawString("  中央林間     ", 100, 38);
          }
        }
        if ((dai2 == 29) && (rou == 6)) {
          tft.drawString("   渋谷        ", 100, 38);
        }
        if ((dai2 == 28)  && (rou == 6)) {
          tft.drawString("  表参道       ", 100, 38);
        }
        if ((dai2 == 27) && (rou == 6)) {
          tft.drawString("  外苑前       ", 100, 38);
        }
        if ((dai2 == 26) && (rou == 6)) {
          tft.drawString("青山一丁目     ", 100, 38);
        }
        if ((dai2 == 25) && (rou == 6)) {
          tft.drawString(" 赤坂見附      ", 100, 38);
        }
        if ((dai2 == 24) && (rou == 6)) {
          tft.drawString(" 溜池山王      ", 100, 38);
        }
        if ((dai2 == 23) && (rou == 6)) {
          tft.drawString("  虎ノ門       ", 100, 38);
        }
        if ((dai2 == 22) && (rou == 6)) {
          tft.drawString("   新橋        ", 100, 38);
        }
        if ((dai2 == 21)  && (rou == 6)) {
          tft.drawString("   銀座        ", 100, 38);
        }
        if ((dai2 == 20)  && (rou == 6)) {
          tft.drawString("   京橋        ", 100, 38);
        }
        if ((dai2 == 19)  && (rou == 6)) {
          tft.drawString("  日本橋       ", 100, 38);
        }
        if ((dai2 == 18) && (rou == 6)) {
          tft.drawString("  三越前       ", 100, 38);
        }
        if ((dai2 == 17)  && (rou == 6)) {
          tft.drawString("   神田        ", 100, 38);
        }
        if ((dai2 == 16)  && (rou == 6)) {
          tft.drawString("  末広町       ", 100, 38);
        }
        if ((dai2 == 15)  && (rou == 6)) {
          tft.drawString("上野広小路   ", 100, 38);
        }
        if ((dai2 == 14)  && (rou == 6)) {
          tft.drawString("   上野        ", 100, 38);
        }
        if ((dai2 == 13) && (rou == 6)) {
          tft.drawString("  稲荷町       ", 100, 38);
        }
        if ((dai2 == 12)  && (rou == 6)) {
          tft.drawString("  田原町       ", 100, 38);
        }
        if ((dai2 == 11) && (rou == 6)) {
          tft.drawString("   浅草        ", 100, 38);
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
  tft.fillRect(360, 0, 480, 65, TFT_YELLOW);
  tft.drawLine(0, 35, 360, 35, TFT_WHITE);
  tft.drawLine(0, 65, 480, 65, TFT_WHITE);
  tft.drawLine(360, 0, 360, 65, TFT_WHITE);
  tft.drawRect(0, 95, 480, 280, TFT_WHITE);
  tft.loadFont("yu-gothic-36");
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.drawString("状  態", 380  , 20);
  tft.fillRect(20, 195, 95, 70, TFT_DARKGREEN);
  tft.fillRect(115, 195, 350, 70, TFT_WHITE);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_blue);
  tft.pushImage(242, 275, 115, 45, button_blue);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("運行設定", 134, 290);
  tft.drawString("放送・表示", 248, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 380, 290);
  tft.loadFont("yu-gothic-28");
  // Set the font colour to be white with a black background, set text size multiplier to 1
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("運行:", 10, 7);
  tft.drawString("行先:", 170, 7);
  tft.drawString(numberBuffer, 80, 7);
  tft.drawString("地点 :    ", 10, 37);
  if (rou == 1) {
    if (dai2 == 2) {
      tft.drawString("   浅草        ", 100, 38);
    }
    if (dai2 == 3) {
      tft.drawString("   業平橋        ", 100, 38);
    }
    if (dai2 == 4) {
      tft.drawString("   曳舟        ", 100, 38);
    }
    if (dai2 == 5) {
      tft.drawString("   東向島        ", 100, 38);
    }
    if (dai2 == 6) {
      tft.drawString("   鐘ヶ淵        ", 100, 38);
    }
    if (dai2 == 7) {
      tft.drawString("   堀切        ", 100, 38);
    }
    if (dai2 == 8) {
      tft.drawString("   牛田        ", 100, 38);
    }
    if (dai2 == 9) {
      tft.drawString("   北千住        ", 100, 38);
    }
    if (dai2 == 10) {
      tft.drawString("   小菅        ", 100, 38);
    }
    if (dai2 == 11) {
      tft.drawString("   五反野        ", 100, 38);
    }
    if (dai2 == 12) {
      tft.drawString("   梅島        ", 100, 38);
    }
    if (dai2 == 13) {
      tft.drawString("   西新井        ", 100, 38);
    }
    if (dai2 == 14) {
      tft.drawString("   竹ノ塚        ", 100, 38);
    }
    if (dai2 == 15) {
      tft.drawString("   谷塚        ", 100, 38);
    }
    if (dai2 == 16) {
      tft.drawString("   草加        ", 100, 38);
    }
    if (dai2 == 17) {
      tft.drawString("   松原団地     ", 100, 38);
    }
    if (dai2 == 18) {
      tft.drawString("   新田        ", 100, 38);
    }
    if (dai2 == 19) {
      tft.drawString("   蒲生        ", 100, 38);
    }
    if (dai2 == 20) {
      tft.drawString("   新越谷        ", 100, 38);
    }
    if (dai2 == 21) {
      tft.drawString("   越谷        ", 100, 38);
    }
    if (dai2 == 22) {
      tft.drawString("   北越谷        ", 100, 38);
    }
    if (dai2 == 23) {
      tft.drawString("   大袋        ", 100, 38);
    }
    if (dai2 == 24) {
      tft.drawString("   せんげん台    ", 100, 38);
    }
    if (dai2 == 25) {
      tft.drawString("   武里        ", 100, 38);
    }
    if (dai2 == 26) {
      tft.drawString("   一ノ割       ", 100, 38);
    }
    if (dai2 == 27) {
      tft.drawString("   春日部        ", 100, 38);
    }
    if (dai2 == 28) {
      tft.drawString("   北春日部      ", 100, 38);
    }
    if (dai2 == 29) {
      tft.drawString("   姫宮        ", 100, 38);
    }
    if (dai2 == 30) {
      tft.drawString("   東武動物公園    ", 100, 38);
    }
    if (dai2 == 31) {
      tft.drawString("   和戸        ", 100, 38);
    }
    if (dai2 == 32) {
      tft.drawString("   久喜        ", 100, 38);
    }
    if (dai2 == 34) {
      tft.drawString("   東武動物公園    ", 100, 38);
    }
    if (dai2 == 35) {
      tft.drawString("   杉戸高野台     ", 100, 38);
    }
    if (dai2 == 36) {
      tft.drawString("   幸手        ", 100, 38);
    }
    if (dai2 == 37) {
      tft.drawString("   南栗橋        ", 100, 38);
    }
    if (dai2 == 42) {
      tft.drawString("   池袋        ", 100, 38);
    }
    if (dai2 == 43) {
      tft.drawString("   北池袋        ", 100, 38);
    }
    if (dai2 == 44) {
      tft.drawString("   下板橋        ", 100, 38);
    }
    if (dai2 == 45) {
      tft.drawString("   大山        ", 100, 38);
    }
    if (dai2 == 46) {
      tft.drawString("   中板橋       ", 100, 38);
    }
    if (dai2 == 47) {
      tft.drawString("   ときわ台     ", 100, 38);
    }
    if (dai2 == 48) {
      tft.drawString("   上板橋        ", 100, 38);
    }
    if (dai2 == 49) {
      tft.drawString("   東武練馬     ", 100, 38);
    }
    if (dai2 == 50) {
      tft.drawString("   下赤塚      ", 100, 38);
    }
    if (dai2 == 51) {
      tft.drawString("   成増        ", 100, 38);
    }
    if (dai2 == 52) {
      tft.drawString("   和光市      ", 100, 38);
    }
    if (dai2 == 53) {
      tft.drawString("   朝霞        ", 100, 38);
    }
    if (dai2 == 54) {
      tft.drawString("   朝霞台       ", 100, 38);
    }
    if (dai2 == 55) {
      tft.drawString("   志木        ", 100, 38);
    }
    if (dai2 == 56) {
      tft.drawString("   柳瀬川       ", 100, 38);
    }
    if (dai2 == 57) {
      tft.drawString("   みずほ台     ", 100, 38);
    }
    if (dai2 == 58) {
      tft.drawString("   鶴瀬       ", 100, 38);
    }
    if (dai2 == 59) {
      tft.drawString("   ふじみ野       ", 100, 38);
    }
    if (dai2 == 60) {
      tft.drawString("   上福岡        ", 100, 38);
    }
    if (dai2 == 61) {
      tft.drawString("   新河岸       ", 100, 38);
    }
    if (dai2 == 62) {
      tft.drawString("   川越        ", 100, 38);
    }
    if (dai2 == 63) {
      tft.drawString("   川越市      ", 100, 38);
    }
    if (dai2 == 64) {
      tft.drawString("   霞ヶ関        ", 100, 38);
    }
    if (dai2 == 65) {
      tft.drawString("   鶴ヶ島        ", 100, 38);
    }
    if (dai2 == 66) {
      tft.drawString("   若葉        ", 100, 38);
    }
    if (dai2 == 67) {
      tft.drawString("   坂戸        ", 100, 38);
    }
    if (dai2 == 68) {
      tft.drawString("   北坂戸       ", 100, 38);
    }
    if (dai2 == 69) {
      tft.drawString("   高坂        ", 100, 38);
    }
    if (dai2 == 70) {
      tft.drawString("   東松山        ", 100, 38);
    }
    if (dai2 == 71) {
      tft.drawString("   森林公園      ", 100, 38);
    }
    if (dai2 == 72) {
      tft.drawString("   つきのわ      ", 100, 38);
    }
    if (dai2 == 73) {
      tft.drawString("   武蔵嵐山      ", 100, 38);
    }
    if (dai2 == 74) {
      tft.drawString("   小川町      ", 100, 38);
    }
  }
  if (rou == 2) {
    if (dai2 == 2) {
      tft.drawString("   小竹向原      ", 100, 38);
    }
    if (dai2 == 3) {
      tft.drawString("   新桜台       ", 100, 38);
    }
    if (dai2 == 4) {
      tft.drawString("   練馬        ", 100, 38);
    }
    if (dai2 == 6) {
      tft.drawString("   池袋        ", 100, 38);
    }
    if (dai2 == 7) {
      tft.drawString("   椎名町        ", 100, 38);
    }
    if (dai2 == 8) {
      tft.drawString("   東長崎        ", 100, 38);
    }
    if (dai2 == 9) {
      tft.drawString("   江古田        ", 100, 38);
    }
    if (dai2 == 10) {
      tft.drawString("   桜台        ", 100, 38);
    }
    if (dai2 == 11) {
      tft.drawString("   練馬        ", 100, 38);
    }
    if (dai2 == 12) {
      tft.drawString("   中村橋        ", 100, 38);
    }
    if (dai2 == 13) {
      tft.drawString("   富士見台        ", 100, 38);
    }
    if (dai2 == 14) {
      tft.drawString("   練馬高野台      ", 100, 38);
    }
    if (dai2 == 15) {
      tft.drawString("   石神井公園        ", 100, 38);
    }
    if (dai2 == 16) {
      tft.drawString("   大泉学園        ", 100, 38);
    }
    if (dai2 == 17) {
      tft.drawString("   保谷     ", 100, 38);
    }
    if (dai2 == 18) {
      tft.drawString("   ひばりヶ丘       ", 100, 38);
    }
    if (dai2 == 19) {
      tft.drawString("   東久留米       ", 100, 38);
    }
    if (dai2 == 20) {
      tft.drawString("   清瀬        ", 100, 38);
    }
    if (dai2 == 21) {
      tft.drawString("   秋津        ", 100, 38);
    }
    if (dai2 == 22) {
      tft.drawString("   所沢        ", 100, 38);
    }
    if (dai2 == 23) {
      tft.drawString("   西所沢    ", 100, 38);
    }
    if (dai2 == 24) {
      tft.drawString("   小手指    ", 100, 38);
    }
    if (dai2 == 25) {
      tft.drawString("   狭山ヶ丘    ", 100, 38);
    }
    if (dai2 == 26) {
      tft.drawString("  武蔵藤沢     ", 100, 38);
    }
    if (dai2 == 27) {
      tft.drawString("  稲荷山公園      ", 100, 38);
    }
    if (dai2 == 28) {
      tft.drawString("   入間市      ", 100, 38);
    }
    if (dai2 == 29) {
      tft.drawString("   仏子        ", 100, 38);
    }
    if (dai2 == 30) {
      tft.drawString("   元加治     ", 100, 38);
    }
    if (dai2 == 31) {
      tft.drawString("   飯能        ", 100, 38);
    }
    if (dai2 == 35) {
      tft.drawString("   西武新宿     ", 100, 38);
    }
    if (dai2 == 36) {
      tft.drawString("   高田馬場        ", 100, 38);
    }
    if (dai2 == 37) {
      tft.drawString("   下落合        ", 100, 38);
    }
    if (dai2 == 38) {
      tft.drawString("   中井        ", 100, 38);
    }
    if (dai2 == 39) {
      tft.drawString(" 新井薬師前   ", 100, 38);
    }
    if (dai2 == 40) {
      tft.drawString("   沼袋        ", 100, 38);
    }
    if (dai2 == 41) {
      tft.drawString("   野方        ", 100, 38);
    }
    if (dai2 == 42) {
      tft.drawString("  都立家政      ", 100, 38);
    }
    if (dai2 == 43) {
      tft.drawString("   鷺ノ宮        ", 100, 38);
    }
    if (dai2 == 44) {
      tft.drawString("   下井草        ", 100, 38);
    }
    if (dai2 == 45) {
      tft.drawString("   井荻        ", 100, 38);
    }
    if (dai2 == 46) {
      tft.drawString("   上井草       ", 100, 38);
    }
    if (dai2 == 47) {
      tft.drawString("   上石神井     ", 100, 38);
    }
    if (dai2 == 48) {
      tft.drawString("   武蔵関        ", 100, 38);
    }
    if (dai2 == 49) {
      tft.drawString("   東伏見     ", 100, 38);
    }
    if (dai2 == 50) {
      tft.drawString("   西武柳沢      ", 100, 38);
    }
    if (dai2 == 51) {
      tft.drawString("   田無        ", 100, 38);
    }
    if (dai2 == 52) {
      tft.drawString("   花小金井      ", 100, 38);
    }
    if (dai2 == 53) {
      tft.drawString("   小平        ", 100, 38);
    }
    if (dai2 == 54) {
      tft.drawString("   久米川       ", 100, 38);
    }
    if (dai2 == 55) {
      tft.drawString("   東村山        ", 100, 38);
    }
    if (dai2 == 56) {
      tft.drawString("   所沢       ", 100, 38);
    }
    if (dai2 == 57) {
      tft.drawString("   航空公園     ", 100, 38);
    }
    if (dai2 == 58) {
      tft.drawString("   新所沢       ", 100, 38);
    }
    if (dai2 == 59) {
      tft.drawString("   入曽       ", 100, 38);
    }
    if (dai2 == 60) {
      tft.drawString("   狭山市        ", 100, 38);
    }
    if (dai2 == 61) {
      tft.drawString("   新狭山       ", 100, 38);
    }
    if (dai2 == 62) {
      tft.drawString("   南大塚        ", 100, 38);
    }
    if (dai2 == 63) {
      tft.drawString("   本川越      ", 100, 38);
    }

    if (dai2 == 67) {
      tft.drawString("   小平        ", 100, 38);
    }
    if (dai2 == 68) {
      tft.drawString("   萩山       ", 100, 38);
    }
    if (dai2 == 69) {
      tft.drawString("   小川        ", 100, 38);
    }
    if (dai2 == 70) {
      tft.drawString("   東大和市    ", 100, 38);
    }
    if (dai2 == 71) {
      tft.drawString("   玉川上水    ", 100, 38);
    }
    if (dai2 == 72) {
      tft.drawString("   武蔵砂川   ", 100, 38);
    }
    if (dai2 == 73) {
      tft.drawString("  西武立川     ", 100, 38);
    }
    if (dai2 == 74) {
      tft.drawString("   拝島      ", 100, 38);
    }
  }
  if (rou == 3) {
    if (dai2 == 2) {
      tft.drawString("   和光市     ", 100, 38);
    }
    if (dai2 == 3) {
      tft.drawString(" 地下鉄成増   ", 100, 38);
    }
    if (dai2 == 4) {
      tft.drawString(" 地下鉄赤塚    ", 100, 38);
    }
    if (dai2 == 5) {
      tft.drawString("   平和台      ", 100, 38);
    }
    if (dai2 == 6) {
      tft.drawString("   氷川台      ", 100, 38);
    }
    if (dai2 == 7) {
      tft.drawString(" 小竹向原    ", 100, 38);
    }
    if (dai2 == 8) {
      tft.drawString("  千川       ", 100, 38);
    }
    if (dai2 == 9) {
      tft.drawString("   要町      ", 100, 38);
    }
    if (dai2 == 10) {
      tft.drawString("   池袋       ", 100, 38);
    }
    if (dai2 == 11) {
      tft.drawString("  雑司が谷      ", 100, 38);
    }
    if (dai2 == 12) {
      tft.drawString("  西早稲田     ", 100, 38);
    }
    if (dai2 == 13) {
      tft.drawString("   東新宿       ", 100, 38);
    }
    if (dai2 == 14) {
      tft.drawString("  新宿三丁目   ", 100, 38);
    }
    if (dai2 == 15) {
      tft.drawString("  北参道       ", 100, 38);
    }
    if (dai2 == 16) {
      tft.drawString("  明治神宮前     ", 100, 38);
    }
    if (dai2 == 17) {
      tft.drawString("   渋谷        ", 100, 38);
    }
    if (dai2 == 19) {
      tft.drawString("  小竹向原     ", 100, 38);
    }
    if (dai2 == 20) {
      tft.drawString("   千川       ", 100, 38);
    }
    if (dai2 == 21) {
      tft.drawString("   要町        ", 100, 38);
    }
    if (dai2 == 22) {
      tft.drawString("   池袋        ", 100, 38);
    }
    if (dai2 == 23) {
      tft.drawString("   東池袋      ", 100, 38);
    }
    if (dai2 == 24) {
      tft.drawString("   護国寺       ", 100, 38);
    }
    if (dai2 == 25) {
      tft.drawString("  江戸川橋       ", 100, 38);
    }
    if (dai2 == 26) {
      tft.drawString("  飯田橋       ", 100, 38);
    }
    if (dai2 == 27) {
      tft.drawString("   市ヶ谷       ", 100, 38);
    }
    if (dai2 == 28) {
      tft.drawString("   麹町       ", 100, 38);
    }
    if (dai2 == 29) {
      tft.drawString("   永田町       ", 100, 38);
    }
    if (dai2 == 30) {
      tft.drawString("   桜田門      ", 100, 38);
    }
    if (dai2 == 31) {
      tft.drawString("   有楽町       ", 100, 38);
    }
    if (dai2 == 32) {
      tft.drawString(" 銀座一丁目   ", 100, 38);
    }
    if (dai2 == 33) {
      tft.drawString("   新富町      ", 100, 38);
    }
    if (dai2 == 34) {
      tft.drawString("  月島        ", 100, 38);
    }
    if (dai2 == 35) {
      tft.drawString("   豊洲        ", 100, 38);
    }
    if (dai2 == 36) {
      tft.drawString("   辰巳        ", 100, 38);
    }
    if (dai2 == 37) {
      tft.drawString("   新木場        ", 100, 38);
    }
    if (dai2 == 39) {
      tft.drawString("   渋谷        ", 100, 38);
    }
    if (dai2 == 40) {
      tft.drawString("   表参道      ", 100, 38);
    }
    if (dai2 == 41) {
      tft.drawString(" 青山一丁目    ", 100, 38);
    }
    if (dai2 == 42) {
      tft.drawString("   永田町       ", 100, 38);
    }
    if (dai2 == 43) {
      tft.drawString("  半蔵門       ", 100, 38);
    }
    if (dai2 == 44) {
      tft.drawString("   九段下       ", 100, 38);
    }
    if (dai2 == 45) {
      tft.drawString("   神保町      ", 100, 38);
    }
    if (dai2 == 46) {
      tft.drawString("   大手町       ", 100, 38);
    }
    if (dai2 == 47) {
      tft.drawString("   三越前       ", 100, 38);
    }
    if (dai2 == 48) {
      tft.drawString(" 水天宮前     ", 100, 38);
    }
    if (dai2 == 49) {
      tft.drawString(" 清澄白河     ", 100, 38);
    }
    if (dai2 == 50) {
      tft.drawString("   住吉        ", 100, 38);
    }
    if (dai2 == 51) {
      tft.drawString("  錦糸町      ", 100, 38);
    }
    if (dai2 == 52) {
      tft.drawString("   押上        ", 100, 38);
    }
    if (dai2 == 53) {
      tft.drawString("   曳舟        ", 100, 38);
    }
    if (dai2 == 55) {
      tft.drawString(" 北千住      ", 100, 38);
    }
    if (dai2 == 56) {
      tft.drawString(" 南千住       ", 100, 38);
    }
    if (dai2 == 57) {
      tft.drawString(" 三ノ輪       ", 100, 38);
    }
    if (dai2 == 58) {
      tft.drawString("   入谷        ", 100, 38);
    }
    if (dai2 == 59) {
      tft.drawString("   上野        ", 100, 38);
    }
    if (dai2 == 60) {
      tft.drawString(" 仲御徒町       ", 100, 38);
    }
    if (dai2 == 61) {
      tft.drawString("  秋葉原       ", 100, 38);
    }
    if (dai2 == 62) {
      tft.drawString(" 小伝馬町      ", 100, 38);
    }
    if (dai2 == 63) {
      tft.drawString("  人形町       ", 100, 38);
    }
    if (dai2 == 64) {
      tft.drawString("  茅場町       ", 100, 38);
    }
    if (dai2 == 65) {
      tft.drawString("  八丁堀       ", 100, 38);
    }
    if (dai2 == 66) {
      tft.drawString("   築地        ", 100, 38);
    }
    if (dai2 == 67) {
      tft.drawString("  東銀座        ", 100, 38);
    }
    if (dai2 == 68) {
      tft.drawString("   銀座        ", 100, 38);
    }
    if (dai2 == 69) {
      tft.drawString("  日比谷       ", 100, 38);
    }
    if (dai2 == 70) {
      tft.drawString("  霞ヶ関        ", 100, 38);
    }
    if (dai2 == 71) {
      tft.drawString("  神谷町       ", 100, 38);
    }
    if (dai2 == 72) {
      tft.drawString("  六本木       ", 100, 38);
    }
    if (dai2 == 73) {
      tft.drawString("   広尾        ", 100, 38);
    }
    if (dai2 == 74) {
      tft.drawString(" 恵比寿       ", 100, 38);
    }
    if (dai2 == 75) {
      tft.drawString(" 中目黒        ", 100, 38);
    }
  }
  if (rou == 4) {
    if (dai2 == 2) {
      tft.drawString("  西船橋        ", 100, 38);
    }
    if (dai2 == 3) {
      tft.drawString("   船橋        ", 100, 38);
    }
    if (dai2 == 4) {
      tft.drawString(" 東船橋        ", 100, 38);
    }
    if (dai2 == 5) {
      tft.drawString(" 津田沼        ", 100, 38);
    }
    if (dai2 == 11) {
      tft.drawString("   三鷹        ", 100, 38);
    }
    if (dai2 == 12) {
      tft.drawString(" 吉祥寺        ", 100, 38);
    }
    if (dai2 == 13) {
      tft.drawString(" 西荻窪        ", 100, 38);
    }
    if (dai2 == 14) {
      tft.drawString("   荻窪        ", 100, 38);
    }
    if (dai2 == 15) {
      tft.drawString(" 阿佐ヶ谷    ", 100, 38);
    }
    if (dai2 == 16) {
      tft.drawString("  高円寺      ", 100, 38);
    }
    if (dai2 == 17) {
      tft.drawString("   中野        ", 100, 38);
    }
    if (dai2 == 19) {
      tft.drawString("   中野        ", 100, 38);
    }
    if (dai2 == 20) {
      tft.drawString("   落合        ", 100, 38);
    }
    if (dai2 == 21) {
      tft.drawString(" 高田馬場  ", 100, 38);
    }
    if (dai2 == 22) {
      tft.drawString(" 早稲田      ", 100, 38);
    }
    if (dai2 == 23) {
      tft.drawString(" 神楽坂       ", 100, 38);
    }
    if (dai2 == 24) {
      tft.drawString(" 飯田橋        ", 100, 38);
    }
    if (dai2 == 25) {
      tft.drawString(" 九段下       ", 100, 38);
    }
    if (dai2 == 26) {
      tft.drawString(" 竹橋        ", 100, 38);
    }
    if (dai2 == 27) {
      tft.drawString(" 大手町        ", 100, 38);
    }
    if (dai2 == 28) {
      tft.drawString("  日本橋        ", 100, 38);
    }
    if (dai2 == 29) {
      tft.drawString("  茅場町        ", 100, 38);
    }
    if (dai2 == 30) {
      tft.drawString(" 門前仲町   ", 100, 38);
    }
    if (dai2 == 31) {
      tft.drawString("   木場        ", 100, 38);
    }
    if (dai2 == 32) {
      tft.drawString(" 東陽町        ", 100, 38);
    }
    if (dai2 == 33) {
      tft.drawString(" 南砂町        ", 100, 38);
    }
    if (dai2 == 34) {
      tft.drawString(" 西葛西        ", 100, 38);
    }
    if (dai2 == 35) {
      tft.drawString("   葛西        ", 100, 38);
    }
    if (dai2 == 36) {
      tft.drawString("   浦安        ", 100, 38);
    }
    if (dai2 == 37) {
      tft.drawString(" 南行徳        ", 100, 38);
    }
    if (dai2 == 38) {
      tft.drawString("   行徳        ", 100, 38);
    }
    if (dai2 == 39) {
      tft.drawString("   妙典        ", 100, 38);
    }
    if (dai2 == 40) {
      tft.drawString("原木中山   ", 100, 38);
    }
    if (dai2 == 41) {
      tft.drawString(" 西船橋        ", 100, 38);
    }
    if (dai2 == 42) {
      tft.drawString(" 東海神        ", 100, 38);
    }
    if (dai2 == 43) {
      tft.drawString(" 飯山満        ", 100, 38);
    }
    if (dai2 == 44) {
      tft.drawString("北習志野  ", 100, 38);
    }
    if (dai2 == 45) {
      tft.drawString("船橋日大前 ", 100, 38);
    }
    if (dai2 == 46) {
      tft.drawString("八千代緑が丘", 100, 38);
    }
    if (dai2 == 47) {
      tft.drawString("八千代中央", 100, 38);
    }
    if (dai2 == 48) {
      tft.drawString("   村上        ", 100, 38);
    }
    if (dai2 == 49) {
      tft.drawString("東葉勝田台 ", 100, 38);
    }
    if (dai2 == 56) {
      tft.drawString("  北綾瀬        ", 100, 38);
    }
    if (dai2 == 57) {
      tft.drawString("   綾瀬        ", 100, 38);
    }
    if (dai2 == 58) {
      tft.drawString(" 北千住        ", 100, 38);
    }
    if (dai2 == 59) {
      tft.drawString("   町屋        ", 100, 38);
    }
    if (dai2 == 60) {
      tft.drawString("西日暮里   ", 100, 38);
    }
    if (dai2 == 61) {
      tft.drawString(" 千駄木        ", 100, 38);
    }
    if (dai2 == 62) {
      tft.drawString("   根津        ", 100, 38);
    }
    if (dai2 == 63) {
      tft.drawString("   湯島        ", 100, 38);
    }
    if (dai2 == 64) {
      tft.drawString("新御茶ノ水", 100, 38);
    }
    if (dai2 == 65) {
      tft.drawString(" 大手町        ", 100, 38);
    }
    if (dai2 == 66) {
      tft.drawString("二重橋前       ", 100, 38);
    }
    if (dai2 == 67) {
      tft.drawString(" 日比谷        ", 100, 38);
    }
    if (dai2 == 68) {
      tft.drawString("  霞ヶ関        ", 100, 38);
    }
    if (dai2 == 69) {
      tft.drawString(" 国会議事堂前", 100, 38);
    }
    if (dai2 == 70) {
      tft.drawString("   赤坂        ", 100, 38);
    }
    if (dai2 == 71) {
      tft.drawString(" 乃木坂        ", 100, 38);
    }
    if (dai2 == 72) {
      tft.drawString("  表参道        ", 100, 38);
    }
    if (dai2 == 73) {
      tft.drawString(" 明治神宮前   ", 100, 38);
    }
    if (dai2 == 74) {
      tft.drawString(" 代々木公園    ", 100, 38);
    }
    if (dai2 == 75) {
      tft.drawString(" 代々木上原   ", 100, 38);
    }
  }
  if (rou == 5) {
    if (dai2 == 2) {
      tft.drawString("   渋谷        ", 100, 38);
    }
    if (dai2 == 3) {
      tft.drawString("  代官山        ", 100, 38);
    }
    if (dai2 == 4) {
      tft.drawString("  中目黒       ", 100, 38);
    }
    if (dai2 == 5) {
      tft.drawString("  祐天寺       ", 100, 38);
    }
    if (dai2 == 6) {
      tft.drawString(" 学芸大学  ", 100, 38);
    }
    if (dai2 == 7) {
      tft.drawString(" 都立大学  ", 100, 38);
    }
    if (dai2 == 8) {
      tft.drawString(" 自由が丘  ", 100, 38);
    }
    if (dai2 == 9) {
      tft.drawString(" 田園調布  ", 100, 38);
    }
    if (dai2 == 10) {
      tft.drawString(" 多摩川       ", 100, 38);
    }
    if (dai2 == 11) {
      tft.drawString(" 新丸子        ", 100, 38);
    }
    if (dai2 == 12) {
      tft.drawString("武蔵小杉  ", 100, 38);
    }
    if (dai2 == 13) {
      tft.drawString("  元住吉        ", 100, 38);
    }
    if (dai2 == 14) {
      tft.drawString("   日吉        ", 100, 38);
    }
    if (dai2 == 15) {
      tft.drawString("   綱島        ", 100, 38);
    }
    if (dai2 == 16) {
      tft.drawString(" 大倉山        ", 100, 38);
    }
    if (dai2 == 17) {
      tft.drawString("   菊名        ", 100, 38);
    }
    if (dai2 == 18) {
      tft.drawString(" 妙蓮寺        ", 100, 38);
    }
    if (dai2 == 19) {
      tft.drawString("   白楽        ", 100, 38);
    }
    if (dai2 == 20) {
      tft.drawString(" 東白楽        ", 100, 38);
    }
    if (dai2 == 21) {
      tft.drawString("   反町        ", 100, 38);
    }
    if (dai2 == 22) {
      tft.drawString("   横浜        ", 100, 38);
    }
    if (dai2 == 23) {
      tft.drawString("  新高島        ", 100, 38);
    }
    if (dai2 == 24) {
      tft.drawString("みなとみらい ", 100, 38);
    }
    if (dai2 == 25) {
      tft.drawString("  馬車道        ", 100, 38);
    }
    if (dai2 == 26) {
      tft.drawString(" 日本大通     ", 100, 38);
    }
    if (dai2 == 27) {
      tft.drawString("元町・中華街  ", 100, 38);
    }
    if (dai2 == 29) {
      tft.drawString(" 長津田        ", 100, 38);
    }
    if (dai2 == 30) {
      tft.drawString("   恩田        ", 100, 38);
    }
    if (dai2 == 31) {
      tft.drawString("こどもの国  ", 100, 38);
    }
    if (dai2 == 33) {
      tft.drawString("   横浜        ", 100, 38);
    }
    if (dai2 == 34) {
      tft.drawString(" 高島町        ", 100, 38);
    }
    if (dai2 == 35) {
      tft.drawString("  桜木町        ", 100, 38);
    }
    if (dai2 == 37) {
      tft.drawString(" 白金高輪        ", 100, 38);
    }
    if (dai2 == 38) {
      tft.drawString(" 白金台        ", 100, 38);
    }
    if (dai2 == 39) {
      tft.drawString("   目黒        ", 100, 38);
    }
    if (dai2 == 40) {
      tft.drawString(" 不動前        ", 100, 38);
    }
    if (dai2 == 41) {
      tft.drawString(" 武蔵小山        ", 100, 38);
    }
    if (dai2 == 42) {
      tft.drawString(" 西小山        ", 100, 38);
    }
    if (dai2 == 43) {
      tft.drawString("   洗足        ", 100, 38);
    }
    if (dai2 == 44) {
      tft.drawString(" 大岡山        ", 100, 38);
    }
    if (dai2 == 45) {
      tft.drawString("   奥沢        ", 100, 38);
    }
    if (dai2 == 46) {
      tft.drawString(" 田園調布      ", 100, 38);
    }
    if (dai2 == 48) {
      tft.drawString("   渋谷        ", 100, 38);
    }
    if (dai2 == 49) {
      tft.drawString("池尻大橋  ", 100, 38);
    }
    if (dai2 == 50) {
      tft.drawString(" 三軒茶屋   ", 100, 38);
    }
    if (dai2 == 51) {
      tft.drawString(" 駒沢大学    ", 100, 38);
    }
    if (dai2 == 52) {
      tft.drawString(" 桜新町        ", 100, 38);
    }
    if (dai2 == 53) {
      tft.drawString("   用賀        ", 100, 38);
    }
    if (dai2 == 54) {
      tft.drawString(" 二子玉川    ", 100, 38);
    }
    if (dai2 == 55) {
      tft.drawString(" 二子新地    ", 100, 38);
    }
    if (dai2 == 56) {
      tft.drawString("   高津        ", 100, 38);
    }
    if (dai2 == 57) {
      tft.drawString(" 溝の口        ", 100, 38);
    }
    if (dai2 == 58) {
      tft.drawString(" 梶が谷        ", 100, 38);
    }
    if (dai2 == 59) {
      tft.drawString(" 宮崎台        ", 100, 38);
    }
    if (dai2 == 60) {
      tft.drawString(" 宮前平        ", 100, 38);
    }
    if (dai2 == 61) {
      tft.drawString("   鷺沼        ", 100, 38);
    }
    if (dai2 == 62) {
      tft.drawString("たまプラーザ", 100, 38);
    }
    if (dai2 == 63) {
      tft.drawString(" あざみ野    ", 100, 38);
    }
    if (dai2 == 64) {
      tft.drawString("   江田        ", 100, 38);
    }
    if (dai2 == 65) {
      tft.drawString(" 市が尾       ", 100, 38);
    }
    if (dai2 == 66) {
      tft.drawString("  藤が丘       ", 100, 38);
    }
    if (dai2 == 67) {
      tft.drawString("  青葉台       ", 100, 38);
    }
    if (dai2 == 68) {
      tft.drawString("   田奈        ", 100, 38);
    }
    if (dai2 == 69) {
      tft.drawString(" 長津田        ", 100, 38);
    }
    if (dai2 == 70) {
      tft.drawString(" つくし野    ", 100, 38);
    }
    if (dai2 == 71) {
      tft.drawString(" すずかけ台   ", 100, 38);
    }
    if (dai2 == 72) {
      tft.drawString("  南町田       ", 100, 38);
    }
    if (dai2 == 73) {
      tft.drawString("  つきみ野      ", 100, 38);
    }
    if (dai2 == 74) {
      tft.drawString("  中央林間     ", 100, 38);
    }
  }
  if (rou == 6) {
    if (dai2 == 29) {
      tft.drawString(" 渋谷  ", 100, 38);
    }
    if (dai2 == 28) {
      tft.drawString("表参道  ", 100, 38);
    }
    if (dai2 == 27) {
      tft.drawString("外苑前       ", 100, 38);
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
  if (des == 1) {
    tft.drawString("渋谷", 230, 8);
  }
  if (des == 2) {
    tft.drawString("浅草", 230, 8);
  }
  if (des == 3) {
    tft.drawString("上野", 230, 8);
  }
  if (des == 4) {
    tft.drawString("方南町 ", 230, 8);
  }
  if (des == 5) {
    tft.drawString("赤坂見附  ", 230, 8);
  }
  if (des == 6) {
    tft.drawString("荻窪", 230, 8);
  }
  if (des == 7) {
    tft.drawString("池袋", 230, 8);
  }
  if (des == 8) {
    tft.drawString("回送", 230, 8);
  }

  tft.drawString("終着戸開 :                   中間", 10, 67);
  if (arr == 1) {
    tft.drawString("1側", 150, 67);
  }
  if (arr == 2) {
    tft.drawString("2側", 150, 67);
  }
  if (arr == 3) {
    tft.drawString("不 定", 150, 67);
  }

  tft.drawString("車内 :   ", 10, 100);
  tft.drawString(" 1      2      3     4      5     6", 135, 165);
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
void page_6() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-21");
  tft.drawString("車両を選択して、設定を押し", 15  , 5);
  tft.drawString("て下さい。", 15  , 30);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor( TFT_BLACK);
  tft.loadFont("yu-gothic-28");
  tft.drawString("前灯点滅", 365  , 15);
  tft.setTextColor(TFT_WHITE);
  tft.pushImage(50, 128, 140, 45, button9);
  tft.pushImage(290, 128, 140, 45, button9);

  tft.drawRect(50, 128, 140, 45, TFT_WHITE);
  tft.drawRect(290, 128, 140, 45, TFT_WHITE);

  tft.drawString("自    車", 80  , 140);
  tft.drawString("他    車", 320  , 140);

  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_black);
  tft.pushImage(242, 275, 115, 45, button_purple);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("設      定", 265, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 385, 290);
  page = '6';//change the state page into 1
}
void page_7() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-21");
  tft.drawString("「入」「切」を選択後、設定を", 15  , 5);
  tft.drawString("押して下さい。", 15  , 30);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor( TFT_BLACK);
  tft.drawString("放送・表示", 365  , 15);

  tft.drawRoundRect(25, 55, 430, 70, 10, TFT_WHITE);
  tft.drawRoundRect(25, 128, 430, 70, 10, TFT_WHITE);
  tft.drawRoundRect(25, 201, 430, 70, 10, TFT_WHITE);
  tft.pushImage(295, 60, 60, 60, button10);
  tft.pushImage(295, 133, 60, 60, button10);
  tft.pushImage(295, 206, 60, 60, button10);
  tft.pushImage(365, 60, 60, 60, button10);
  tft.pushImage(365, 133, 60, 60, button10);
  tft.pushImage(365, 206, 60, 60, button10);
  tft.drawRect(295, 60, 60, 60, TFT_WHITE);
  tft.drawRect(295, 133, 60, 60, TFT_WHITE);
  tft.drawRect(295, 206, 60, 60, TFT_WHITE);
  tft.drawRect(365, 60, 60, 60, TFT_WHITE);
  tft.drawRect(365, 133, 60, 60, TFT_WHITE);
  tft.drawRect(365, 206, 60, 60, TFT_WHITE);
  tft.setTextColor(TFT_WHITE);
  tft.loadFont("yu-gothic-28");
  tft.drawString("自 動 放 送", 45  , 80);
  tft.drawString("車 内 表 示", 45  , 153);
  tft.drawString("乗 換 案 内", 45  , 223);
  tft.drawString("入", 310  , 80);
  tft.drawString("切", 380  , 80);
  tft.drawString("入", 310  , 153);
  tft.drawString("切", 380  , 153);
  tft.drawString("入", 310  , 223);
  tft.drawString("切", 380  , 223);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_blue);
  tft.pushImage(242, 275, 115, 45, button_purple);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("各種放送", 134, 290);
  tft.drawString("設      定", 265, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 385, 290);
  page = '7';//change the state page into 1
}
void page_8() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-21");
  tft.drawString("行先を選択して、設定を押して", 15  , 5);
  tft.drawString("下さい。", 15  , 30);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor( TFT_BLACK);
  tft.loadFont("yu-gothic-28");
  tft.drawString(" 行    先 ", 365  , 15);
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(50, 60, 170, 45, button);
  tft.pushImage(50, 115, 170, 45, button);
  tft.pushImage(50, 170, 170, 45, button);
  tft.pushImage(50, 225, 170, 45, button);
  tft.pushImage(290, 60, 170, 45, button);
  tft.pushImage(290, 115, 170, 45, button);
  tft.pushImage(290, 170, 170, 45, button);
  tft.pushImage(290, 225, 170, 45, button);
  tft.drawString(" 東  武 ", 80  , 70);
  tft.drawString(" 西  武 ", 320  , 70);
  tft.drawString(" 営  団 ", 80  , 125);
  tft.drawString(" 営  団 2 ", 320  , 125);
  tft.drawString("東急・横高", 70  , 180);
  tft.drawString("銀 座 線 ", 320  , 180);
  tft.drawString("非 表 示 ", 320  , 235);

  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_black);
  tft.pushImage(242, 275, 115, 45, button_blue);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("起      動", 265, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 385, 290);
  page = '8';//change the state page into 1
}
void page_10() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-28");
  tft.drawString("放送を選択して下さい。", 15  , 10);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor( TFT_BLACK);
  tft.loadFont("yu-gothic-28");
  tft.drawString("各種放送", 365  , 15);
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(50, 60, 170, 45, button);
  tft.pushImage(50, 115, 170, 45, button);
  tft.pushImage(50, 170, 170, 45, button);
  tft.pushImage(50, 225, 170, 45, button);
  tft.pushImage(290, 60, 170, 45, button);
  tft.pushImage(290, 115, 170, 45, button);
  tft.pushImage(290, 170, 170, 45, button);
  tft.pushImage(290, 225, 170, 45, button);
  tft.drawString("座席整理", 80  , 70);
  tft.drawString("駆け込み", 320  , 70);
  tft.drawString("携帯電話", 80  , 125);
  tft.drawString("間隔調整", 320  , 125);
  tft.drawString("運転再開", 80  , 180);
  tft.drawString(" 停    止 ", 320  , 235);

  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_black);
  tft.pushImage(242, 275, 115, 45, button_blue);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("放送・表示", 248, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 385, 290);
  page = '10';//change the state page into 1
}
void page_11() {
  drawKeypad();


  page = '11';//change the state page into 1
}
void page_12() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-21");
  tft.drawString("行先を選択して、設定を押して", 15  , 5);
  tft.drawString("下さい。", 15  , 30);
  tft.loadFont("yu-gothic-21");
  tft.setTextColor( TFT_BLACK);
  tft.loadFont("yu-gothic-28");
  tft.drawString(" 行    先 ", 365  , 15);
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(50, 60, 170, 45, button);
  tft.pushImage(50, 115, 170, 45, button);
  tft.pushImage(50, 170, 170, 45, button);
  tft.pushImage(50, 225, 170, 45, button);
  tft.pushImage(290, 60, 170, 45, button);
  tft.pushImage(290, 115, 170, 45, button);
  tft.pushImage(290, 170, 170, 45, button);
  tft.pushImage(290, 225, 170, 45, button);
  tft.drawString(" 渋    谷 ", 80  , 70);
  tft.drawString(" 浅    草 ", 320  , 70);
  tft.drawString(" 上    野 ", 80  , 125);
  tft.drawString(" 方 南 町 ", 320  , 125);
  tft.drawString("赤 坂 見 附", 70  , 180);
  tft.drawString(" 荻    窪 ", 320  , 180);
  tft.drawString(" 池    袋 ", 80  , 235);
  tft.drawString(" 回    送 ", 320  , 235);

  tft.loadFont("yu-gothic-21");
  tft.setTextColor(TFT_BLACK);
  tft.pushImage(0, 275, 115, 45, button_blue);
  tft.pushImage(121, 275, 115, 45, button_black);
  tft.pushImage(242, 275, 115, 45, button_blue);
  tft.pushImage(363, 275, 115, 45, button_red3);
  tft.drawString("メニュー", 13, 290);
  tft.drawString("起      動", 265, 290);
  //  tft.setTextColor(TFT_WHITE);
  tft.drawString("異      常", 385, 290);
  page = '12';//change the state page into 1
}
void page_13() {
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 50, 480, 50, TFT_WHITE);
  tft.fillRect(360, 0, 480, 50, TFT_YELLOW);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.loadFont("yu-gothic-21");
  tft.drawString("終着戸開けを設定し、起動を押して", 15  , 5);
  tft.drawString("下さい。", 15  , 30);
  tft.loadFont("yu-gothic-28");
  tft.setTextColor( TFT_BLACK);
  tft.drawString("着番設定", 365  , 10);

  tft.pushImage(50, 100, 140, 45, button9);
  tft.pushImage(290, 100, 140, 45, button9);
  tft.pushImage(170, 200, 140, 45, button9);
  tft.setTextColor( TFT_WHITE);

  tft.drawRect(50, 100, 140, 45, TFT_WHITE);
  tft.drawRect(290, 100, 140, 45, TFT_WHITE);
  tft.drawRect(170, 200, 140, 45, TFT_WHITE);

  tft.drawString("1    側", 80  , 110);
  tft.drawString("2    側", 320  , 110);
  tft.drawString("不   定", 200  , 210);

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
  page = '13';//change the state page into 1
}

void play(unsigned char Track)
{
  unsigned char play[6] = {0xAA, 0x07, 0x02, 0x00, Track, Track + 0xB3};
//  Serial2.write(play, 6);
}
void volume( unsigned char vol)
{
  unsigned char volume[5] = {0xAA, 0x13, 0x01, vol, vol + 0xBE};
//  Serial2.write(volume, 5);
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

//------------------------------------------------------------------------------------------
/*
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File fs = SPIFFS.open(CALIBRATION_FILE, "w");
    if (fs) {
      fs.write((const unsigned char *)calData, 14);
      fs.close();
    }
  }
}
*/

//------------------------------------------------------------------------------------------

// Print something in the mini status bar
void status(const char *msg) {
  tft.setTextPadding(240);
  //tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.setTextFont(0);
  tft.setTextDatum(TC_DATUM);
  tft.setTextSize(1);
  tft.drawString(msg, STATUS_X, STATUS_Y);
}

//------------------------------------------------------------------------------------------


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
