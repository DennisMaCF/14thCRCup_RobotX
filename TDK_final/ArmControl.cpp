#include "ArmControl.h"


ArmControl::ArmControl()
  : x(0), y(0), z(0) {
  Serial.begin(115200);  // 初始化串行通訊
}

void ArmControl::parseCoordinates(String input) {
  // 解析輸入的字串，將其轉換為 x, y, z 座標
  input.trim();
  int firstComma = input.indexOf(',');
  int secondComma = input.indexOf(',', firstComma + 1);

  if (firstComma > 0 && secondComma > firstComma) {
    x = (input.substring(0, firstComma).toFloat());
    y = (input.substring(firstComma + 1, secondComma).toFloat());
    z = (input.substring(secondComma + 1).toFloat());
    float total = sqrt(x * x + y * y + z * z);
  }

  else {
    //Serial.println("Invalid input format. Expected format: x,y,z");
  }
}

float ArmControl::getMaxValue(float det, float th1, float th2) {
  // 計算 det, th1, th2 的絕對值
  float absDet = abs(det);
  float absTh1 = abs(th1);
  float absTh2 = abs(th2);

  // 比較三個值的最大值
  float maxValue = absDet;
  if (absTh1 > maxValue) {
    maxValue = absTh1;
  }
  if (absTh2 > maxValue) {
    maxValue = absTh2;
  }
  return maxValue;
}


void ArmControl::calculateDifference(float det, float th1, float th2) {
  // 計算當前與之前的差異
  diff_det = det - previous_det;
  diff_th1 = th1 - previous_th1;
  diff_th2 = th2 - previous_th2;

  // 以 float 型態輸出差異值
}
// Function to validate the calculated angles and position
bool ArmControl::isValid(float theta1, float theta2, float delta) {
  // Implement your validation logic here
  // Example: Check if angles are within operational range
  if (theta1 > -180 and theta1 < 180) return false;
  if (theta2 > -180 and theta2 < 180) return false;
  // Add more checks as needed
  return true;
}

void ArmControl::inverseKinematics(float x, float y, float z, float a3_angle) {  // x = height, y = distance, z = angle
  const float pi = 3.141593;
  float j1 = 500;  // Length of the first arm segment
  float j2 = 380;  // Length of the second arm segment
  psi = a3_angle;
  psi = constrain(psi, -150, 90);
  // Z direction movement
  delta = z;

  // Calculate theta2 (in radians)
  theta2 = -acos((sq(x) + sq(y) - sq(j1) - sq(j2)) / (2 * j1 * j2));

  // Calculate theta1 (in radians)
  theta1 = atan(y / x) + atan((j2 * sin(theta2)) / (j1 + j2 * cos(theta2))) - pi;

  // Convert radians to degrees
  theta2 *= (180 / pi);
  theta1 *= (180 / pi);
  theta1 += 90;
  theta1 = -abs(theta1);

  /*
  if (theta2 < 0 && theta1 > 0) {
    if (theta1 < 90) {
      theta1 += (180 - (theta1 * 2));  // 繞 y 軸鏡像
      theta2 *= -1;
    } else if (theta1 > 90) {
      theta1 = theta1 - (2 * (theta1 - 90));  // 繞 y 軸鏡像
      theta2 *= -1;
    }
  } else if (theta1 < 0 && theta2 < 0) {
    theta1 *= -1;
    theta2 *= -1;
  }
  // Adjust for gripper orientation


  if (abs(theta1) >= 180) {
    if (theta1 > 0)
      theta1 -= 180;
    else if (theta1 < 0)
      theta1 += 180;
  }
  theta1 = -abs(90 - theta1);
  */
  if(x<=0 or  theta1>180)
   theta1+=180;
  theta3 = psi + theta2 - theta1;  // 計算 theta3

  if (abs(theta3) > 180) {
    if (theta3 > 180)
      theta3 -= 180;
    else if (theta3 < -180)
      theta3 += 180;
  }
  theta3 = constrain(theta3, -135, 135);
  theta1 = constrain(theta1, -150, 0);
  theta2 = constrain(theta2, -180, 0);
  delta = constrain(delta, -90, 90);
    // Check if the calculated values are valid; otherwise, use the previous valid values
    if (isValid(theta1, theta2, delta) != false) {
    // 使用前一个有效值
    theta1 = last_valid_th1;
    theta2 = last_valid_th2;
    delta = last_valid_det;
    Serial.println("error1");
  }
  else {
    // 记录当前有效值
    last_valid_th1 = theta1;
    last_valid_th2 = theta2;
    last_valid_det = delta;
  }

  // Generate G-code command with absolute position


  calculateDifference(delta, theta1, theta2);
  delaytimes = getMaxValue(diff_det, diff_th1, diff_th2);
  float F_fix = map((delaytimes * 100), 0, 10, 4800, 600);
  F_fix = constrain(F_fix, 4800, 600);

  output = "G90 G21 X" + String(delta) + " Y" + String(theta1) + " Z" + String(theta2)
           + " A" + String(theta3) + " F" + String(F_fix);
  //Serial.print(delaytimes);
  // Serial.print(" ");
  //F_fix = constrain(F_fix, 6000, 600);
  F_val = F_fix;
  previous_det = delta;
  previous_th1 = theta1;
  previous_th2 = theta2;
}
