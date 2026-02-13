#ifndef PWMInput_h
#define PWMInput_h

#include <Arduino.h>

class PWMInput {
public:
  PWMInput(const int* pins, int numPins);
  void begin();
  int getWidth(int index);
  int mapPWMToRange(int width);
  char controlDirection(int width);
  char controlmecDirection(int width);
  char controlSteering(int width);
  bool hasSignal(int ch11Width);
  int sixCase(int width);
  int twoCase(int width);
  int twoCasenot(int width);
  int thrCase(int width);

private:
  static const int* _pins;
  static int _numPins;
  static volatile uint32_t* _pwmStart;
  static volatile uint32_t* _pwmWidth;
  static void handleInterrupt0();
  static void handleInterrupt1();
  static void handleInterrupt2();
  static void handleInterrupt3();
  static void handleInterrupt4();
  static void handleInterrupt5();
  static void handleInterrupt6();
  static void handleInterrupt7();
  static void handleInterrupt8();
  static void handleInterrupt9();
  static void handleInterrupt10();
  static void handleInterrupt(int index);
};

#endif
