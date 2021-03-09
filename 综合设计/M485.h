#ifndef _M485_
#define _M485_
#include "Data.h"
#define uchar unsigned char
#define ulong unsigned long
#define MAX_L 10
#define cstUart2Ri  0x01                        //?????????
#define cstUart2Ti  0x02                        //?????????
extern uint player1,player2;
#define cstNoneParity           0           //???
#define PARITYBIT cstNoneParity         //?????
uchar ucGetDataTmp ;          //??????
uchar ucPutDataTmp ;          //??????
/*???????*/
#define cstFosc 11059200L               //?????? 
#define cstBaud2    9600                    //?????    
#define cstT2HL     (65536-(cstFosc/4/cstBaud2))    //???????
uchar ucGetDataTmp ;          //??????
uchar ucPutDataTmp ;          //??????
sbit sbtM485_TRN  = P3^7;
#define M485_Read sbtM485_TRN=0
#define M485_Write sbtM485_TRN=1

struct M485{
	uchar Rx_Data;
	uchar Rx_point;
	uchar Tx_Data;
	uchar Tx_point;
	uchar Busy;
}m485;
void Uart458_Init( void )
{
	M485_Read;
	P_SW2 |= 0x01;
  S2CON = 0x10 ;     
  T2L = cstT2HL ;     
  T2H = cstT2HL >> 8 ;
  AUXR |= 0x14 ;  
	IE2 |= 0x01;
	IP2 |= 0x01;
	EA=1;
	m485.Tx_point=0;
	m485.Rx_point=0;
	m485.Busy = 0;
}
void Uart2_Process( void ) interrupt 8 using 1
{
    if( S2CON & cstUart2Ri )
    {
      m485.Rx_Data = S2BUF;
      S2CON &= ~cstUart2Ri;
    }
    if( S2CON & cstUart2Ti )
    {          
      S2CON &= ~cstUart2Ti ; 
			M485_Read;
				player1=0xff;
			m485.Busy = 0;
    }
}
void M485_Send(uchar t_data){
	if(m485.Busy) return;
	m485.Tx_Data=t_data;

	m485.Busy=1;
}
void M485_Driver(){
	if(m485.Busy){
		M485_Write;
		S2BUF=m485.Tx_Data;
		
	}
}
#endif