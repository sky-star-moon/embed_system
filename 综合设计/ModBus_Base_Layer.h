#ifndef _LINK_LAYER_
#define _LINK_LAYER_
#include "STC15F2K60S2.H"
#include "Data.h"
#define TIMEOUT_OFF    AUXR&=0xEF
#define TIMEOUT_ON     T2H=0xfa;T2L=0x9a;AUXR|=0x10
#define Focs      11059200L
//#include "Instruct_Handle.h"
uchar Request[MAX_L]={0,0,0};
uchar Req=0;

uchar Respond[2*MAX_L]={0,0,0,0,0};
uchar Respond_length;
uchar Res=0;
uchar len;

bit R1_T0=1;
bit Busy=0;
bit packet_flag=0;
uchar Transmit=0;

extern uint wait;
void Timer2_Init(){//超时定时器，用来分辨不同帧
	T2H   = 0xfa;
	T2L   = 0x9a;
	AUXR |= 0x10;
	IE2  |= 0x04;
	EA    =1;
	TIMEOUT_OFF;//1.5ms Timer Off
}
void Timer2() interrupt 12	//如果超时，则说明一帧接收完毕
{
	TIMEOUT_OFF;
	len=Req;
	Req=0;
	packet_flag=1;//表明接收到一个数据帧
	//set the pack_flag, 
	//so that App_layer can know data_packet is OK
	if(R1_T0)
		R1_T0=0;//进入等待发送状态
}
void Uart1_Init(uint baudrate){//串口初始化
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
void Uart1_Process() interrupt 4//串口中断
{
	if(RI){
		RI=0;
		if(R1_T0==1){
			Request[Req++]=SBUF;//如果处于监听状态，则开始接收
			TIMEOUT_OFF;//关闭超时定时器
			TIMEOUT_ON;//重启超时定时器
			//意味着每个字节都重启定时器，如果两个字节之间超过1.5ms，则进入定时中断，将数据帧发送给Mid层
		}
	}
	if(TI){
		TI=0;
		if(R1_T0==0){//处于可发送状态
			if(Res==Respond_length) //发送完毕，进入监听状态，将wait设为8，wait用途请查看本模块第78行
				{R1_T0=1;Res=0;Transmit=0;Busy=0;wait=8;}
				
				else SBUF=Respond[Res++];//响应报文为发送完毕，继续发送
		}
	}
}
void ModBus_send(){
	if(Transmit&&!Busy&&!wait) {//如果数据帧完成，则先发送第一个字节
		SBUF=Respond[Res++];
		Busy=1;
	}
}
void ModBus_INIT(){
	Timer2_Init();
	Uart1_Init(9600);
}
#endif
