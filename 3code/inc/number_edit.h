#ifndef _NUMBER_EDIT_H_
#define _NUMBER_EDIT_H_

void NumberEditSetValue(float value, int powMinDelta, int powMaxDelta);
void NumberEditSetText(const char* text);

void NumberEditStart(void);
void NumberEditEnd(void);

float NumberEditGetValue(void);

bool NumberEditStarted(void);
bool NumberEditCompleted(void);

void NumberEditOnButtonPressed(void);
void NumberEditOnWeel(int16_t delta);
void NumberEditRepaint(void);

#endif//_NUMBER_EDIT_H_
