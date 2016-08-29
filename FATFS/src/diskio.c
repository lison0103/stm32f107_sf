/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/


/* Includes ------------------------------------------------------------------*/
#include "diskio.h"		/* FatFs lower layer API */
#include "usbh_usr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/**  U disk, the volume label 0 **/
#define USB_DISK 0

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
 

	


/* Initialize Disk */
DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	u8 res=0;	    
	switch(pdrv)
	{
                
           case USB_DISK:
            /*  U disk is connected, it returns 0. Otherwise, it returns 1 */
            if(USBH_UDISK_Status())return 0;	 
            else return 1;	 
           default:
            res=1; 
	}		 
	if(res)return  STA_NOINIT;
	else return 0; 
}  

/* Obtaining state disk */
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{ 
	return 0;
} 

// Read Sector
// Dev: disk numbers 0 to 9
// * Buf: data buffer receiving the first address
// Sector: sector address
// Count: the number of sectors to be read
DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
    u8 res=0; 
    
    /* Count is not equal to 0, otherwise it returns an error parameter */
    if (!count)return RES_PARERR;		 	 
    switch(pdrv)
    {
       case USB_DISK: 
        res=USBH_UDISK_Read(buff,sector,count);	  
        break;
       default:
        res=1; 
    }
   
    /* Process the return value, the return value SPI SD driver.c turn into ff.c return value */
    if(res==0x00)return RES_OK;	 
    else return RES_ERROR;	   
}

// Write sector
// Drv: disk numbers 0 to 9
// * Buf: first address to send data
// Sector: sector address
// Count: the number of sectors to be written
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
    u8 res=0;  
    
    /* Count is not equal to 0, otherwise it returns an error parameter */
    if (!count)return RES_PARERR;		 	 
    switch(pdrv)
    {
       case USB_DISK:
        res=USBH_UDISK_Write((u8*)buff,sector,count); 
        break;
       default:
        res=1; 
    }
    
    /* Process the return value, the return value SPI SD driver.c turn into ff.c return value */
    if(res == 0x00)return RES_OK;	 
    else return RES_ERROR;	
}
#endif


// Get the other table parameters
// Drv: disk numbers 0 to 9
// Ctrl: control code
// * Buff: send / receive buffer pointer
#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;						  			     
	
	if(pdrv==USB_DISK)	
	{
	    switch(cmd)
	    {
		    case CTRL_SYNC:
                     res = RES_OK; 
		        break;	 
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff=512;
		        res = RES_OK;
		        break;	 
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff=512;
		        res = RES_OK;
		        break;	 
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff=USBH_MSC_Param.MSCapacity;
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }		
	}else res=RES_ERROR;//Does not support other
    return res;
}
#endif

//Get time
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
	return 0;
}	



/******************************  END OF FILE  *********************************/














