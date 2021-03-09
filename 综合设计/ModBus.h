#ifndef _MODBUS_
#define _MODBUS_
#include "STC15F2K60S2.H"
#include "Data.h"
//#include "Uart.h"
#define Focs 11059200L
#define MAX_L 10
bit Key1_status;
bit Key2_status;
bit Key3_status;
bit Beep_switch;
bit L0;
bit pack_flag=0;
bit handle_OK;
bit Busy;
uchar C_4ms=0;
extern bit flag_500us;
extern uint player1,player2;
#define TIMEOUT_OFF    AUXR&=0xEF
#define TIMEOUT_ON     T2H=0xfa;T2L=0x9a;AUXR|=0x10
#define DIFS_ON        T2H=0xf1;T2L=0x9a;AUXR|=0x10
#define DIFS_OFF       AUXR&=0xEF

static struct{
	uint ID;
	uint Temperature;
	uint Light;
	uint Key1;
	uint Key2;
	uint Key3;
}Regs;

static struct{
	uchar Request[MAX_L];
	uchar Req;
	uchar len;
	uchar IR[MAX_L];
	uchar Respond[MAX_L];
	char Res;
	uchar OK;
	uchar i;
}MB;
/*void Uart1_Init(uint baudrate){
	SCON |= 0x50;
	AUXR = 0X80;  
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
}
void Uart1_Process() interrupt 4
{
	if(RI){
		MB.Request[MB.Req++]=SBUF;
		RI=0;
		TIMEOUT_OFF;
		TIMEOUT_ON;
	}
	if(TI) {
		TI=0;
		Busy=0;
	}
}*/
void Timer2_Init(){
	T2H   =0xfa;
	T2L   =0x9a;
	AUXR |= 0x10;
	IE2  |=0x04;
	EA    =1;
	TIMEOUT_OFF;
}
void Timer2() interrupt 12
{
	MB.len=MB.Req;
	player2=MB.len;
	TIMEOUT_OFF;
	MB.Req=0;
	pack_flag=1;
}
void Behave();
void IR_handle(){
	if(pack_flag&&MB.Request[0]==0x01){
		for(MB.i=1;MB.i<MB.len;MB.i++){
			MB.IR[MB.i-1]=MB.Request[MB.i];
		}
		Behave();
	}
}
void Behave(){
	if(MB.IR[0]==0x01){
		MB.Respond[0]=0x00;
		MB.Respond[1]=0x01;
		MB.Respond[2]=0x02;
		MB.Respond[3]=0x03;
		MB.Respond[4]=0x04;
		MB.Respond[5]=0x0f;
		MB.Res       =5;
		pack_flag=0;
		handle_OK=1;
	}
}

void ModBus_send(){
	player1=handle_OK;
	if(C_4ms) { C_4ms--;return;}
	if(!handle_OK&&TI) return;
	
	if(MB.Res==0) {handle_OK=0;C_4ms=0xff;return;}
	if(Busy==1) return ;
	//=MB.Res+player1;
	SBUF=MB.Respond[MB.Res--];
	Busy=1;
	
	
	
	
	
	
}
void ModBus_Init(){
	Uart1_Init(9600);
	Timer2_Init();
	MB.OK=0;
	handle_OK=0;
	player1=0;
}
#endif