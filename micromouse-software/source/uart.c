#include "uart.h"

tCircular_buffer UART_Buffer;

void UART1_IT_Init(void)
{
	// clock enable for A port, USART1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	// PA9, PA10 as an alternate function for USART1_TX and USART1_RX
	GPIOA->MODER &= ~GPIO_MODER_MODE9 & ~GPIO_MODER_MODE10;
	GPIOA->MODER |= GPIO_MODER_MODE9_1 | GPIO_MODER_MODE10_1;
	
	GPIOA->AFR[1] |= 0x00000770;
	
	// USART1 setup
	USART1->BRR = (SystemCoreClock / 115200);	// baud rate 115200
	USART1->CR1 |=  USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_PCE | USART_CR1_M; // USART enable transmitter | reciever | USART enable | recieve interrupt | parity check enable | 9 bits frame
	NVIC_EnableIRQ(USART1_IRQn);
}

void UART1_COMMAND_PARSERHandler(tCircular_buffer *hBuffer)
{
	if(hBuffer->commands_cnt > 0)
	{
		uint8_t tmp_buf;
		uint8_t single_command[32] = {NULL};
		uint8_t tmp_cnt = 0;
		eCircular_buffer_status result;
		
		do
		{
			result = tCircular_buffer_pop(&UART_Buffer, &tmp_buf);
			
			if(result == BUFFER_EMPTY) 
			{
				UART1_Log("Unknown command!\r\n");
				break;
			}
		}	while(tmp_buf != '$');
		
		tmp_cnt = 0;
		
		do
		{
			result = tCircular_buffer_pop(&UART_Buffer, &tmp_buf);
			
			if(result == BUFFER_EMPTY) break;
			
			if(tmp_buf != '#') single_command[tmp_cnt] = tmp_buf;
			tmp_cnt++;
		}	while(tmp_buf != '#');
		
		COMMAND_Execute((char *)single_command);
	}
}
void COMMAND_Execute(char *command)
{
	switch(COMMAND_GET_TYPE(command))
	{
		case UNKNOWN:
		{
			char buf[64];
			sprintf(buf, "Unknown command!\t $%s# \r\n", command);
			
			UART1_Log(buf);
			
			break;
		}
		case LED:
		{
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			LED_Switch((eLEDnum)(atoi(param_buffer[0])), (eLEDstate)(atoi(param_buffer[1])));
			
			break;
		}
		case MOTOR:
		{
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			float left_motor_speed = (float)atof(param_buffer[0]);
			float right_motor_speed = (float)atof(param_buffer[1]);
			
			MOTOR_LEFT.set_rpm = left_motor_speed;
			MOTOR_RIGHT.set_rpm = right_motor_speed;
			
			break;
		}
		case BUZZER:
		{
			
			break;
		}
		case STATE:
		{
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == '?')
			{
				char buf[32];
				sprintf(buf, "Actual mouse state = %i \r\n", MOUSE.state);
				UART1_Log(buf);
				UART1_Log(STATE_HELP);
			}
			else if((atoi(param_buffer[0]) >= 0) && ((atoi(param_buffer[0]) <= 15)))
			{
				MOUSE.state = (eMouseState)(atoi(param_buffer[0]));
			}
			else
			{
			UART1_Log("Unknown state! \r\n");	
			UART1_Log(STATE_HELP);
			}				
			
			break;
		}
		case BATTERY:
		{	
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == 'G' && param_buffer[0][1] == 'E' && param_buffer[0][2] == 'T')
			{
			char buf[128];
			sprintf(buf,"\r\nBattery voltage = %.2fV\r\nVREFINT voltage = %.2fV\r\nProcessor temperature = %.1fC\r\n",ADC_GET_BATTERY_VOLTAGE(),ADC_GET_VREF_INTERNAL(),ADC_GET_TEMPERATURE_INTERAL());
			UART1_Log(buf);
			}
			
			break;
		}
		case READY:
		{	
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == '?')
			{
			UART1_Log("READY\r\n");
			}
			
			break;
		}
		case MOVE:
		{	
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == 'F' && param_buffer[0][1] == 'W' && param_buffer[0][2] == 'D')
			{
				uint8_t num_of_cells = atoi(param_buffer[1]);
				
				if(num_of_cells > 0) MOVE_CELL_FORWARD(&MOUSE, num_of_cells);
			}
			else
			{
			float posX = (float)atof(param_buffer[0]);
			float posY = (float)atof(param_buffer[1]);
			
			delay_ms(1000);
			
			MOVE_SET_POSITION(&MOUSE, posX, posY);
			}
			break;
		}
		case ROTATE:
		{	
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			float ang = (float)atof(param_buffer[0]);
			
			MOVE_SET_ORIENTATION(&MOUSE, ang);
			
			break;
		}
		case POSITION:
		{	
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == 'G' && param_buffer[0][1] == 'E' && param_buffer[0][2] == 'T')
			{
			char buf[256];
			char direction[12];
				
			if(MOUSE.face_direction == NORTH)	sprintf(direction, "NORHT\r\n");
			else if(MOUSE.face_direction == SOUTH)	sprintf(direction, "SOUTH\r\n");
			else if(MOUSE.face_direction == EAST)	sprintf(direction, "EAST\r\n");
			else if(MOUSE.face_direction == WEST)	sprintf(direction, "WEST\r\n");
			else sprintf(direction, "UNKNOWN\r\n");
				
			sprintf(buf, "\r\nX = %1.f\r\nY = %1.f\r\nAng = %1.f\r\nEncL = %d\r\nEncR = %d\r\nTotalDistL = %1.f\r\nTotalDistR = %1.f\r\nIndx = %d\r\n", (double)MOUSE.actual_position_x, (double)MOUSE.actual_position_y, (double)MOUSE.actual_angle, MOTOR_LEFT.enc, MOTOR_RIGHT.enc,MOTOR_LEFT.totalDist, MOTOR_RIGHT.totalDist, MOUSE.current_map_index);	
			UART1_Log(buf);
			UART1_Log(direction);
			}
			
			break;
		}
		case SENSORS:
		{	
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == 'G' && param_buffer[0][1] == 'E' && param_buffer[0][2] == 'T')
			{	
				char buf[512];
				double LF,RF,LS,RS;
					
				if(param_buffer[1][0] == 'R' && param_buffer[1][1] == 'A' && param_buffer[1][2] == 'W')
				{
					LF = SENSOR_GET_LEFT_FRONT_DISTANCE(RAW);
					RS = SENSOR_GET_RIGHT_SIDE_DISTANCE(RAW);
					LS = SENSOR_GET_LEFT_SIDE_DISTANCE(RAW);
					RF = SENSOR_GET_RIGHT_FRONT_DISTANCE(RAW);
				}
				else if(param_buffer[1][0] == 'M' && param_buffer[1][1] == 'M')
				{
					LF = SENSOR_GET_LEFT_FRONT_DISTANCE(MM);
					RS = SENSOR_GET_RIGHT_SIDE_DISTANCE(MM);
					LS = SENSOR_GET_LEFT_SIDE_DISTANCE(MM);
					RF = SENSOR_GET_RIGHT_FRONT_DISTANCE(MM);
				}
				else if(param_buffer[1][0] == 'C' && param_buffer[1][1] == 'M')
				{
					LF = SENSOR_GET_LEFT_FRONT_DISTANCE(CM);
					RS = SENSOR_GET_RIGHT_SIDE_DISTANCE(CM);
					LS = SENSOR_GET_LEFT_SIDE_DISTANCE(CM);
					RF = SENSOR_GET_RIGHT_FRONT_DISTANCE(CM);
				}
				else
				{
					LF = SENSOR_GET_LEFT_FRONT_DISTANCE(RAW);
					RS = SENSOR_GET_RIGHT_SIDE_DISTANCE(RAW);
					LS = SENSOR_GET_LEFT_SIDE_DISTANCE(RAW);
					RF = SENSOR_GET_RIGHT_FRONT_DISTANCE(RAW);
				}
					
					
				sprintf(buf, "\r\n+-----------------------+\r\n|         ^    ^        |\r\n|         |    |        |\r\n|%.2f   |    |%.2f  |\r\n|         |    |        |\r\n|         |    |        |\r\n|         +----+        |\r\n|        ++    ++       |\r\n|<-------+|    |+------>|\r\n|%.2f  ++    ++%.2f  |\r\n|         +----+        |\r\n|                       |\r\n+-----------------------+", \
				LF,RF, \
				LS,RS);

				UART1_Log(buf);
			}
			
			if((param_buffer[0][0] == 'L' && param_buffer[0][1] == 'F'))
			{
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'N')
				{
					IR_LEFT_FRONT_ON;
				}
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'F' && param_buffer[1][2] == 'F')
				{
					IR_LEFT_FRONT_OFF;
				}
			}
			
			if((param_buffer[0][0] == 'R' && param_buffer[0][1] == 'F'))
			{
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'N')
				{
					IR_RIGHT_FRONT_ON;
				}
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'F' && param_buffer[1][2] == 'F')
				{
					IR_RIGHT_FRONT_OFF;
				}
			}
			
			if((param_buffer[0][0] == 'L' && param_buffer[0][1] == 'S'))
			{
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'N')
				{
					IR_LEFT_SIDE_ON;
				}
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'F' && param_buffer[1][2] == 'F')
				{
					IR_LEFT_SIDE_OFF;
				}
			}
			
			if((param_buffer[0][0] == 'R' && param_buffer[0][1] == 'S'))
			{
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'N')
				{
					IR_RIGHT_SIDE_ON;
				}
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'F' && param_buffer[1][2] == 'F')
				{
					IR_RIGHT_SIDE_OFF;
				}
			}
			if((param_buffer[0][0] == 'A' && param_buffer[0][1] == 'L' && param_buffer[0][2] == 'L'))
			{
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'N')
				{
					IR_LEFT_FRONT_ON;
					IR_RIGHT_FRONT_ON;
					IR_LEFT_SIDE_ON;
					IR_RIGHT_SIDE_ON;
				}
				if(param_buffer[1][0] == 'O' && param_buffer[1][1] == 'F' && param_buffer[1][2] == 'F')
				{
					IR_LEFT_FRONT_OFF;
					IR_RIGHT_FRONT_OFF;
					IR_LEFT_SIDE_OFF;
					IR_RIGHT_SIDE_OFF;
				}
			}
			
			break;
		}
		case NRST:
		{
			NVIC_SystemReset();
			
			break;
		}
		case MAP:
		{
			char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS];
			GET_COMMAND_PARAMS(command, param_buffer);
			
			if(param_buffer[0][0] == 'G' && param_buffer[0][1] == 'E' && param_buffer[0][2] == 'T')
			{	
				MAP_PRINT(&MOUSE);
			}
			
			if(param_buffer[0][0] == 'U' && param_buffer[0][1] == 'P' && param_buffer[0][2] == 'D' && param_buffer[0][3] == 'A' && param_buffer[0][4] == 'T' && param_buffer[0][5] == 'E')
			{	
				MAP_UPDATE(&MOUSE);
			}
			
			break;
		}
	}
}
eCOMMANDS COMMAND_GET_TYPE(char *command)
{
	unsigned int counter = 0;
	char command_type[16];
	eCOMMANDS type = UNKNOWN;
	
	while( (command[counter] != '=') && (counter < strlen(command)) )
	{
		command_type[counter] = command[counter];
		
		counter++;
	}
	
	if(strncmp(command_type, "LED", counter) == 0) type = LED;
	else if(strncmp(command_type, "MOTOR", counter) == 0) type = MOTOR;
	else if(strncmp(command_type, "BUZZER", counter) == 0) type = BUZZER;
	else if(strncmp(command_type, "STATE", counter) == 0) type = STATE;
	else if(strncmp(command_type, "BATTERY", counter) == 0) type = BATTERY;
	else if(strncmp(command_type, "READY", counter) == 0) type = READY;
	else if(strncmp(command_type, "MOVE", counter) == 0) type = MOVE;
	else if(strncmp(command_type, "ROTATE", counter) == 0) type = ROTATE;
	else if(strncmp(command_type, "POSITION", counter) == 0) type = POSITION;
	else if(strncmp(command_type, "SENSORS", counter) == 0) type = SENSORS;
	else if(strncmp(command_type, "NRST", counter) == 0) type = NRST;
	else if(strncmp(command_type, "MAP", counter) == 0) type = MAP;
	
	return type;
}
unsigned int GET_COMMAND_PARAMS(char *command, char param_buffer[PARAM_BUFFER_ROWS][PARAM_BUFFER_COLS])
{
  unsigned int counter = 0, single_param_counter = 0, params_counter = 0;

	while(command[counter++] != '=') {}

	command = &command[counter];

    counter = 0;
    while(counter < strlen(command))
    {
        if(command[counter] == ',')
        {
           params_counter++;
           single_param_counter = 0;
        }
        else
        {
            param_buffer[params_counter][single_param_counter] = command[counter];
            single_param_counter++;
        }

    counter++;
    }

    return (params_counter + 1);
}
void UART1_SendChar(char c)
{
	while(!(USART1->SR & USART_SR_TXE)) {}
		
	USART1->DR = c;
}
void UART1_Log(const char *message)
{
	while(*message)
	{
		UART1_SendChar(*message++);
	}
}
void USART1_IRQHandler(void)
{
	if ( (USART1->SR & USART_SR_RXNE ) != RESET)
	{
		volatile uint8_t data = (uint8_t)USART1->DR;
		
		tCircular_buffer_push(&UART_Buffer, (uint8_t)USART1->DR);
	}
}

