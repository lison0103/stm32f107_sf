


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ESC_CONTROL_H
#define __ESC_CONTROL_H

/* Includes ------------------------------------------------------------------*/
#include "esc.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Esc_Control(void);
void Brake_Status_Check(void);
void Feedback_Contactors_Check(void);
void Aux_Brake_CS(void);
void Floorplate_Check(void);
void Inspection_Normal_Key_Check(void);


#endif  /* __ESC_CONTROL_H */
