/*******************************************************************************
* File Name          : update.c
* Author             : lison
* Version            : V1.0
* Date               : 03/23/2016
* Description        : This file contains app bin update functions.
*                      
*******************************************************************************/


/* Includes ------------------------------------------------------------------*/
#include "fatfs.h"
#include "ewdt.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*******************************************************************************
* Function Name  : isFileExist
* Description    : Checks whether a file exists
*                  
* Input          : filename: file name string
*                  
* Output         : None
* Return         : 0 exist
*******************************************************************************/
u8 isFileExist(char *filename)
{
  FIL* fp;
  FRESULT res = FR_NO_FILE;
  
  fp = (FIL*)mymalloc(sizeof(FIL));	
  if(fp != NULL)
  {    
      
      res = f_open(fp,filename,FA_READ);  
      printf("\r\n open res = %d \r\n",res);
      
      if(res==FR_OK)
      {
	 f_close(fp);						
      }
      myfree(fp);      
  }  
    
  return res;
  
}

/*******************************************************************************
* Function Name  : CopyFile
* Description    : Copy a new file.
*                  
* Input          : readfilename: Copied files name string
                   newfilename: New file name string.
*                  
* Output         : None
* Return         : 0 success
*******************************************************************************/
u8 CopyFile(char *readfilename, char *newfilename)
{
  FIL* fp1;
  FIL* fp2;
  FRESULT res = FR_NO_FILE;
    u8 *tempbuf;
    u16 bread;
    u32 offx=0;
  
  fp1 = (FIL*)mymalloc(sizeof(FIL));	
  fp2 = (FIL*)mymalloc(sizeof(FIL));	
  tempbuf = mymalloc(1024);
  
  
  if(fp1 != NULL && fp2 != NULL && tempbuf != NULL)
  {
    
      res = f_open(fp1,readfilename,FA_READ);
      
      res = f_open(fp2,newfilename,FA_CREATE_NEW | FA_WRITE);  
      printf("\r\n open res = %d \r\n",res);
      
        while(res==FR_OK)
        {
          res = f_read(fp1,tempbuf,1024,(UINT *)&bread);		
          if(res!=FR_OK)break;					
          res = f_write(fp2,tempbuf,bread,&offx);	  
          offx+=bread;
          if(bread!=1024)
          {                
                break;					
          }
        }       
      
        
       f_close(fp1); 
       f_close(fp2); 
        
      myfree(fp1);    
      myfree(fp2);
  }  
    
  return res;
  
}



/*******************************************************************************
* Function Name  : DeleteFile
* Description    : Delete a file.
*                  
* Input          : filename: deleted file name string.
*                  
* Output         : None
* Return         : 0£ºsuccess
*******************************************************************************/
u8 DeleteFile(char *filename)
{
    FRESULT res;
    
    res = f_unlink(filename);
    
    return res;
}





/******************************  END OF FILE  *********************************/

