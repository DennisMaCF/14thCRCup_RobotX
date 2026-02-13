#include <Arduino.h>
#include "PWMInput.h"
#include <math.h>
#include <HardwareSerial.h>
#include "ArmControl.h"
#include "baseControl.h"
#include <ESP32Servo.h>

//遙控器腳位
const int pwmPins[11] = { 48, 35, 36, 37, 38, 39, 40, 41, 42, 2, 1 };  //S3
PWMInput pwmInput(pwmPins, 11);

const int suctionCupPin = 19;

const int relay = 15;

float location_distance, location_hight, location_angle = 0;
float armranges = 4000;
float location[3] = { location_distance, location_hight, location_angle };
float loa = 0;
float divt = 1000;

#define RX_PIN 18
#define TX_PIN 8

ArmControl armControl;


unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
long interval;

char direction = 'N';
char steering = 'S';
int speed = 0;
int turnSpeed = 0;
char mecFB = 'N';
char mecLR = 'S';
int mecSpeed = 0;
int mecturnSpeed = 0;
float arm_hight, arm_distance, arm_angle, armA = 0;

int lastMode = -1;
float last_distance = 319.33;
float last_hight = -34.74;
float last_angle = -5.7;
float lsa = -45;

int suCase = 0;
int ch9 = 0;
int turnTable = 0;
int ch10 = 0;
String table;
int lsmode = 0;
int lsch7 = 0;
int lsch9;
String ESP32_Sensor;
String servo_signal;
float TA = 0;
float CA = 0;
float kp = 1.5;
float ki = 0.0;
float kd = 0.0;
float lastError = 0.0;
float integral = 0.0;
float change = 0.0;
float error = 0.0;
float derivative = 0.0;
bool TAset = false;

char basedir = 'S';
char basehor = 'S';
char baserot = 'S';
float dirval = 0;
float horval = 0;
float rotval = 0;
int level = 0;
int su = 0;
int grab = 0;
float tray = 0;
String arm;

bool blow = false;
unsigned long startMillis = 0;


int set_mode = 0;
float turnangle = 0;


void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
  pwmInput.begin();
  location_distance = 319.33;
  location_hight = -34.74;
  location_angle = -5.7;
  loa = -45;
  last_distance = location_distance;
  last_hight = location_hight;
  last_angle = location_angle;
  lsa = loa;
  lsmode = 3;
  lsch7 = 1;
  //設定馬達腳位為輸出
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      pinMode(DCmotors[i][j], OUTPUT);
    }
  }
  pinMode(suctionCupPin, OUTPUT);
  digitalWrite(suctionCupPin, LOW);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);


  Serial2.println("$X");
  Serial2.println("G92 X-5.7 Y-43.25 Z-140.03 A-135 C0");

  delay(50);
  TA = readGyroAngle();
  Serial.println(TA);
  TAset = true;
}

void loop() {

  unsigned long currentMillis = millis();
  unsigned long currentMillis2 = millis();
  unsigned long currentMillis3 = millis();

  int ch11Width = pwmInput.getWidth(10);
  bool signal = pwmInput.hasSignal(ch11Width);

  int mode = 0;
  int carmode = 0;
  int ch7 = 0;
  int put = 0;
  int putsig = 0;
  int armend = 0;

  int change;  // 使用位置變化的總和
  float delay_times = armControl.delaytimes;

  //取ch1到ch10的值以及其PWM轉換
  for (int i = 0; i < 11; i++) {
    int width = pwmInput.getWidth(i);
    int mappedValue;
    if (signal) {
      mappedValue = pwmInput.mapPWMToRange(width);
    } else {
      mappedValue = 0;
    }

    if (i == 0) {
      mecLR = pwmInput.controlSteering(width);
      mecturnSpeed = mappedValue;
    }

    if (i == 1) {
      mecFB = pwmInput.controlmecDirection(width);
      mecSpeed = mappedValue;
    }
    if (i == 2) {
      direction = pwmInput.controlDirection(width);
      speed = mappedValue;
    }

    if (i == 3) {
      steering = pwmInput.controlSteering(width);
      turnSpeed = mappedValue;
    }

    if (i == 4) {  //關卡選擇
      mode = pwmInput.sixCase(width);
    }

    if (i == 5) {  //吸盤
      put = pwmInput.twoCase(width);
    }

    if (i == 6) {  //儲球/關卡
      ch7 = pwmInput.twoCasenot(width);
    }

    if (i == 7) {  //手動/車體
      carmode = pwmInput.twoCase(width);
    }

    if (i == 8) {  //轉盤
      ch9 = pwmInput.thrCase(width);
    }

    if (i == 9) {  //夾爪
      ch10 = pwmInput.thrCase(width);
    }

    if (i == 10) {
      armend = map(width, 940, 2080, 500, 2500);
    }
  }


  switch (suCase) {  //吸盤動作
    case 0:
      if (blow == false) {
        digitalWrite(suctionCupPin, LOW);
        digitalWrite(relay, HIGH);
        delay(200);
        blow = true;
      }
      digitalWrite(relay, HIGH);
      digitalWrite(suctionCupPin, HIGH);
      su = 0;
      if (put == 1) suCase++;
      break;
    case 1:
      if (put == 0) suCase++;
      break;
    case 2:
      digitalWrite(suctionCupPin, HIGH);
      digitalWrite(relay, LOW);
      blow = false;
      su = 1;
      if (put == 1) suCase++;
      break;
    case 3:
      if (put == 0)
        suCase = 0;
      break;
  }

  switch (turnTable) {  //轉盤機構
    case 0:
      if (table != "G90 G21 C" + String(turnangle) + " F20000") {
        table = "G90 G21 C" + String(turnangle) + " F20000";
        Serial2.println(table);
        tray = 90;
      }
      if (ch9 == 0) {
        turnangle += 90;
        turnTable++;
      }

      else if (ch9 == 2) {
        turnangle -= 90;
        turnTable = 5;
      }
      break;
    case 1:
      if (ch9 == 1) turnTable = 2;
      break;
    case 2:
      if (table != "G90 G21 C" + String(turnangle) + " F20000") {
        table = "G90 G21 C" + String(turnangle) + " F20000";
        Serial2.println(table);
        tray = 180;
      }
      if (ch9 == 0) {
        turnangle += 90;
        turnTable++;
      }

      else if (ch9 == 2) {
        turnangle -= 90;
        turnTable = 7;
      }
      break;
    case 3:
      if (ch9 == 1) turnTable = 4;
      break;
    case 4:
      if (table != "G90 G21 C" + String(turnangle) + " F20000") {
        table = "G90 G21 C" + String(turnangle) + " F20000";
        Serial2.println(table);
        tray = 270;
      }
      if (ch9 == 0) {
        turnangle += 90;
        turnTable++;
      }

      else if (ch9 == 2) {
        turnangle -= 90;
        turnTable = 1;
      }
      break;
    case 5:
      if (ch9 == 1) turnTable = 6;
      break;
    case 6:
      if (table != "G90 G21 C" + String(turnangle) + " F20000") {
        table = "G90 G21 C" + String(turnangle) + " F20000";
        Serial2.println(table);
        tray = 0;
      }
      if (ch9 == 0) {
        turnangle += 90;
        turnTable++;
      }

      else if (ch9 == 2) {
        turnangle -= 90;
        turnTable = 3;
      }
      break;
    case 7:
      if (ch9 == 1) turnTable = 0;
      break;
  }

  switch (ch10) {  //夾爪
    case 0:        //裡抓
      grab = 0;
      break;
    case 1:  //外抓
      grab = 1;
      break;
    case 2:  //外放
      grab = 2;
      break;
  }

  switch (carmode) {
    case 1:
      controlmec(0, 0, 0, 0, 0, 0, 0);
      arm_angle = map(turnSpeed, 0, 255, 0, armranges / 2.5);
      arm_angle /= divt;
      arm_distance = map(speed, 0, 255, 0, armranges);
      arm_distance /= divt;
      arm_hight = map(mecSpeed, 0, 255, 0, armranges);
      arm_hight /= divt;
      armA = map(mecturnSpeed, 0, 255, 0, armranges);
      armA /= divt;


      // 根據變化量調整 interval，確保 interval 不會太小或太大
      delay_times *= 10;
      delay_times += 1;
      interval = delay_times;                  // 簡單的減法調整
      interval = constrain(interval, 1, 100);  // 設定最小和最大值範圍

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  // 更新上次更新的時間

        arm_M_mode();
        Auto_arm(location_hight, location_distance, location_angle, loa);
        arm = (" H = " + String(location_hight) + " D = " + String(location_distance) + " A =" + String(location_angle) + " A3 = "
               + String(loa));
        last_distance = location_distance;
        last_hight = location_hight;
        last_angle = location_angle;
        lsa = loa;
      }
      break;
    case 0:
      if (mecLR != 'S') {
        TA = readGyroAngle();
        TAset = false;
      } else {
        TAset = true;
      }
      switch (TAset) {
        case true:
          CA = readGyroAngle();
          break;
        case false:
          CA = TA;
          break;
          
      }
      error = TA - CA;
      integral += error;
      derivative = error - lastError;
      if (abs(error) >= 0.3 && abs(error) <= 30 && (direction != 'S' || steering != 'S' || mecLR != 'S')) change = kp * error + ki * integral + kd * derivative;
      else {
        change = 0;
      }
      controlmec(direction, steering, speed, turnSpeed, mecLR, mecturnSpeed, change);
      basedir = direction;
      basehor = steering;
      baserot = mecLR;
      dirval = speed;
      horval = turnSpeed;
      rotval = mecturnSpeed;
      //Serial.println("Target: " + String(TA) + ", Now: " + String(CA) + ", Correct: " + String(change));
      //序列繪圖家
      // Serial.print(TA);
      // Serial.print(",");
      // Serial.print(error);
      // Serial.print(",");
      // Serial.println(change);
      delay(1);
      lastError = error;
      break;
  }
  level = set_mode;
  if (ch7 == 0) {
    if (ch7 == 0 && lsch7 == 1) {
      if (lsmode == 0) {
        Auto_arm(200, 200, -65.82, 25);
      } else if (lsmode == 5) {
        Auto_arm(81.69, 286.12, -65.82, 61.02);
      }
      Auto_arm(200, 200, 0, 0);
      Auto_arm(239.55, 296.06, 2.23, 65.45);
      Auto_arm(179.55, 296.06, 2.23, 65.45);
      Auto_arm(239.55, 296.06, 2.23, 65.45);
    }
    set_mode = mode;
  } else {
    if (ch7 == 1 && lsch7 == 0) {
      switch (set_mode) {
        case 0:  //抓球
          Auto_arm(200, 200, -65.82, 25);
          Auto_arm(50.65, 266.12, -65.82, 63.02);
          Auto_arm(50.65, 407.54, -65.82, 63.02);
          Auto_arm(71.27, 407.54, -65.82, 63.02);
          Auto_arm(71.27, 286.12, -65.82, 63.02);
          Auto_arm(21.69, 286.12, -65.82, 63.02);
          lsmode = set_mode;
          break;
        case 1:  // 圓環
          suCase = 1;
          Auto_arm(239.55, 296.06, 2.23, 65.45);
          Auto_arm(179.55, 296.06, 2.23, 65.45);
          Auto_arm(239.55, 296.06, 2.23, 65.45);
          Auto_arm(850, 53.5, 0.04, -45);
          lsmode = set_mode;
          break;
        case 2:  // 箱子
          suCase = 1;
          Auto_arm(239.55, 296.06, 2.23, 65.45);
          Auto_arm(179.55, 296.06, 2.23, 65.45);
          Auto_arm(239.55, 296.06, 2.23, 65.45);
          Auto_arm(176.11, 366.83, 1.66, -1.86);
          lsmode = set_mode;
          break;
        case 3:  // TDK
          suCase = 1;
          Auto_arm(271.83, 404.57, 2.19, 83.48);
          Auto_arm(221.83, 404.57, 2.19, 83.48);
          Auto_arm(271.83, 404.57, 2.19, 83.48);
          Auto_arm(878.39, 21.04, 0.04, -100.97);
          lsmode = set_mode;
          break;
        case 4:  // 牌樓
          suCase = 1;
          Auto_arm(271.83, 404.57, 2.19, 83.48);
          Auto_arm(221.83, 404.57, 2.19, 83.48);
          Auto_arm(271.83, 404.57, 2.19, 83.48);
          Auto_arm(878.39, 21.04, 0.04, -100.97);
          lsmode = set_mode;
          break;
        case 5:  //無前搖取球
          Auto_arm(200, 200, -65.82, 25);
          Auto_arm(50.65, 266.12, -65.82, 63.02);
          suCase = 1;
          Auto_arm(21.69, 286.12, -65.82, 61.02);
          lsmode = set_mode;
          break;
        default:
          //Serial.println("mode error");
          break;
      }
    }
    // 更新 ch7 的上一个状态
  }
  lsch7 = ch7;
  if (carmode == 0) {
    arm_hight = map(mecSpeed, 0, 255, 0, armranges);
    arm_hight /= divt;
    float total = sqrt(location_distance * location_distance + location_hight * location_hight);
    if (total <= 880) {
      if (mecFB == 'F') {
        location_hight += arm_hight;
      } else if (mecFB == 'B' && location_hight - arm_hight >= -300) {
        location_hight -= arm_hight;
      }
    } else if (total >= 880) {
      if (mecFB == 'F' or mecFB == 'B') {
        location_hight -= arm_hight;
      }
    }

    if (location_distance != last_distance || location_hight != last_hight || location_angle != last_angle || loa != lsa) {
      if (speed == 0) {
        last_distance = location_distance;
      }
      if (mecSpeed == 0) {
        last_hight = location_hight;
      }
      if (turnSpeed == 0) {
        last_angle = location_angle;
      }
      if (mecturnSpeed == 0) {
        lsa = loa;
      }
    }
    Auto_arm(location_hight, location_distance, location_angle, loa);
  }
  Serial.print("base," + String(basedir) + ";" + String(dirval) + ";" + String(basehor) + ";" + String(horval) + ";" + String(baserot) + ";" + String(rotval) + ",level,");
  Serial.println(String(level) + ",su," + String(su) + ",grab," + String(grab) + ",tray," + String(tray) + ",arm," + arm + ",carmode," + String(carmode) + ",end," + String(armend));
}


float readGyroAngle() {  //讀取陀螺儀
  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    return receivedData.toFloat();
  } else {
    return 0;
  }
}

void arm_M_mode() {
  float total = sqrt(location_distance * location_distance + location_hight * location_hight);
  if (total <= 880) {
    // 執行要定時進行的操作
    if (direction == 'F') {
      location_distance += arm_distance;
    } else if (direction == 'B' and location_distance >= 0) {
      location_distance -= arm_distance;
    }
    if (steering == 'R' and location_angle <= 90) {
      location_angle += arm_angle;
    } else if (steering == 'L' and location_angle >= -90) {
      location_angle -= arm_angle;
    }

    else {  //旋轉極限保護
      if (steering == 'R' and location_angle > 90) {
        location_angle -= arm_angle;
      } else if (steering == 'L' and location_angle < -90) {
        location_angle += arm_angle;
      }
    }
    if (mecFB == 'F') {
      location_hight += arm_hight;
    } else if (mecFB == 'B' && location_hight - arm_hight >= -300) {
      location_hight -= arm_hight;
    }

    if (mecLR == 'R' and loa <= 90) {
      loa += armA;
    } else if (mecLR == 'L' and loa >= -150) {
      loa -= armA;
    }

    if (mecLR == 'R' and loa > 90) {
      loa = 90;
    } else if (mecLR == 'L' and loa < -150) {
      loa = -150;
    }

  } else if (total >= 880) {
    if (direction == 'F' or direction == 'B') {
      location_distance -= arm_distance;
    }
    if (mecFB == 'F' or mecFB == 'B') {
      location_hight -= arm_hight;
    }
  }

  if (location_distance != last_distance || location_hight != last_hight || location_angle != last_angle || loa != lsa) {
    if (speed == 0) {
      last_distance = location_distance;
    }
    if (mecSpeed == 0) {
      last_hight = location_hight;
    }
    if (turnSpeed == 0) {
      last_angle = location_angle;
    }
    if (mecturnSpeed == 0) {
      lsa = loa;
    }
  }
}

void Auto_arm(float hight, float distance, float angle, float A3) {
  armControl.inverseKinematics(hight, distance, angle, A3);
  Serial2.print(armControl.output + "\n");
  //Serial.print(armControl.output);
  location_distance = distance;
  location_hight = hight;
  location_angle = angle;
  loa = A3;

  last_distance = location_distance;
  last_hight = location_hight;
  last_angle = location_angle;
  lsa = loa;
  delay(50);
}