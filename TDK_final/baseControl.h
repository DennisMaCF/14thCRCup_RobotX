#ifndef BASE_CONTROL_H
#define BASE_CONTROL_H

extern const int DCmotors[4][3];

void controlmec(char mecFB, char mecLR, int mecSpeed, int mecturnSpeed, char turn, int turnspeed , float change);

#endif // BASE_CONTROL_H
