#ifndef _MAP_H
#define _MAP_H

#include "stm32f405xx.h"
#include "stm32f4xx.h"

#include "controller.h"

#define VISITED 128 	// binary 1000 0000

#define POST '+'
#define WALL_HORIZONTAL '-'
#define WALL_VERTICAL '|'
#define CELL_VISITED 'x'
#define CELL_NOT_VISITED 'o'
#define SPACE ' '

void MAP_INIT(sMOUSE *mouse);
void MAP_ADD_WALL(sMOUSE *mouse, uint8_t wall_position);
void MAP_UPDATE(sMOUSE *mouse);
void MAP_PRINT(sMOUSE *mouse);

#endif
