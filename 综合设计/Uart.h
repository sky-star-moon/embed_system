#ifndef _UART_
#define _UART_
#include "Data.h"
#define NONE_PARITY 0
#define ODD_PARITY  1
#define EVEN_PARITY 2
#define MARK_PARITY 3
#define SPACE_PARITY 4
#define Focs      11059200L
static struct{
	uchar Rx_buf[10];
	uchar Tx_buf[10];
	uchar R_head,R_tail;
	uchar T_head,T_tail;
	uchar rx;
}uart; 
void Uart1_Init(uint baudrate){
	SCON |= 0x50;
	//AUXR = 0X80;  
  SCON |= 0X50; 
  TL1 = ( 65536 - ( Focs / 4 / baudrate ) );
  TH1 = ( 65536 - ( Focs / 4 / baudrate ) ) >> 8;
  AUXR |= 0X40; 
  RI = 0;      
  TI = 0;      
  TR1 = 1;     
  ES = 1;      
  EA = 1;      
  PS = 1;   
	uart.T_head=0;uart.T_tail=0;
	uart.R_head=0;uart.R_tail=0;	
}


void Uart1_Handle(){
	if(RI){
		RI=0;
		if((uart.R_tail+1)%10 == uart.R_head)
			return;
		uart.Rx_buf[uart.R_tail++] = SBUF;
		uart.R_tail=uart.R_tail%10;
	}
	if(TI){
		TI=0;
	}
}

void Uart1_Sent(uchar dat){
	if((uart.T_tail+1)%10 == uart.T_head)
			return;
	uart.Tx_buf[uart.T_tail++] = dat;
	uart.T_tail=uart.T_tail%10;
}
uchar Uart1_Receive(){
	if(uart.R_head!=uart.R_tail){
		uart.rx = uart.Rx_buf[uart.R_head++];
		uart.R_head=uart.R_head%10;
		return uart.rx;
	}
	else return 0xff;
}

void Uart1_Process() interrupt 4
{
	Uart1_Handle();
}
void Uart1_Driver(){
	if(TI)	return;
	if(uart.T_head!=uart.T_tail){
		SBUF = uart.Tx_buf[uart.T_head++];
		uart.T_head=uart.T_head%10;
	}
	else return;
}

#endif