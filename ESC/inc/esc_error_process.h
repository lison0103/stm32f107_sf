#ifndef __ESC_ERROR_PROCESS_H
#define __ESC_ERROR_PROCESS_H


extern int EscBuff[10];

#define EN_ERROR_SYS1 EscBuff[0]

void ESC_EWDT_Error_Process(void);
void ESC_Flash_Error_Process(void);
void ESC_Fram_Error_Process(void);
void ESC_SPI_Error_Process(void);

#endif