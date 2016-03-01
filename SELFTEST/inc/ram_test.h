/**
 * Company	: ThyssenKrupp PDC
 * @file	: ram_tes.h
 * @purpose     :
 * @brief	: header for c based RAM test POST RAM test 
 *                  IEC61508 verification on ETC-01 mainboard.
 * @version	: 1.0
 * @date	: 22-April-2014
 * @author	: Paul
*/

#ifndef _RAM_TEST_H__
#define _RAM_TEST_H__

//#include "iec60335_class_b_typedef.h"
#include "iec61508.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*! @addtogroup IEC61508_CONFIG
 * @{
 */

typedef enum testResult_enum
{
  IEC61508_testFailed     = 0,              /*!< test result failed replacement */
  IEC61508_testPassed     = 1,              /*!< test result passed replacement */
  IEC61508_testInProgress = 2               /*!< test is still in progress replacement */
} type_testResult_t;

typedef enum testState_enum
{
  IEC61508_stateDone       = 0,             /*!< test is done replacement */
  IEC61508_statePending    = 1,             /*!< test is pending replacement */
  IEC61508_stateInProgress = 2              /*!< test is still in progress replacement */
} type_testState_t;

typedef enum testStart_enum
{
  IEC61508_stop  = 0,                       /*!< test command stop */
  IEC61508_start = 1                        /*!< test command start */
} type_testStart_t;

/*	User configuration	*/

#define IEC61508_RAM_START    (0x20000000UL)    /*!< start address of RAM memory */
#ifndef SRAM_SIZE
#define SRAM_SIZE             (0x00004000UL)    /*!< size of RAM memory */
#endif
#define IEC61508_RAM_SIZE     SRAM_SIZE         /*!< size of RAM memory in kBytes */
  
/* BIST RAM buffer size in Byte */
#define IEC61508_RAM_buffersize    40
#define IEC61508_RAM_SP_Offset     (0x20UL)
/*!
 * @}
 */
/* ------------------------ */
/* RAM test suite status definitions */
#define TS_STA_RAM_BIST_OK          (0x00B00000UL)
#define TS_STA_RAM_BIST_R0_FAULT    (0x00B10000UL)
#define TS_STA_RAM_BIST_R1_FAULT    (0x00B20000UL)

#define BLOCK_SEL_CORE              0x1
#define BLOCK_SEL_PLINK             0x2

/* RAM algo constants */
#define initial_sp                  (*(uint32_t *) 0x00u)
#define get_initial_sp              (*initial_sp)
#define CORE_RAM_BASE_ADDR          IEC61508_RAM_START
#define CORE_RAM_BASE_PTR           (volatile uint8_t *) CORE_RAM_BASE_ADDR
#define CORE_RAM_MAX_BC             (1024 * 4 * 4)
#define CORE_RAM_R0_EXP             0x0
#define CORE_RAM_R1_EXP             (~CORE_RAM_R0_EXP)
#define PLINK_RAM_BASE_ADDR         (0x40012200UL)
#define PLINK_RAM_BASE_PTR          (volatile uint8_t *) PLINK_RAM_BASE_ADDR
#define PLINK_RAM_MAX_BC            384
#define PLINK_RAM_R0_EXP            0x0
#define PLINK_RAM_R1_EXP            0xFF

#define RAM_MEM_BASE         ((uint32_t) 0x20000000UL)  /**< RAM base address  */
#define RAM_MEM_SIZE         ((uint32_t) 0x4000UL)      /**< RAM available address space  */
#define RAM_MEM_END          ((uint32_t) 0x20003FFFUL)  /**< RAM end address  */
#define RAM_MEM_BITS         ((uint32_t) 0x15UL)        /**< RAM used bits  */
#define RAM_CODE_MEM_BASE    ((uint32_t) 0x10000000UL)  /**< RAM_CODE base address  */
#define RAM_CODE_MEM_SIZE    ((uint32_t) 0x4000UL)      /**< RAM_CODE available address space  */
#define RAM_CODE_MEM_END     ((uint32_t) 0x10003FFFUL)  /**< RAM_CODE end address  */
#define RAM_CODE_MEM_BITS    ((uint32_t) 0x14UL)        /**< RAM_CODE used bits  */

/*!
 * @brief   private function referenced by POST and BIST tests
 * @param   startAddr   start address in RAM memory (>0x20000000)
 * @param   size        size of RAM to check in bytes (<RAM_MEM_SIZE)
 * @param   block_sel   selection of RAM block to check, valid values are 
 *                  BLOCK_SEL_CORE for direct RAM access
 *                  BLOCK_SEL_PLINK for peripherals (ignoring size and startAddr)
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * The function allows check in RAM area only. The stack can not be tested here.
 * If an area is tested with stack inside the stack area is excluded. Interrupts
 * during the RAM test may change stack size and corrupt the test.
 */
type_testResult_t IEC61508_RAMtest(uint32_t startAddrs, uint32_t size, uint8_t block_sel);

/*!
 * @brief	POST public function testing the complete RAM
 * @return  passed or failed. See \link #testResult_enum testResult_t \endlink.
 * The function calls IEC60335_ClassB_RAMtest() with all available RAM memory as parameter.
 * The stack area is excluded from this test in fact there is no relevant stack 
 * depth expected at the POST test.
 */
extern type_testResult_t IEC61508_RAMtest_POST(void);

/*!
 * @brief   BIST public function referenced by BIST tests
 * @param   startAddr   start address in RAM
 * @param   length      length of RAM to check in Bytes
 * @return passed or failed. See \link #testResult_enum testResult_t \endlink.
 * @image html RAM-Test_BIST.jpg
 * The stack area is excluded from this test.
 */
extern type_testResult_t IEC61508_RAMtest_BIST(uint32_t startAddr, uint32_t length);


/*! @addtogroup IEC60335_TOOLS
 * @{
 */

/*!
 * @brief   public inline function referenced by BIST test
 *          A memcpy() function, if library is not used
 * @param   [in, out]  t uint8 *          ;Pointer to target buffer
 * @param   [in, out]  s uint8 *          ;Pointer to source buffer
 * @param   [in]       byte_count uint32  ;Byte count to copy from source to target
 * @remarks Does not detect overlapping source/target pointers
 */

static __INLINE void util_memcpy(void * t, void * s, uint32_t byte_count)
//static  void util_memcpy(void * t, void * s, uint32_t byte_count)
{
  uint8_t * ptr_t = (uint8_t *) t;
  uint8_t * ptr_s = (uint8_t *) s;
  while (byte_count)
  {
    byte_count--;
    *ptr_t = *ptr_s;
    ptr_s++;
    ptr_t++;
  }
}

/*!
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

/*!
 * @}
 */

#endif  /* _RAM_TEST_H__ */

/************************************** EOF *********************************/
