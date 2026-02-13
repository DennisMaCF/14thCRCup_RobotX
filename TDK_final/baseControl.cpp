#include "baseControl.h"
#include <Arduino.h>

// 底盤直流馬達腳位(右前,右後,左前,左後)
// {en,in1,in2}
const int DCmotors[4][3] = {
  { 47, 21, 20 },  // v1: 右上
  { 4, 5, 6 },     // v2: 右下
  { 9, 10, 11 },   // v3: 左上
  { 12, 13, 14 }   // v4: 左下
};

double theta, x1, y2, r;
const double pi = 3.1415926;

double armXY(int y, int x) {
  theta = atan2(y, x);
  if (sqrt(x * x + y * y) > 255) {
    r = 255;
    x1 = r * cos(theta);
    y2 = r * sin(theta);
  } else {
    r = sqrt(x * x + y * y);
    x1 = x;
    y2 = y;
  }
  return theta;
}


void controlmec(char mecFB, char mecLR, int mecSpeed, int mecturnSpeed, char turn, int turnspeed, float change) {
  int v1, v2, v3, v4;
  int Vspeed[4];
  int Vturn;
  int X0, Y0;

  // 底盤馬達初始化
  for (int i = 0; i < 4; i++) {
    analogWrite(DCmotors[i][0], 0);
    digitalWrite(DCmotors[i][1], LOW);
    digitalWrite(DCmotors[i][2], LOW);
  }

  // 創建象限
  if (mecFB == 'B') {
    X0 = -mecSpeed;
  } else {
    X0 = mecSpeed;
  }
  if (mecLR == 'L') {
    Y0 = -mecturnSpeed;
  } else {
    Y0 = mecturnSpeed;
  }

  if (turn == 'L') {
    Vturn = -turnspeed*0.6;
  } else {
    Vturn = turnspeed*0.6;
  }

  // 得到兩向量所夾角度以及向量合成
  armXY(X0, Y0);


  // 根據馬達配置調整計算
  v1 = (r * sin(theta - pi / 4) - Vturn) - change;  // 右上
  v2 = (r * cos(theta - pi / 4) - Vturn) - change;  // 右下
  v3 = (r * cos(theta - pi / 4) + Vturn) + change;  // 左上
  v4 = (r * sin(theta - pi / 4) + Vturn) + change;  // 左下

  // 調整速度以防止超出範圍
  v1 = constrain(v1 * 1.414, -180, 180);
  v2 = constrain(v2 * 1.414, -180, 180);
  v3 = constrain(v3 * 1.414, -180, 180);
  v4 = constrain(v4 * 1.414, -180, 180);

  Vspeed[0] = v1;
  Vspeed[1] = v2;
  Vspeed[2] = v3;
  Vspeed[3] = v4;
  // Serial.print(v1);
  // Serial.print(" ");
  // Serial.print(v2);
  // Serial.print(" ");
  // Serial.print(v3);
  // Serial.print(" ");
  // Serial.print(v4);
  // Serial.print(" ");
  
  for (int i = 0; i < 4; i++) {
    if (Vspeed[i] > 0) {
      digitalWrite(DCmotors[i][1], HIGH);
      digitalWrite(DCmotors[i][2], LOW);
    } else if (Vspeed[i] < 0) {
      digitalWrite(DCmotors[i][1], LOW);
      digitalWrite(DCmotors[i][2], HIGH);
    } else {
      digitalWrite(DCmotors[i][1], LOW);
      digitalWrite(DCmotors[i][2], LOW);
    }
    analogWrite(DCmotors[i][0], abs(Vspeed[i]));
  }
}



