#include <LiquidCrystal.h>

/*
  Arduinoにシリアル経由で送るコマンド:
  setMotorPower(Integer left_power, Integer right_power)　-　モータ出力の設定．値は−100〜100．
  setHeadLight(Integer led_power)　-　ヘッドライトの設定．値は0〜100．
  setRearLight(Integer led_power_red, led_power_green, led_power_blue)　-　バックライトの設定．値は0〜255．
  setBlinker(Boolean is_left_turn_on,  is_right_turn_on)　-　方向指示器の設定．
  setLCD(String str)　-　LCD文字列の指定．strは空文字の場合のときクリアとする．

  コマンドの値について:
  Boolean は "true" または "false" という文字列として渡されます．

  コマンドの例:
  setMotorPower:10:20;

  最初の文字列はコマンド名，2つめ以降はコマンドにより可変数の値，値の区切りはコロン(:)．
  ;がコマンド文字列の終端記号．
*/

// 駆動用モータ制御
const int PIN_RIGHT_MOTOR = 5;
const int PIN_LEFT_MOTOR = 6;

// ウィンカーライト
const int PIN_RIGHT_BLINKER = 18;
const int PIN_LEFT_BLINKER = 19;

// ヘッドライト
const int PIN_HEAD_LIGHT = 3;

// バックライト （フルカラー）
const int PIN_BACK_LIGHT_R = 0;
const int PIN_BACK_LIGHT_G = 1;
const int PIN_BACK_LIGHT_B = 2;

// LCDレジスタ選択
const int PIN_LCD_RS = 8;

// LCD読み書き設定
const int PIN_LCD_RW = 9;

// LCD データビット
const int PIN_LCD_D4 = 10;
const int PIN_LCD_D5 = 11;
const int PIN_LCD_D6 = 12;
const int PIN_LCD_D7 = 13;

// 超音波センサ
const int PIN_SONIC = 7;

// コマンドパラメータの最大数
const int MAX_COMMAND_PARAMETER = 3;


LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_RW, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);

// 制御コマンド
String command = "";

// コマンドパラメータ
String param[MAX_COMMAND_PARAMETER];

boolean LEFT_BLINKER = LOW;
boolean RIGHT_BLINKER = LOW;

unsigned long beforeBlinkeTime;
unsigned long backLightRTime;
unsigned long backLightGTime;
unsigned long backLightBTime;
int ledPowerRed;
int ledPowerGreen;
int ledPowerBlue;


// 擬似割り込み用(約1mS毎)
void pseudoFlash() {

  // ウィンカー処理
  if ( millis() - beforeBlinkeTime > 500 ) {

    beforeBlinkeTime = millis();

    if (LEFT_BLINKER) {
      digitalWrite(PIN_LEFT_BLINKER, !digitalRead(PIN_LEFT_BLINKER));
    } else {
      digitalWrite(PIN_LEFT_BLINKER, LOW);
    }

    if (RIGHT_BLINKER) {
      digitalWrite(PIN_RIGHT_BLINKER, !digitalRead(PIN_RIGHT_BLINKER));
    } else {
      digitalWrite(PIN_RIGHT_BLINKER, LOW);
    }

  }

  // バックライト赤
  if ( millis() - backLightRTime <= ledPowerRed ) {//0~255
    digitalWrite(PIN_BACK_LIGHT_R, HIGH);
  } else {
    digitalWrite(PIN_BACK_LIGHT_R, LOW);
    backLightRTime = millis();
  }


  // バックライト緑
  if ( millis() - backLightGTime <= ledPowerGreen ) {//0~255
    digitalWrite(PIN_BACK_LIGHT_G, HIGH);
  } else {
    digitalWrite(PIN_BACK_LIGHT_G, LOW);
    backLightGTime = millis();
  }


  // バックライト青
  if ( millis() - backLightBTime <= ledPowerBlue ) {//0~255
    digitalWrite(PIN_BACK_LIGHT_B, HIGH);
  } else {
    digitalWrite(PIN_BACK_LIGHT_B, LOW);
    backLightBTime = millis();
  }

}


// 初期化
void setup() {

  // 入出力設定
  pinMode(PIN_RIGHT_MOTOR, OUTPUT);
  pinMode(PIN_LEFT_MOTOR, OUTPUT);
  pinMode(PIN_RIGHT_BLINKER, OUTPUT);
  pinMode(PIN_LEFT_BLINKER, OUTPUT);
  pinMode(PIN_HEAD_LIGHT, OUTPUT);
  pinMode(PIN_BACK_LIGHT_R, OUTPUT);
  pinMode(PIN_BACK_LIGHT_G, OUTPUT);
  pinMode(PIN_BACK_LIGHT_B, OUTPUT);
  pinMode(PIN_LCD_RS, OUTPUT);
  pinMode(PIN_LCD_RW, OUTPUT);
  pinMode(PIN_LCD_D4, OUTPUT);
  pinMode(PIN_LCD_D5, OUTPUT);
  pinMode(PIN_LCD_D6, OUTPUT);
  pinMode(PIN_LCD_D7, OUTPUT);
  pinMode(PIN_SONIC, OUTPUT);

  // 初期値を出力
  digitalWrite(PIN_RIGHT_MOTOR, LOW);
  digitalWrite(PIN_LEFT_MOTOR, LOW);
  digitalWrite(PIN_RIGHT_BLINKER, LOW);
  digitalWrite(PIN_LEFT_BLINKER, LOW);
  digitalWrite(PIN_HEAD_LIGHT, LOW);
  digitalWrite(PIN_BACK_LIGHT_R, LOW);
  digitalWrite(PIN_BACK_LIGHT_G, LOW);
  digitalWrite(PIN_BACK_LIGHT_B, LOW);
  digitalWrite(PIN_LCD_RS, LOW);
  digitalWrite(PIN_LCD_RW, LOW);
  digitalWrite(PIN_LCD_D4, LOW);
  digitalWrite(PIN_LCD_D5, LOW);
  digitalWrite(PIN_LCD_D6, LOW);
  digitalWrite(PIN_LCD_D7, LOW);
  digitalWrite(PIN_SONIC, LOW);


  // シリアルポートを9600 bps[ビット/秒]
  Serial.begin(9600);

  beforeBlinkeTime = millis();
  backLightRTime = millis();
  backLightGTime = millis();
  backLightBTime = millis();

  // LCD初期化
  lcd.begin(16, 2);
  delay(1000);

  // 起動メッセージの表示
  setLCD("Running Now");
  delay(500);

  for (int i = 11; i < 14; i++) {
    lcd.setCursor(i, 0);
    lcd.print(".");
    delay(500);
  }

  setLCD("Created by\\n      Oden Tools");

}


void setMotorPower(int left_power, int right_power) {

  Serial.println(left_power);
  Serial.println(right_power);

  analogWrite(PIN_LEFT_MOTOR, left_power);
  analogWrite(PIN_RIGHT_MOTOR, right_power);

}


void setHeadLight(int led_power) {

  analogWrite(PIN_HEAD_LIGHT, led_power);

}


void setRearLight(int led_power_red, int led_power_green, int led_power_blue) {

  ledPowerRed = led_power_red;
  ledPowerGreen = led_power_green;
  ledPowerBlue = led_power_blue;

}


void setBlinker(bool is_left_turn_on, bool is_right_turn_on) {

  LEFT_BLINKER = is_left_turn_on;
  RIGHT_BLINKER = is_right_turn_on;
  digitalWrite(PIN_LEFT_BLINKER, is_left_turn_on);
  digitalWrite(PIN_RIGHT_BLINKER, is_right_turn_on);

}


void setLCD(String str) {

  unsigned long time = millis();

  lcd.clear();
  lcd.print(str.substring(0, str.indexOf("\\n")));

  if (str.indexOf("\\n") != -1) {
    lcd.setCursor(0, 1);
    lcd.print(str.substring(str.indexOf("\\n") + 2, str.length()));
  }

}


unsigned int getSonic () {

  unsigned int duration, cm;

  pinMode(PIN_SONIC, OUTPUT);          // Set pin to OUTPUT
  digitalWrite(PIN_SONIC, LOW);        // Ensure pin is low
  delayMicroseconds(2);
  digitalWrite(PIN_SONIC, HIGH);       // Start ranging
  delayMicroseconds(5);                //   with 5 microsecond burst
  digitalWrite(PIN_SONIC, LOW);        // End ranging
  pinMode(PIN_SONIC, INPUT);           // Set pin to INPUT
  duration = pulseIn(PIN_SONIC, HIGH); // Read echo pulse
  cm = (duration / 74 / 2) / 0.39370;

  return cm;

}


bool atob(String str) {

  if (str == "true") return true;
  else return false;

}


void loop() {

  String buff;

  // シリアル受信時
  while (Serial.available() > 0) {

    // バッファに読み込む
    char c = Serial.read();

    if ( c == ';') break;
    buff += c;

  }

  delay(1);

  if (buff.length() > 0)
    Serial.println(buff);

  // コマンドの探索
  int index = buff.indexOf(":"), nextIndex;
  String command = buff.substring(0, index);
  String param[MAX_COMMAND_PARAMETER];

  // コマンドがパラメータを保有する場合
  if (index != -1) {

    // パラメータ1を探索
    for (int i = 0; i < MAX_COMMAND_PARAMETER; i++) {

      nextIndex = buff.indexOf(":", index + 1);
      if ( nextIndex != -1) {
        param[i] = buff.substring(index + 1, nextIndex); Serial.println(param[i]);
        index = nextIndex;
      } else {
        param[i] = buff.substring(index + 1, buff.length()); Serial.println(param[i]);
        break;
      }

    }

  }

  // コマンドの実行
  if (command == "setMotorPower") {
    setMotorPower(atoi(param[0].c_str()), atoi(param[1].c_str()));
  } else if (command == "setHeadLight") {
    setHeadLight(atoi(param[0].c_str()));
  } else if (command == "setBlinker") {
    setBlinker(atob(param[0]), atob(param[1]));
  } else if (command == "setRearLight") {
    setRearLight(atoi(param[0].c_str()), atoi(param[1].c_str()), atoi(param[2].c_str()));
  } else if (command == "setLCD") {
    setLCD(param[0]);
  } else if (command == "getSonic") {
    Serial.print("getSonic:");
    Serial.print(getSonic());
    Serial.println(";");
  }

  // 擬似割り込み実行
  pseudoFlash();

}
