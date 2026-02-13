#include <Arduino.h>

/*
e.g. 前後代表了(+100)or(-100) ...
左前輪 = 前後 + 左右 + 自轉
右前輪 = 前後 - 左右 - 自轉
左後輪 = 前後 - 左右 + 自轉
右後輪 = 前後 + 左右 - 自轉

順序:左前->右前->左後->右後

BLDC必需接的pin:
2(F/R) DigitalPin, 正轉反轉 -Gray
3(PWM) AnalogPin -Purple
6(on/off) DigitalPin, 馬達開關 -Yellow
8(GND) 地線 -Black
9(VM) 電源線 -Red
*/

/*
與主控raspberry pi的通訊, 直接使用arduino mega封裝好的UART即可
Serial1 對應 RX1與TX1
*/

// LF = LeftForward
// A0 = D54
const int LF_FR = A0;
const int LF_PWM = 2;
const int LF_OF = A1;

const int RF_FR = A2;
const int RF_PWM = 3;
const int RF_OF = A3;

const int LB_FR = A4;
const int LB_PWM = 4;
const int LB_OF = A5;

const int RB_FR = A6;
const int RB_PWM = 5;
const int RB_OF = A7;


void setup() {
    // 此Serial = Serial0 = debugSerial 默認連電腦USB
    // 與電腦通訊
    Serial.begin(115200);
    Serial.println("Inital text!");

    // 與Raspberry Pi通訊
    Serial1.begin(115200);
    Serial1.println("Hello, raspberry pi!");

    pinMode(LF_FR, OUTPUT);
    pinMode(LF_PWM, OUTPUT);
    pinMode(LF_OF, OUTPUT);

    pinMode(RF_FR, OUTPUT);
    pinMode(RF_PWM, OUTPUT);
    pinMode(RF_OF, OUTPUT);

    pinMode(LB_FR, OUTPUT);
    pinMode(LB_PWM, OUTPUT);
    pinMode(LB_OF, OUTPUT);

    pinMode(RB_FR, OUTPUT);
    pinMode(RB_PWM, OUTPUT);
    pinMode(RB_OF, OUTPUT);

    digitalWrite(LF_OF, HIGH);
    digitalWrite(RF_OF, HIGH);
    digitalWrite(LB_OF, HIGH);
    digitalWrite(RB_OF, HIGH);
}

void loop() {
    if (Serial1.available()) {
        int speedLF = Serial1.parseInt();
        int speedRF = Serial1.parseInt();
        int speedLB = Serial1.parseInt();
        int speedRB = Serial1.parseInt();

        if (Serial1.read() == '\n') {
            motorSpeeds(speedLF, speedRF, speedLB, speedRB);

        }
    }
}

// motor control function
void motorSpeeds(int speedLF, int speedRF, int speedLB, int speedRB) {
    /*
    FR, PWM, OF
    */
    // 左前輪
    if (speedLF > 0) {
        digitalWrite(LF_FR, HIGH);
        analogWrite(LF_PWM, speedLF);
    } else {
        digitalWrite(LF_FR, LOW);
        analogWrite(LF_PWM, abs(speedLF));
    }
    //右前輪
    if (speedRF > 0) {
        digitalWrite(RF_FR, HIGH);
        analogWrite(RF_PWM, speedRF);
    } else {
        digitalWrite(RF_FR, LOW);
        analogWrite(RF_PWM, abs(speedRF));
    }
    //左後輪
    if (speedLB > 0) {
        digitalWrite(LB_FR, HIGH);
        analogWrite(LB_PWM, speedLB);
    } else {
        digitalWrite(LB_FR, LOW);
        analogWrite(LB_PWM, abs(speedLB));
    }
    //右後輪
    if (speedRB > 0) {
        digitalWrite(RB_FR, HIGH);
        analogWrite(RB_PWM, speedRB);
    } else {
        digitalWrite(RB_FR, LOW);
        analogWrite(RB_PWM, abs(speedRB));
    }
}