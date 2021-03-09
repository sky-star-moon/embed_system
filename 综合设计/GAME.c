#include "STC15F2K60S2.H"
#include "LED_segment.h"
#include "KEY.h"
#include "ADC.h"
#include "Real_Time.h"
#include "EEPROM.h"
//#include "Uart.h"
//#include "M485.h"
#include "Beep.h"
#include "ModBus_Mid_Layer.h"
#define uchar unsigned char 
#define uint unsigned int
#define ID_Adder_High 0x00
#define ID_Adder_Low 0x01

#define Hour_Addr 0x02
#define Minute_Addr 0x03
bit flag_500us=0;
bit flag_1ms=0;
bit flag_10ms=0;
bit flag_100ms=0;
bit flag_1s=0;
uchar data_485=0;
char Dis[8];
uchar count_500us=0;
uchar count_1ms=0;
uchar count_10ms=0;
uchar count_100ms=0;
uchar count_1s=0;
uchar WATER=0x01;
uchar temp_K;
uint Light_Digit=0;
uint Temp_Digit=0;
uint player1=0;
uint player2=0;
uint player3=0;
bit p1,p2,p3;
uint wait;
uchar minute_save,hour_save;
void Data_fresh(){//ˢ�¼Ĵ�������Ȧ
	Regs[1]=Temp_Digit;
	Regs[2]=Light_Digit;
	Regs[3]=player1;
	Regs[4]=player2;
	Regs[5]=player3;
	Regs[6]=hour_save;
	Regs[7]=minute_save;
	Coil=p1?(Coil|0x01):(Coil&~0x01);
	Coil=p2?(Coil|0x02):(Coil&~0x02);
	Coil=p3?(Coil|0x04):(Coil&~0x04);
	Coil=bp_ON?(Coil|0x08):(Coil&~0x08);
}

void GPIO_Init(void)				                                                    //Ӳ����ʼ��/BootLoad/BIOS
{  	
	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
	P3M0 = 0x00 ;
  P3M1 = 0x00 ;
	P1M0 = 0x00 ;
  P1M1 = 0x00 ;       //P1?P3????
}
void Timer0_Init(void)	  //1mS@12.000MHz
{	AUXR &= 0x7f;		  
	TMOD &= 0xf0;		  //ʹ�ö�ʱ��0��16λ��װ��ģʽ
	TH0=(65535-500)/256; //��8λ����ֵ
	TL0=(65535-500)%256; //��8λ����ֵ
	TR0=1;				  //������ʱ��1	
	ET0=1;                //������ʱ��0�ж�
}

void timer0() interrupt 1
{
	flag_500us=1;
	if(wait){wait--;}	 
}
void Func_500us(){
	flag_500us=0;
	count_500us++;
	if(count_500us==2){
		flag_1ms=1;
		count_500us=0;
	}
	Voice();//����������
}
void Key_Count(){//��������
	switch (temp_K){
		case 1:player1=player1+1;p1=!p1;break;//������ͬʱ������Ȧ��p1����Ȧ�ȼۣ���ת
		case 2:player2=player2+1;p2=!p2;break;
		case 3:player3=player3+1;p3=!p3;break;
	}
}
void Func_1ms(){
	flag_1ms=0;
	count_1ms++;						   
	if( count_1ms == 10 )   {
		flag_10ms = 1; 
		count_1ms = 0;  
	}
	Data_fresh();//���¼Ĵ�������Ȧ
	Display();	//�����&LED������
	count_key();//��������
	ModBus_Instruct_Handle(Request);//Mid�����Ƿ���������
	temp_K = key_push();//����
		if(temp_K==1){
			write_add( Hour_Addr, realtime.hour );
		}
		else if(temp_K==2){
			write_add( Minute_Addr, realtime.second );
		}
		else if(temp_K==3){
			hour_save=read_add( Hour_Addr );
			_nop_();
			minute_save=read_add( Minute_Addr );
			_nop_();
		}
		Key_Count();
}
void Func_10ms(){
	flag_10ms=0;
	count_10ms++; 
	if( count_10ms == 10 )  {
		flag_100ms = 1; 
		count_10ms = 0;  
	}
	print_Light(p1 | p2*4 | p3*16 | bp_ON*64);
	ADC_Continue();
	Light_Digit=adc.ADC_Num[0];
	Temp_Digit=tempdata[(adc.ADC_Num[1]>>2)-65];
}
void get_number(){				//�������ʾʱ����
	Dis[0]=realtime.hour/10;
	Dis[1]=realtime.hour%10;
	Dis[2]=0x7f;
	Dis[3]=realtime.minute/10;
	Dis[4]=realtime.minute%10;
	Dis[5]=0x7f;
	Dis[6]=realtime.second/10;
	Dis[7]=realtime.second%10;
	print_LED(Dis[0],Dis[1],Dis[2],Dis[3],Dis[4],Dis[5],Dis[6],Dis[7]);
}

void Func_100ms(){
	flag_100ms=0;
	count_100ms++;
	if(count_100ms==10) {count_100ms=0;flag_1s=1;}
	realtime=GetDA1302();//��ȡʱ��
	get_number();////�������ʾʱ����
}


void Func_1s(){
	flag_1s=0;
	count_1s++;	
}
void main(){
	GPIO_Init();
	Timer0_Init();
	EA=1;
	IIC_init();												//��ʼ��IIC������ʧ�洢��
	set_pose(0,7);										//��0~7λ�������
	Light_ON();												//����ˮ��
	set_LED_char(2,0x40);    					//3��λ����ʾ���
	set_LED_char(5,0x40);              //4��λ����ʾ���
	print_Light(WATER);								//��ˮ����ʾ0x01
	Segment_ON();											//���������
	key_init();												//������ʼ��
	set_Beep(2000,0xff);							//������2000Hz��0xff��ʾһֱ����
	Beep_OFF();												//�رշ�����
	key_enable(1);										//ʹ�ܰ���1
	key_enable(2);										//ʹ�ܰ���2
	key_enable(3);										//ʹ�ܰ���3
	ModBus_INIT();											//��ʼ��ModBus
	ADC_Func();													//ע��ADC
	ADC_ON(LIGHT);											//
	ADC_ON(TEMP);												//
	Light_Init();												//��ʼ��Light
	//Regs[0]=read_add( ID_Addr );	
	if(DS1302Read(DS1302_second_read)&0X80)
		 Init_DS1302();										//��ʼDS1302����1302��CE��0������ʵʱʱ��
	Regs[0]=0;													
	Regs[0]=read_add( ID_Adder_High )*256;
	_nop_();
	Regs[0]=Regs[0]+read_add( ID_Adder_Low);//��ȡ�洢�ڷ���ʧ�е�ID
	_nop_();
	while(1){
		if(flag_500us  ==1)	{Func_500us();}
		if(flag_1ms  ==1) {Func_1ms();   }
		if(flag_10ms ==1) {Func_10ms();ModBus_send(); }//Base�����Ƿ�����Ӧ���ķ���
		if(flag_100ms==1) {Func_100ms();}
		if(flag_1s   ==1)	{Func_1s();}
	}
}


