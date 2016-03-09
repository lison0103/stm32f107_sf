#include "esc_comm_db.h"
//#include "esc.h" 
#include "usart.h" 
#include "crc16.h" 

#include "stm32f10x_crc.h"


u8 Modbuff[5400]; 


u8 EscRTBuff[200],McRxBuff[1000];    

u16 *const ptModbuff  = (u16*)Modbuff; 

//------------------------------------------------------------------------------------------- ModBus
u8 *const pcMbRtccBuff = &Modbuff[50];  				//6字节

//modbus 
u8 *const pcEscErrorCodeBuff = &Modbuff[454];  	//5字节
u8 *const pcEscStateBuff = &Modbuff[460];	    	//4字节 
u8 *const pcEscAlarmBuff = &Modbuff[464];				//4字节
u8 *const pcEscErrorBuff = &Modbuff[468];				//32字节

//统计数组
u8 *const pcStatDataBuff = &Modbuff[500];  			
//记录故障
u16 *const ptErrorRecordFirstItem = (u16*)&Modbuff[530];

//系统参数
u8 *const ParaBuff = &Modbuff[1100];
u16 *const ptParaBuff = (u16*)&Modbuff[1100];

// 通信配置40676																	
//u16 *ptParaCommCgcBuff = (u16*)&Modbuff[1350];	//242字节
//u8 *pcParaCommCgcBuff = &Modbuff[1350];			//242字节

//多段速配置 50字节
u8 *const pcParaMultiSpeedBuff = &Modbuff[1600];	//50字节

//故障复位bit标志
u8 *const pcParaERB_DecodeBuff = &Modbuff[1660];		//32字节

//故障复位方式，人工复位252字节
u8 *const pcParaErrorResetBuff = &Modbuff[1700];		//252字节

//记录故障
u8 *const pcRtDataBuff = &Modbuff[2700];  
u8 *const pcRtRecordErrorCodeBuff = &Modbuff[2750];  

u8 *const pcErrorRecordBuff = &Modbuff[3000];   

//----------------------------------------------------------------------------------------- 实时数据
u16 *const ptEscRTBuff = 	(u16*)EscRTBuff;  

u8 *const pcInputBuff = 	&EscRTBuff[4];  
u8 *const pcCMDBuff = 		&EscRTBuff[20];  
u8 *const pcOutputBuff = 	&EscRTBuff[30];  
//u8 *const pcStateBuff = 	&EscRTBuff[60];  
u8 *const pcAlarmBuff = 	&EscRTBuff[64];  
u8 *const pcErrorBuff = 	&EscRTBuff[68];

//常数指针 const char* chrPtr
u32 *const plPowerOnTms = (u32*)&EscRTBuff[140];  

u8 *const pcPgaInputMask = &EscRTBuff[150];    
u8 *const pcPgaInputBuff = &EscRTBuff[170];  
u8 *const pcPgaOutputBuff = &EscRTBuff[190];  

//
u8 *const pcOMC_EscRTBuff = &McRxBuff[0];    

#define MBA_BASE				(u32)&Modbuff[0];  			

static u8 buff[300]; 

u8 DB_Comm_Addr=0;

/*************************************************************************************************** 
***************************************************************************************************/
////////////////////////////////////////////////////////////////////////////////
void modbus_format_new_encode(void)
{
	u8 *pt1,*pt2,*pt3,*pt4;
  u16 i=0;//, *pt = (u16*)Modbuff;
	
	pt1 = &Modbuff[1100];
	pt2 = ParaBuff;
	for(i=0;i<240;i++)
	{		
		*pt1 = *pt2;
		pt1++;
		pt2++;
	}	

	pt1 = &Modbuff[600];
	pt2 = EscRTBuff;
	pt3 = &Modbuff[800];
	pt4 = &McRxBuff[0];
	for(i=0;i<200;i++)
	{		
		*pt1 = *pt2;
		pt1++;
		pt2++;
		*pt3 = *pt4;
		pt3++;
		pt4++;
	}	
}

////////////////////////////////////////////////////////////////////////////////
void modbus_format_old_encode(void)
{
  u16 i=0; //, *pt = (u16*)Modbuff;
          
  u32 ulTemp1=0;
  u32 *pls;

  //点表HEAD CODE
  Modbuff[0] = Modbuff[200];
  Modbuff[1] = Modbuff[201];
  Modbuff[2] = Modbuff[202];
  Modbuff[3] = Modbuff[203];
  
  //
  //Modbuff[351] = EscRTBuff[134];     
  //Modbuff[352] = EscRTBuff[134];     
  //Modbuff[353] = EscRTBuff[134];     
  //Modbuff[411] = EscRTBuff[134];     
  

  Modbuff[400] = EscRTBuff[134];     
  Modbuff[401] = EscRTBuff[135];  

  Modbuff[402] = pcOMC_EscRTBuff[134];     
  Modbuff[403] = pcOMC_EscRTBuff[135];     
  
  //运行时间  
  pls = (u32*)&EscRTBuff[140];
	ulTemp1 = pls[0]/100;
	Modbuff[808] = ulTemp1%60;
  ulTemp1 = ulTemp1/60; 
	Modbuff[881] = ulTemp1>>8; 
	Modbuff[880] = ulTemp1;
	
	pls = (u32*)&pcOMC_EscRTBuff[140];    
	ulTemp1 = pls[0]/100;
	Modbuff[904] = ulTemp1%60; 
  ulTemp1 = ulTemp1/60; 
	Modbuff[981] = ulTemp1>>8; 
	Modbuff[980] = ulTemp1; 
		
  //输入信号
  for(i=0;i<16;i++)
  {
    Modbuff[604+i] = EscRTBuff[4+i];
    Modbuff[704+i] = pcOMC_EscRTBuff[4+i];
  }		
  
  //控制信号，继电器输出，电机速度，扶手带比值
  for(i=0;i<28;i++)
  {
    Modbuff[620+i] = EscRTBuff[20+i];
    Modbuff[720+i] = pcOMC_EscRTBuff[20+i];
  }		

  //主轴频率
  for(i=0;i<4;i++)
  {
    Modbuff[648+i] = EscRTBuff[52+i];
    Modbuff[748+i] = pcOMC_EscRTBuff[52+i];
  }		
  
  //扶梯状态，报警列表，故障列表
  for(i=0;i<40;i++)
  {
    Modbuff[660+i] = EscRTBuff[60+i];
    Modbuff[760+i] = pcOMC_EscRTBuff[60+i];
  }		
        
  //自动距离
  for(i=0;i<4;i++)
  {
    Modbuff[866+i] = EscRTBuff[48+i];
    Modbuff[966+i] = pcOMC_EscRTBuff[48+i];
  }		
  
  //接触器组合，运行状态        
  Modbuff[896] = EscRTBuff[148];
  Modbuff[897] = EscRTBuff[149];
  Modbuff[996] = pcOMC_EscRTBuff[148];
  Modbuff[997] = pcOMC_EscRTBuff[149];
                          
  //-------------------------------------------- 非对称
  
  //运行时间，指示快慢速状态
  Modbuff[906] = Modbuff[2928];
  Modbuff[907] = Modbuff[2929];
  
  //可配置输入数组
  for(i=0;i<40;i++)
  {      
    Modbuff[910+i] = EscRTBuff[150+i];
  }
            
	Modbuff[411] = EscRTBuff[0];
	Modbuff[351] = pcOMC_EscRTBuff[0];
	Modbuff[352] = pcOMC_EscRTBuff[132];
	Modbuff[353] = pcOMC_EscRTBuff[133];
}

////////////////////////////////////////////////////////////////////////////////
void modbus_format_encode(void)
{
  u16 *pt = (u16*)Modbuff; // i=0, 
  
  //安全回路端子号
  //Modbuff[408] 
  Modbuff[409] = pcOMC_EscRTBuff[131]; 
  
  //系统故障F81
  Modbuff[444] = EscRTBuff[104]|pcOMC_EscRTBuff[104]; 
  Modbuff[445] = EscRTBuff[105]|pcOMC_EscRTBuff[105]; 
  
//  pt[210] = EEP_Struct_BasePara.crc;
//  pt[211] = EEP_Struct_SysPara.crc;
//  pt[212] = EEP_Struct_ComPara.crc;
//  pt[213] = EEP_Struct_MspPara.crc;
//	//pt[214]  
//	//pt[215] 
//  pt[216] = EEP_Struct_StatBuff.crc;
//  pt[217] = EEP_Struct_RtBuff.crc;
}

/*************************************************************************************************** 
***************************************************************************************************/
u16 modbus_slave_03(void)
{
	u8 *MBApt = (u8*)MBA_BASE;

	u16 i=0,start_adr=0,dlen=0,tlen=0;
	
  modbus_format_encode();
  
	if((DB_Comm_Addr==1) || (DB_Comm_Addr==2))
	{		
		modbus_format_old_encode();
	}
	else 
	{
		modbus_format_new_encode();
	}		
	
	start_adr = ((buff[2]<<8)|buff[3])<<1;
	dlen = ((buff[4]<<8)|buff[5])<<1;
	i = start_adr + dlen; 	
	if((dlen>256) || (dlen<1) || (i>5400))
	{
		buff[2] |= 0x80;	
		tlen = 5;		
	}	
	else
	{
		buff[2] = dlen;
		
		MBApt += (u32)start_adr; 
/*		
    if(start_adr>=600 && start_adr<900)
    {
      buff[2] = dlen;
    }  
*/    
		for(i=0;i<dlen;i++)
		{
			buff[3+i] = *MBApt;
			MBApt++;
		}
		
		tlen = dlen+5;
	}		

	return(tlen);
}

u16 modbus_slave_16(void)
{
	u8 *MBApt = (u8*)MBA_BASE;
  u8 err=0;
	u16 i=0;
  u16 start_adr=0,data_len=0,tlen=0,end_adr=0;
	
	start_adr = ((buff[2]<<8)|buff[3])<<1;
	data_len = ((buff[4]<<8)|buff[5])<<1;
	end_adr = start_adr + data_len-1;  	    
		
	//系统时间		
	if(start_adr>=50 && end_adr<56)
	{
//	  CMD_FLAG10 |= 0x40;	 	
	}		
	//基本信息
	else if(start_adr>=1000 && end_adr<1100)
	{
//		EEP_Struct_BasePara.flag |= 0xf1;   
	}	
	//系统参数
	else if(start_adr>=1100 && end_adr<1340)
	{  
    //V3.0之前设置受限
    if(DB_Comm_Addr==1)
    {
      if((start_adr>=1150) && (end_adr<1159)) //|| ((start_adr==1100)&&(end_adr==1339))
      {
         err = 1;
      }  
      else
      {
//        EEP_Struct_SysPara.flag |= 0xf1; 
      }  
    }  
    else
    {  
//      EEP_Struct_SysPara.flag |= 0xf1;          
    }  
	}	
	//通信点表
	else if(start_adr>=1350 && end_adr<1592)
	{
//    EEP_Struct_ComPara.flag |= 0xf1;         
	}	
	//多段速
	else if(start_adr>=1600 && end_adr<1650)
	{
//		EEP_Struct_MspPara.flag |= 0xf1; 		
	}	 
  else if(start_adr>=1700 && end_adr<1952)
  {  
//    EEP_Struct_ErrRstPara.flag |= 0xf1; 	
	}
  else if((start_adr==5602) && (data_len==2) && (buff[7]==0xfa) && (buff[8]==0x01)) 
  {
//    CMD_FLAG10 |= 0x80; //故障复位
  }
/*  
	else if((start_adr==5604) && (data_len==2) && (buff[7]==0xaa) && (buff[8]==0x55) && (POWER_ON_TMS<500)) 
  {
    CMD_FLAG10 |= 0x01; //测试模式,上电5s内可以置位
  }
*/  
  else if((start_adr==5606) && (data_len==2) && (buff[7]==0xa5))
  {
    //启停控制指令 note
    if(buff[8] == 0x01)	//停梯
    {
      //REMOTE_COMM_CMD |= REMOTE_COMM_CMD_STOP;
    }
    else if(buff[8] == 0x02) //上行
    {
      //REMOTE_COMM_CMD |= REMOTE_COMM_CMD_UP;		
    }
    else if(buff[8] == 0x04) //下行
    {
      //REMOTE_COMM_CMD |=	REMOTE_COMM_CMD_DOWN;	
    }
  }    
  else if((start_adr==5608) && (data_len==2) && (buff[7]==0xa6) && (buff[8]<=8))
  {
    //REMOTE_COMM_MULTI_SPEED = ptRxBuff[8];
    //EEPROM_WR_FLAG |= EEP_WR_RT_DATA_ARRAY;
  }      

	if((err) || (data_len>256) || (data_len<1) || (end_adr>6000) || (data_len!=buff[6]))
	{
		buff[2] |= 0x80;	
		tlen = 5;		
	}	
	else 
	{
    if((end_adr<3000))
    {    
      MBApt += (u32)start_adr;
		
      for(i=0;i<data_len;i++)
      {
        *MBApt = buff[7+i];
        MBApt++;
      }
		}

		tlen = 8;
	}		
	
	return(tlen);
	//
}

/******************************************************************************* 
*******************************************************************************/
void Comm_DisplayBoard(void)
{
#ifdef GEC_SF_MASTER 
	u16 i=0,len=0,tlen=0;

	len = BSP_USART_Receive(USART3,buff,0);
	
  if(len<5)
  {
    
  }  
	else if( ((buff[0]==1)||(buff[0]==2)) && (!MB_CRC16(buff, len)) )   	
	{
		DB_Comm_Addr = buff[0];
		switch(buff[1])                
		{
			case 3:                     
				tlen = modbus_slave_03();                       
				break; 
			case 16:	
				tlen = modbus_slave_16();
				break;                    
		}	
		
		if(tlen)	
		{			
			i = MB_CRC16(buff, tlen-2);
			buff[tlen-1] = i>>8;
			buff[tlen-2] = i;
			BSP_USART_Send(USART3,buff,tlen);
		}			

	}		
  //新格式
  else if( (buff[0]==0x11) && (!MB_CRC16(buff, len)) )
  {
    
  }  
#endif	
}

/******************************************************************************* 
*******************************************************************************/
