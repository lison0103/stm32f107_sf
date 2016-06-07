/*******************************************************************************
* File Name          : exfuns.c
* Author             : lison
* Version            : V1.0
* Date               : 03/23/2016
* Description        : This file contains fatfs externel functions.
*                      
*******************************************************************************/

#include "string.h"
#include "exfuns.h"
#include "fattester.h"	
#include "malloc.h"
#include "usart.h"

#define FILE_MAX_TYPE_NUM		7	/* Up FILE_MAX_TYPE_NUM categories */
#define FILE_MAX_SUBT_NUM		4	/* Up FILE_MAX_SUBT_NUM small class */

/* List of file types */
u8*const FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
{
{"BIN"},			
{"LRC"},			
{"NES"},			
{"TXT","C","H"},	
{"WAV","MP3","APE","FLAC"},
{"BMP","JPG","JPEG","GIF"},
{"AVI"},			
};

FATFS *fs[1];           //Logical Disk Workspace.	 
//FIL *file;	  		
UINT br,bw;		//Read write variables
FILINFO fileinfo;	//File information
DIR dir;  		//table of Contents


// Allocate memory for the exfuns
// Returns: 0 Success
// 1, failure
u8 exfuns_init(void)
{

	fs[0]=(FATFS*)mymalloc(sizeof(FATFS));	
//	file=(FIL*)mymalloc(sizeof(FIL));		
        
	if(fs[0])return 0; 
	else return 1;	
}


// Lowercase letters to uppercase letters for numbers, remain unchanged.
u8 char_upper(u8 c)
{
	if(c<'A')return c;
	if(c>='a')return c-0x20;
	else return c;
}	      

// report file Type
// Fname: filename
// Return Value: 0XFF, said it could not identify the type of file number.
// Other high four bits represent the relevant categories, the lower four bits indicate Sorts.
u8 f_typetell(u8 *fname)
{
	u8 tbuf[5];
	u8 *attr='\0';
	u8 i=0,j;
	while(i<250)
	{
		i++;
		if(*fname=='\0')break;
		fname++;
	}
	if(i==250)return 0XFF;
 	for(i=0;i<5;i++)
	{
		fname--;
		if(*fname=='.')
		{
			fname++;
			attr=fname;
			break;
		}
  	}
	strcpy((char *)tbuf,(const char*)attr);
 	for(i=0;i<4;i++)tbuf[i]=char_upper(tbuf[i]);
	for(i=0;i<FILE_MAX_TYPE_NUM;i++)	
	{
            for(j=0;j<FILE_MAX_SUBT_NUM;j++)
            {
                if(*FILE_TYPE_TBL[i][j]==0)break;
                if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)
                {
                    return (i<<4)|j;
                }
            }
	}
	return 0XFF;	 			   
}	 

// Get the remaining disk capacity
// Drv: Disk number ("0:" / "1:")
// Total: The total capacity (in KB)
// Free: remaining capacity (in KB)
// Return: 0, normal other error code.
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
    FATFS *fs1;
    u8 res;
    u32 fre_clust=0, fre_sect=0, tot_sect=0;

    res =(u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect=(fs1->n_fatent-2)*fs1->csize;	
	    fre_sect=fre_clust*fs1->csize;			
#if _MAX_SS!=512				  				
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif	  
		*total=tot_sect>>1;	
		*free=fre_sect>>1;	
 	}
	return res;
}	

/******************************  END OF FILE  *********************************/









