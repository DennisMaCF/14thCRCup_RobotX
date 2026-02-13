#ifndef ArmControl_h
#define ArmControl_h

#include <Arduino.h>

class ArmControl {
public:
    ArmControl();
    void parseCoordinates(String input);
    void inverseKinematics(float height, float distance, float angle, float a3_angle);
    void calculateDifference(float y, float x, float z);
    String output;
    float diff_det;
    float diff_th1;
    float diff_th2;
    float delaytimes;
    float theta3;
    int F_val;

private:
    bool  isValid(float theta1, float theta2, float delta);
    float theta1; // 第一關節的角度
    float theta2; // 第二關節的角度
    float x; // 目標 x 座標
    float y; // 目標 y 座標
    float z; // 目標 z 座標
    float delta; // 基座旋轉角度
    //float theta3; // 夾持器的角度
    float psi = 0; // 夾持器的目標方向
    float lastTheta1;
    float lastTheta2;
    float lastDelta;
    float previous_det;
    float previous_th1;
    float previous_th2;
    float getMaxValue(float det, float th1, float th2);
    float last_valid_th1;
    float last_valid_th2 ;
    float last_valid_det ;
    String previousOutput;
};

#endif // ArmControl_h