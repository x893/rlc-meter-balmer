#ifndef _NUMBER_EDIT_H_
#define _NUMBER_EDIT_H_

void NumberEditSetValue(float value, int powMinDelta, int powMaxDelta);
void NumberEditSetText(const char* text);

void NumberEditStart();
void NumberEditEnd();

float NumberEditGetValue();

bool NumberEditStarted();
bool NumberEditCompleted();

void NumberEditOnButtonPressed();
void NumberEditOnWeel(int16_t delta);
void NumberEditRepaint();

#endif//_NUMBER_EDIT_H_