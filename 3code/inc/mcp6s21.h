#ifndef _MCP6S21_H_
#define _MCP6S21_H_

#include <stdint.h>
#include <stdbool.h>

void MCPInit(void);
void MCPSetGain(bool voltage, uint8_t gain);

#endif//_MCP6S21_H_
