#ifndef _MENU_H_
#define _MENU_H_

#include <stdbool.h>

void MenuRepaint(void);
bool MenuIsOpen(void);

void MessageBox(char* line1);
void MessageBox2(char* line1, char* line2);
void OnCalibrationComplete(void);

#endif//_MENU_H_
