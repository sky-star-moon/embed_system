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
void Timer2_Init(){//��ʱ��ʱ���������ֱ治ͬ֡
	T2H   = 0xfa;
	T2L   = 0x9a;
	AUXR |= 0x10;
	IE2  |= 0x04;
	EA    =1;
	TIMEOUT_OFF;//1.5ms Timer Off
}
void Timer2() interrupt 12	//�����ʱ����˵��һ֡�������
{
	TIMEOUT_OFF;
	len=Req;
	Req=0;
	packet_flag=1;//�������յ�һ������֡
	//set the pack_flag, 
	//so that App_layer can know data_packet is OK
	if(R1_T0)
		R1_T0=0;//����ȴ�����״̬
}
void Uart1_Init(uint baudrate){//���ڳ�ʼ��
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
void Uart1_Process() interrupt 4//�����ж�
{
	if(RI){
		RI=0;
		if(R1_T0==1){
			Request[Req++]=SBUF;//������ڼ���״̬����ʼ����
			TIMEOUT_OFF;//�رճ�ʱ��ʱ��
			TIMEOUT_ON;//������ʱ��ʱ��
			//��ζ��ÿ���ֽڶ�������ʱ������������ֽ�֮�䳬��1.5ms������붨ʱ�жϣ�������֡���͸�Mid��
		}
	}
	if(TI){
		TI=0;
		if(R1_T0==0){//���ڿɷ���״̬
			if(Res==Respond_length) //������ϣ��������״̬����wait��Ϊ8��wait��;��鿴��ģ���78��
				{R1_T0=1;Res=0;Transmit=0;Busy=0;wait=8;}
				
				else SBUF=Respond[Res++];//��Ӧ����Ϊ������ϣ���������
		}
	}
}
void ModBus_send(){
	if(Transmit&&!Busy&&!wait) {//�������֡��ɣ����ȷ��͵�һ���ֽ�
		SBUF=Respond[Res++];
		Busy=1;
	}
}
void ModBus_INIT(){
	Timer2_Init();
	Uart1_Init(9600);
}
#endif
