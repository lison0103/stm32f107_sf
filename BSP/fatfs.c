/*******************************************************************************
* File Name          : update.c
* Author             : lison
* Version            : V1.0
* Date               : 03/23/2016
* Description        : This file contains fatfs operation functions.
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


FATFS *fs[1];           /* Logical Disk Workspace. */


/*******************************************************************************
* Function Name  : fatfs_init
* Description    : Allocate memory for the fatfs                 
* Input          : None                 
* Output         : None
* Return         : 0 Success, 1 failure
*******************************************************************************/
u8 fatfs_init(void)
{
        u8 initresult = 0u;
        
	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));			
        
	if(fs[0])
        {
            initresult = 0u; 
        }
	else
        {
            initresult = 1u;	
        }
        
        return initresult;
}

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
      
      if(res==FR_OK)
      {
	 f_close(fp);						
      }
      myfree(fp);      
  }  
    
  return res;
  
}

/*******************************************************************************
* Function Name  : ReadFile
* Description    : Read a file to buffer.
*                  
* Input          : readfilename: Read files name string
                   buffer: copy the contents of file to buffer.
*                  
* Output         : None
* Return         : read buffer len 
*******************************************************************************/
u16 ReadFile(char *readfilename, u8 buffer[])
{
    FIL* fp1;
    FRESULT res = FR_NO_FILE;
    u8 *tempbuf;
    u16 bread = 0u;
    u16 offx = 0u;
    u16 i;
    u8 result = 0u;
    
    fp1 = (FIL*)mymalloc(sizeof(FIL));		
    tempbuf = mymalloc(512u);
    
    
    if( (fp1 != NULL) && (tempbuf != NULL) )
    {
        
        res = f_open( fp1, readfilename, FA_READ );       
        
        while( res == FR_OK )
        {
            res = f_read( fp1, tempbuf, 512u, (UINT *)&bread );		
            if( res != FR_OK )
            {
                result = 1u;
            }
            else
            {
                for( i = 0u; i < bread; i++ )
                {
                    buffer[ offx + i ] = tempbuf[i];
                }
                
                offx += bread;
                if( bread != 512u )
                {              
                    result = 1u;					
                }
            }
            
            if(result)
            {
                break;
            }
        }       
                
        f_close(fp1);          
        myfree(fp1);    
        myfree(tempbuf);
    }  
    
    return offx;
    
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
    u32 bread;
    u32 offx = 0u;
    u8 result = 0u;
    
    fp1 = (FIL*)mymalloc(sizeof(FIL));	
    fp2 = (FIL*)mymalloc(sizeof(FIL));	
    tempbuf = mymalloc(1024u);
        
    if( (fp1 != NULL) && (fp2 != NULL) && (tempbuf != NULL))
    {
        
        res = f_open(fp1,readfilename,FA_READ);       
        res = f_open(fp2,newfilename,FA_CREATE_NEW | FA_WRITE);  
        
        while(res==FR_OK)
        {
            res = f_read(fp1,tempbuf,1024u,(UINT *)&bread);		
            if(res != FR_OK)
            {
                result = 1u;					
            }
            else
            {
                res = f_write(fp2,tempbuf,bread,&offx);	  
                offx += bread;
                if( bread != 1024u )
                {                
                    result = 1u;					
                }
            }
            if(result)
            {
                break;
            }
        }       
        
        
        f_close(fp1); 
        f_close(fp2);        
        myfree(fp1);    
        myfree(fp2);
        myfree(tempbuf);
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

