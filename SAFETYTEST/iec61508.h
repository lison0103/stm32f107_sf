/**
 * Company	: ThyssenKrupp PDC
 * @file	: iec61508.h
 * @purpose     :
 * @brief	: IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 22-April-2014
 * @author	: Paul
*/

#ifndef __IEC61508_
#define __IEC61508_

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/*	User configuration	*/
  
/* clock test */
typedef struct ClkTest_struct
{
  uint32_t Ratio;                           /*!< ration of timer ticks to RTC ticks, defines the number of timer calls in the test*/
  uint32_t Tolerance;                       /*!< tolerance of timer ticks, defines the deviation acceptable in the test*/
  uint32_t timerCounter;                    /*!< The counter Variable in the test */
  uint32_t baseTicks;                       /*!< basic counter occurrences */
  uint32_t result;                          /*!< result after test finished = (testResult_t + 1) */
} ClockTest_t;  
  
/* Interrupt test */
#define _VAL_A    0x01234567                /*!< values fixed to 32 bit regardless of compiler */
#define _VAL_B    0x12345678
#define _VAL_C    0x23456789
#define _VAL_D    0x3456789A
#define _VAL_E    0x456789AB
#define _VAL_F    0x56789ABC
#define _VAL_G    0x6789ABCD
/* possible return values */
enum _ReturnValues
{
  VAL_A = _VAL_A,
  VAL_B = _VAL_B,
  VAL_C = _VAL_C,
  VAL_D = _VAL_D,
  VAL_E = _VAL_E,
  VAL_F = _VAL_F,
  VAL_G = _VAL_G,
};
typedef enum _ReturnValues   PCTest_ReturnValues;

/* Program Counter test */
struct _Data
{
  PCTest_ReturnValues ret;
  uint32_t            number;        /*!< action type */
};
typedef struct _Data   Data_t;

typedef PCTest_ReturnValues (*Exec_t)(Data_t *data);


  #ifndef FLASH_SIZE
  #define FLASH_SIZE            (0x00020000UL)    /*!< size of FLASH memory */
  #endif
  #ifndef __INLINE
  #define __INLINE          inline                /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
  #endif

  #define FLASH_CRC_ADDR    0x000010BA            /* LENGTH = 0x20 */
  #define __STEXT           0x100
  
/* Flash test */
typedef struct IEC61508_FlashSign
{
  uint32_t CRC32Val;                            /*!< CRC32 value */
  uint32_t MemLenght;                           /*!< memory length for calculation */
  uint32_t NextAddress;                         /*!< next address in Flash */
  uint32_t BlockSize;                           /*!< block size of Flash */
  uint32_t Status;                              /*!< status information of this struct */
} FlashCRC_t;

/*	End of user configuration */
#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif
