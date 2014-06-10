// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include <stdbool.h>

void QuadEncInit();

/*
Раз в 20 ms проверяется не нажата ли кнопка и не повернулось ли колесико.
Если событие произошло, то вызывается соответсвующий обработчик.
*/
void OnButtonPressed();
void OnWeel(int16_t delta);
void OnTimer();
