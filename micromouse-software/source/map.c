#include "map.h"

#include "controller.h"
#include "uart.h"

/*
Every map's cell coded on 1 byte as follow : 2#0000 NESW
If the wall is present on the direction there is 1 in code if not there is 0.

														N
		+---+---+---+					+---+
		|   |		|		|					|		|
		|	15|	16|	17|				W	|		| E
		|		|		|		|					|		|
		+---+---+---+					+---+
		|		|		|		|						S
		|	12| 13|	14|
		|		|		|		|						8
		+---+---+---+					+---+
		|		|		|		|					|		|
		|	9	|	10|	11|				1	|		| 4
		|		|		|		|					|		|
		+---+---+---+					+---+
		|		|		|		|						2
		|	6	|	7	|	8	|
		|		|		|		|
		+---+---+---+
		|		|		|		|
		|	3	|	4	|	5	|
		|		|		|		|
		+---+---+---+
		|		|		|		|
		|	0	|	1	|	2	|
		|		|		|		|
		+---+---+---+

*/


void MAP_INIT(sMOUSE *mouse)
{
	for(int i = 0; i < 18; i++)
	{
	mouse->map[i] = 0;
	}
	
	mouse->current_map_index = 0;
	MAP_ADD_WALL(mouse, WEST);
	MAP_ADD_WALL(mouse, SOUTH);
	mouse->current_map_index = 1;
	MAP_ADD_WALL(mouse, SOUTH);
	mouse->current_map_index = 2;
	MAP_ADD_WALL(mouse, EAST);
	MAP_ADD_WALL(mouse, SOUTH);
	mouse->current_map_index = 3;
	MAP_ADD_WALL(mouse, WEST);
	mouse->current_map_index = 6;
	MAP_ADD_WALL(mouse, WEST);
	mouse->current_map_index = 9;
	MAP_ADD_WALL(mouse, WEST);
	mouse->current_map_index = 12;
	MAP_ADD_WALL(mouse, WEST);
	mouse->current_map_index = 15;
	MAP_ADD_WALL(mouse, WEST);
	MAP_ADD_WALL(mouse, NORTH);
	mouse->current_map_index = 16;
	MAP_ADD_WALL(mouse, NORTH);
	mouse->current_map_index = 17;
	MAP_ADD_WALL(mouse, EAST);
	MAP_ADD_WALL(mouse, NORTH);
	mouse->current_map_index = 14;
	MAP_ADD_WALL(mouse, EAST);
	mouse->current_map_index = 11;
	MAP_ADD_WALL(mouse, EAST);
	mouse->current_map_index = 8;
	MAP_ADD_WALL(mouse, EAST);
	mouse->current_map_index = 5;
	MAP_ADD_WALL(mouse, EAST);
	
	mouse->current_map_index = 0;
}
void MAP_ADD_WALL(sMOUSE *mouse, uint8_t wall_position)
{
	mouse->map[mouse->current_map_index] |= wall_position;
	
	if(wall_position == NORTH) 
	{           
		mouse->map[mouse->current_map_index + 3] |= SOUTH; // index pointing on cell above
	} 
	if(wall_position == EAST) 
	{ 
		mouse->map[mouse->current_map_index + 1] |= WEST;	// index pointing on cell on right
	} 
	if(wall_position == SOUTH) 
	{ 
		uint8_t index_temp;
		index_temp = mouse->current_map_index + 253; // same as sub 3, index pointing on cell below
		mouse->map[index_temp] = mouse->map[index_temp] | NORTH;	
	} 
	if(wall_position == WEST) 
	{ 
		uint8_t index_temp;
		index_temp = mouse->current_map_index + 255;	// same as sub 1, index pointing on cell on left
		mouse->map[index_temp] = mouse->map[index_temp] | EAST;
	}
}
void MAP_UPDATE(sMOUSE *mouse)
{
	if((MOUSE.map[MOUSE.current_map_index] & VISITED) == 0)
	{
		mouse->map[mouse->current_map_index] |= VISITED;
		
		switch(mouse->face_direction)
		{
			case NORTH: 
				{
					if(mouse->left_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, WEST);
					}
					if(mouse->right_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, EAST);
					}
					if(mouse->right_front_sensor_mm < 140.f && mouse->left_front_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, NORTH);
					}	
				break;
			}
			case EAST: 
				{
					if(mouse->left_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, NORTH);
					}
					if(mouse->right_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, SOUTH);
					}
					if(mouse->right_front_sensor_mm < 140.f && mouse->left_front_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, EAST);
					}					
				break;
			}
			case SOUTH: 
				{
					if(mouse->left_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, EAST);
					}
					if(mouse->right_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, WEST);
					}
					if(mouse->right_front_sensor_mm < 140.f && mouse->left_front_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, SOUTH);
					}					
				break;
			}
			case WEST: 
				{
					if(mouse->left_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, SOUTH);
					}
					if(mouse->right_side_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, NORTH);
					}
					if(mouse->right_front_sensor_mm < 140.f && mouse->left_front_sensor_mm < 140.f)
					{
						MAP_ADD_WALL(mouse, WEST);
					}			
				break;
			}			
		}
	}
}
void MAP_PRINT(sMOUSE *mouse)
{
	char buf[13];
	uint8_t index = 15;
	uint8_t count = 0;
	
	UART1_Log("\r\n");
	
	for(int j = 24; j >= 0; j--)
	{
		if(j % 4 == 0)
		{
			buf[0] = POST;
				if((mouse->map[index] & NORTH) != 0)
				{
					buf[1] = WALL_HORIZONTAL;
					buf[2] = WALL_HORIZONTAL;
					buf[3] = WALL_HORIZONTAL;
				}
				else
				{
					buf[1] = SPACE;
					buf[2] = SPACE;
					buf[3] = SPACE;
				}
			buf[4] = POST;
				if((mouse->map[index + 1] & NORTH) != 0)
				{
					buf[5] = WALL_HORIZONTAL;
					buf[6] = WALL_HORIZONTAL;
					buf[7] = WALL_HORIZONTAL;
				}
				else
				{
					buf[5] = SPACE;
					buf[6] = SPACE;
					buf[7] = SPACE;
				}
			buf[8] = POST;
				if((mouse->map[index + 2] & NORTH) != 0)
				{
					buf[9] 	= WALL_HORIZONTAL;
					buf[10] = WALL_HORIZONTAL;
					buf[11] = WALL_HORIZONTAL;
				}
				else
				{
					buf[9] 	= SPACE;
					buf[10] = SPACE;
					buf[11] = SPACE;
				}
			buf[12] = POST;
				
			count++;
			UART1_Log(buf);
			UART1_Log("\r\n");
		}
		else
		{
			buf[0] = WALL_VERTICAL;
			buf[1] = SPACE;
			buf[2] = SPACE;
				if(j == 22 | j == 18 | j == 14 | j == 10 | j == 6 | j == 2) 
				{
					if((mouse->map[index] & VISITED) != 0)
					{
						buf[2] = CELL_VISITED;
					}
					else 
					{
						buf[2] = CELL_NOT_VISITED;
					}
				}
			buf[3] = SPACE;
			if((mouse->map[index] & EAST) != 0)
			{
				buf[4] = WALL_VERTICAL;
			}
			else
			{
				buf[4] = SPACE;
			}
			buf[5] = SPACE;
			buf[6] = SPACE;
				if(j == 22 | j == 18 | j == 14 | j == 10 | j == 6 | j == 2) 
				{
					if((mouse->map[index + 1] & VISITED) != 0)
					{
						buf[6] = CELL_VISITED;
					}
					else 
					{
						buf[6] = CELL_NOT_VISITED;
					}
				}			
			buf[7] = SPACE;
			if((mouse->map[index + 1] & EAST) != 0)
			{
				buf[8] = WALL_VERTICAL;
			}
			else
			{
				buf[8] = SPACE;
			}
			buf[9] = SPACE;
			buf[10] = SPACE;
				if(j == 22 | j == 18 | j == 14 | j == 10 | j == 6 | j == 2) 
				{
					if((mouse->map[index + 2] & VISITED) != 0)
					{
						buf[10] = CELL_VISITED;
					}
					else 
					{
						buf[10] = CELL_NOT_VISITED;
					}
				}			
			buf[11] = SPACE;
			buf[12] = WALL_VERTICAL;
			
			count++;
			if(count == 4)
			{
				count = 0;
				index -= 3;
			}
			UART1_Log(buf);
			UART1_Log("\r\n");
		}
	}
}
