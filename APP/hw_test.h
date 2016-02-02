#ifndef __HW_TEST_H
#define __HW_TEST_H	

void Hw_Test_Init(void);
void Hw_Test1(void);
void Hw_Test2(void);

//#define EXTERNAL_INPUT_TEST

//INPUT
#define IN1     PEin(1)
#define IN2     PEin(0)
#define IN3     PBin(7)
#define IN4     PBin(6)
#define IN5     PBin(5)
#define IN6     PBin(4)
#define IN7     PBin(3)
#define IN8     PDin(7)
#define IN9     PDin(6)
#define IN10     PDin(5)
#define IN11     PDin(4)
#define IN12     PDin(3)
#define IN13     PDin(2)
#define IN14     PCin(12)
#define IN15     PCin(11)
#define IN16     PCin(10)
#define IN17     PAin(15)
#define IN18     PAin(9)
#define IN19     PAin(8)
#define IN20     PCin(9)
#define IN21     PCin(8)
#define IN22     PCin(7)
#define IN23     PEin(9)
#define IN24     PEin(8)
#define IN25     PEin(7)
#define IN26     PBin(2)
#define IN27     PBin(1)
#define IN28     PBin(0)

#define EX_IN1     PCin(6)
#define EX_IN2     PDin(15)
#define EX_IN3     PDin(14)
#define EX_IN4     PDin(13)
#define EX_IN5     PDin(12)
#define EX_IN6     PDin(11)
#define EX_IN7     PDin(10)
#define EX_IN8     PDin(9)
#define EX_IN9     PDin(8)
#define EX_IN10     PBin(15)
#define EX_IN11     PBin(14)
#define EX_IN12     PEin(15)
#define EX_IN13     PEin(14)
#define EX_IN14     PEin(13)
#define EX_IN15     PEin(12)
#define EX_IN16     PEin(11)
#define EX_IN17     PEin(10)

//OUTPUT
#define AUX1_CTR     PCout(0)
#define SF_RL1_CTR     PEout(2)

#define SF_RL1_WDT     PBout(9)




#endif