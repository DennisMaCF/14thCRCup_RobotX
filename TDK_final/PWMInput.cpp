#include "PWMInput.h"
#include <math.h>

const int* PWMInput::_pins;
int PWMInput::_numPins;
volatile uint32_t* PWMInput::_pwmStart;
volatile uint32_t* PWMInput::_pwmWidth;

PWMInput::PWMInput(const int* pins, int numPins) {
  _pins = pins;
  _numPins = numPins;
  _pwmStart = new uint32_t[_numPins];
  _pwmWidth = new uint32_t[_numPins];
}

void PWMInput::begin() {
  for (int i = 0; i < _numPins; i++) {
    pinMode(_pins[i], INPUT);
    switch (i) {
      case 0: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt0, CHANGE); break;
      case 1: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt1, CHANGE); break;
      case 2: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt2, CHANGE); break;
      case 3: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt3, CHANGE); break;
      case 4: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt4, CHANGE); break;
      case 5: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt5, CHANGE); break;
      case 6: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt6, CHANGE); break;
      case 7: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt7, CHANGE); break;
      case 8: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt8, CHANGE); break;
      case 9: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt9, CHANGE); break;
      case 10: attachInterrupt(digitalPinToInterrupt(_pins[i]), handleInterrupt10, CHANGE); break;
    }
  }
}

int PWMInput::getWidth(int index) {
  if (index < 0 || index >= _numPins) {
    return -1;  
  }
  return _pwmWidth[index];
}

int PWMInput::sixCase(int width) {
  if (width == 0) {
    return 0;
  } else if (width >= 1050 && width <= 1100) {
    return 0;
  } else if (width < 1260) {
    return 1;
  } else if (width < 1430) {
    return 2;
  } else if (width < 1600) {
    return 5;
  } else if (width < 1780) {
    return 4;
  } else if (width < 1930) {
    return 3;
  }
}

int PWMInput::twoCase(int width) {
  if (width > 1500) {
    return 0;
  } else {
    return 1;
  } 
}

int PWMInput::twoCasenot(int width) {
  if (width > 1500) {
    return 1;
  } else {
    return 0;
  } 
}

int PWMInput::thrCase(int width) {
  if (width == 0) {
    return 0;
  } else if (width >= 1000 && width <= 1400) {
    return 0;
  } else if (width > 1400 && width <= 1900) {
    return 1;
  } else if (width > 1900) {
    return 2;
  }
}

int PWMInput::mapPWMToRange(int width) {
  if (width == 0) {
    return 0;
  } else if (width >= 1480 && width <= 1520) {
    return 0;
  } else if (width < 1480) {
    int value = map(width, 1100, 1480, 255, 0);  // 1100-1480 to 255-0
    return constrain(value, 0, 255);             // 0-255
  } else {
    int value = map(width, 1520, 1900, 0, 255);  // 1520-1900 to 0-255
    return constrain(value, 0, 255);             // 0-255
  }
}

char PWMInput::controlDirection(int width) {
  if (width >= 1480 && width <= 1520) {
    return 'S';  //Stop
  } else if (width < 1480) {
    return 'B';  //Backward
  } else {
    return 'F';  //Forward
  }
}
char PWMInput::controlmecDirection(int width) {
  if (width >= 1400 && width <= 1520) {
    return 'S';  //Stop
  } else if (width < 1400) {
    return 'F';  //Backward
  } else {
    return 'B';  //Forward
  }
}

char PWMInput::controlSteering(int width) {
  if (width >= 1480 && width <= 1520) {
    return 'S';  //Straight
  } else if (width < 1480) {
    return 'L';  //Left
  } else {
    return 'R';  //Right
  }
}

bool PWMInput::hasSignal(int ch11Width) {
  return ch11Width > 700;  
}

void PWMInput::handleInterrupt(int index) {
  if (digitalRead(_pins[index]) == HIGH) {
    _pwmStart[index] = micros();
  } else {
    _pwmWidth[index] = micros() - _pwmStart[index];
  }
}

void PWMInput::handleInterrupt0() {
  handleInterrupt(0);
}
void PWMInput::handleInterrupt1() {
  handleInterrupt(1);
}
void PWMInput::handleInterrupt2() {
  handleInterrupt(2);
}
void PWMInput::handleInterrupt3() {
  handleInterrupt(3);
}
void PWMInput::handleInterrupt4() {
  handleInterrupt(4);
}
void PWMInput::handleInterrupt5() {
  handleInterrupt(5);
}
void PWMInput::handleInterrupt6() {
  handleInterrupt(6);
}
void PWMInput::handleInterrupt7() {
  handleInterrupt(7);
}
void PWMInput::handleInterrupt8() {
  handleInterrupt(8);
}
void PWMInput::handleInterrupt9() {
  handleInterrupt(9);
}
void PWMInput::handleInterrupt10() {
  handleInterrupt(10);
}