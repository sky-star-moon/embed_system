/*
�ù��̶�STC��������һ����Դ�����˷�װ��ʵ�����û����������ķ���

���������ϴ�ѧSTC�����壬�������µ�Ч����
��ˮ�ƴ��������������𣬵�ȫ����˸�󣬷�������һ�����Զ����밴��������Ϸ
KEY1��KEY3�İ���������ʵʱ�������������ʾ��
������ˮ�ƴ��ҵ���ÿһ�붼�������ƶ�һ��
����ˮ�Ƶ����㵽����ߵ�ʱ����Ϸ����

��Ϸ�������ȷָߵĽ���˸��ʾ��ƽ����ͬʱ��˸

*/
#include "STC15F2K60S2.H"
#include "LED_segment.h"
#include "KEY.h"
#include "Beep.h"
#define uchar unsigned char 
#define uint unsigned int 
bit flag_500us=0;
bit flag_1ms=0;
bit flag_10ms=0;
bit flag_100ms=0;
bit flag_1s=0;
uchar count_500us=0;
uchar count_1ms=0;
uchar count_10ms=0;
uchar count_100ms=0;
uchar count_1s=0;
uchar WATER=0x01;
uchar number=0;
uchar player1=0,player2=0;
uchar RL_TH0=(65536-1000)/256;
uchar RL_TL0=(65536-1000)%256;
char win=0;
char Dis[8];
uchar win_pos=0;
uchar win_begin=0;
uchar win_end=3;
uchar win_count=0;
uint rand;
void win_display(){
	P0=0;
	P23=0;
	if(win_pos==8)win_pos=0;
	P2=win_pos;
	if(win_pos>=win_begin && win_pos<=win_end){
		//win_begin��win_end��ʤ�ߵ÷����ڵ������
		//win_begin��win_end����Ϸ����ʱ������
		win_count++;
		if(win_count<=100){//��˸��ʾʤ����Ϣ
			P0=Dis[win_pos]==0x7f?0x40:LED_Data[Dis[win_pos]];
		}
		else 
			P0=0;
		if(win_count>=200) win_count=0;
	}
	else{
		P0=Dis[win_pos]==0x7f?0x40:LED_Data[Dis[win_pos]];
	}
	win_pos++;
}

void get_number(){//��������ܵ���ʾ
	Dis[0]=player2/100;
	Dis[1]=(player2/10)%10;
	Dis[2]=player2%10;
	Dis[3]=0x7f;
	Dis[4]=0x7f;
	Dis[5]=player1/100;
	Dis[6]=(player1/10)%10;
	Dis[7]=player1%10;
}
void GPIO_Init(void)				                                                    //Ӳ����ʼ��/BootLoad/BIOS
{  	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
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
void Func_1ms(){
	flag_1ms=0;
	count_1ms++;						   
	if( count_1ms == 10 )   {
		flag_10ms = 1; 
		count_1ms = 0;  
	}
	if(win)
		win_display();
	Display();
	count_key();//��������
}
void Func_10ms(){
	flag_10ms=0;
	count_10ms++; 
	if( count_10ms == 10 )  {
		flag_100ms = 1; 
		count_10ms = 0;  
	}	
}
void Func_100ms(){
	flag_100ms=0;
	count_100ms++;
	if(count_100ms==10) {count_100ms=0;flag_1s=1;}
}
void Func_1s(){
	flag_1s=0;
	count_1s++;	
}
void game_rule(uchar num);
void main(){
	GPIO_Init();
	Timer0_Init();
	EA=1;
	set_pose(0,7);										//��0~7λ�������
	Light_ON();												//����ˮ��
	print_LED(0,0,0,0x7f,0x7f,0,0,0); //ȫ����ʾ0
	set_LED_char(3,0x40);    					//3��λ����ʾ���
	set_LED_char(4,0x40);              //4��λ����ʾ���
	print_Light(WATER);								//��ˮ����ʾ0x01
	segment_ON();											//���������
	count_1s=0;
	WATER=0x01;
	while(1){
		if(WATER==0xff) break; 
		if(flag_500us  ==1)	{Func_500us();}
		if(flag_1ms  ==1) {Func_1ms();   }
		if(flag_10ms ==1) {Func_10ms(); }
		if(flag_100ms==1) {Func_100ms();print_Light(WATER);WATER=(WATER<<1)|WATER;}
	  if(flag_1s   ==1)	{Func_1s();}
		
	}
	print_Light(0);//��ˮ�Ƴ�ʼ��
	key_init();//������ʼ��
	key_enable(1);//ʹ��key1
	key_enable(2);//ʹ��key2
	key_enable(3);//ʹ��key2
	set_Beep(1500,1);//������1500hz����1s
	Beep_ON();//���������������̿�ʼ��
	count_1s=0;//ʱ���ʱ
	WATER=0x01;//��ˮ�ƣ���ˮ�ƿ�������һ��ģʽ�Ѿ�����
	segment_ON();//���������
	print_Light(WATER);//��ˮ����ʾ
	WATER<<=1;
	
	
	while(1){
		if(flag_500us  ==1){Func_500us();}
		if(flag_1ms  ==1) {Func_1ms();   }
		if(flag_10ms ==1) {Func_10ms();   }
		if(flag_100ms==1) {Func_100ms(); }
	  if(flag_1s   ==1)	{Func_1s();print_Light(WATER);WATER<<=1;}
			//ÿ1s����ˮ������һλ
		number=key_push();//��ȡ������Ϣ
		if(number){game_rule(number);}
		//���������£���Ӧѡ�ֱȷ�+1
		if(count_1s==8) break;//8s����������
	}
	segment_OFF();
	Light_OFF();
	while(1){
		win=1;
		if(flag_500us  ==1){Func_500us();}
		if(flag_1ms  ==1) {Func_1ms();   }
		if(flag_10ms ==1) {Func_10ms();  }
		if(flag_100ms==1) {Func_100ms(); }
	  if(flag_1s   ==1)	{Func_1s();print_Light(WATER);WATER<<=1;}
		if(player1<player2){
			win_begin=0;
			win_end=2;
		}
		else if(player1>player2){
			win_begin=5;
			win_end=7;
		}
		else{
			win_begin=0;
			win_end=7;
		}
	}
}
void game_rule(uchar num){

	switch (num){
		case 1:player1++;break;
		case 3:player2++;break;
	}
	get_number();
	print_LED(Dis[0],Dis[1],Dis[2],0x7f,0x7f,Dis[5],Dis[6],Dis[7]);
}

