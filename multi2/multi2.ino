// Font file is stored in SPIFFS
#include "FS.h"

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

#define CALIBRATION_FILE "/calibrationData"

#define ELEMENTS_NUM 5 /**< カンマ区切りデータの項目数 */

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
  listFiles(); // Lists the files so you can see what is in the SPIFFS

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
  tft.loadFont("msgothic-28");
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("項目キーを選択して下さい。", 10  , 10);
  tft.setTextColor(TFT_BLACK, TFT_YELLOW);
  tft.drawString(" メニュー", 360  , 10);
  tft.fillRect(35, 75, 170, 45, TFT_GREEN); //stroke line
  tft.setTextColor(TFT_BLACK, TFT_GREEN);
  tft.drawString("  状態表示  ", 50  , 85);
  tft.fillRect(35, 135, 170, 45, TFT_GREEN); //stroke line
  tft.drawString("  着番設定  ", 50  , 145);
  tft.fillRect(35, 195, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(35, 255, 170, 45, TFT_GREEN); //stroke line
  tft.drawString("  画面消去  ", 50  , 265);
  tft.fillRect(275, 75, 170, 45, TFT_GREEN); //stroke line
  tft.drawString("  地点補正  ", 285  , 85);
  tft.fillRect(275, 135, 170, 45, TFT_GREEN); //stroke line
  tft.drawString("  前灯点滅  ", 285  , 145);
  tft.fillRect(275, 195, 170, 45, TFT_GREEN); //stroke line
  tft.fillRect(275, 255, 170, 45, TFT_RED); //stroke line
  tft.setTextColor(TFT_BLACK, TFT_RED);
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
  tft.drawString("運行 : 21  行先 : ", 10, 7);
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
