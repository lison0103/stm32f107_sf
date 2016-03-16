#ifndef __ESC_ERROR_PROCESS_H
#define __ESC_ERROR_PROCESS_H


extern int EscBuff[10];

#define EN_ERROR_SYS1 EscBuff[0]
#define EN_ERROR_SYS2 EscBuff[1]
#define EN_ERROR_SYS3 EscBuff[2]

void ESC_EWDT_Error_Process(void);
void ESC_Flash_Error_Process(void);
void ESC_Fram_Error_Process(void);
void ESC_SPI_Error_Process(void);
void ESC_SafeRelay_Error_Process(void);

#endif