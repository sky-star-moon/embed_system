/*
该工程对STC开发板上一下资源进行了封装，实现了用户层和驱动层的分离

下载至湖南大学STC开发板，会有如下的效果：
流水灯从右至左依次亮起，当全部闪烁后，蜂鸣器鸣一声，自动进入按键比赛游戏
KEY1和KEY3的按键次数将实时的在数码管上显示，
并且流水灯从右到左，每一秒都将向左移动一下
当流水灯的亮点到最左边的时候，游戏结束

游戏结束，比分高的将闪烁显示，平分则同时闪烁

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
		//win_begin和win_end是胜者得分所在的数码管
		//win_begin和win_end在游戏结束时被设置
		win_count++;
		if(win_count<=100){//闪烁显示胜者信息
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

void get_number(){//更新数码管的显示
	Dis[0]=player2/100;
	Dis[1]=(player2/10)%10;
	Dis[2]=player2%10;
	Dis[3]=0x7f;
	Dis[4]=0x7f;
	Dis[5]=player1/100;
	Dis[6]=(player1/10)%10;
	Dis[7]=player1%10;
}
void GPIO_Init(void)				                                                    //硬件初始化/BootLoad/BIOS
{  	P0M1=0x00;						//设置P0为推挽模式，点亮数码管
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//将P2^3设置为推挽模式，其余为准双向口模式
}
void Timer0_Init(void)	  //1mS@12.000MHz
{	AUXR &= 0x7f;		  
	TMOD &= 0xf0;		  //使用定时器0，16位重装载模式
	TH0=(65535-500)/256; //高8位赋初值
	TL0=(65535-500)%256; //低8位赋初值
	TR0=1;				  //启动定时器1	
	ET0=1;                //开启定时器0中断
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
	Voice();//蜂鸣器驱动
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
	count_key();//按键驱动
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
	set_pose(0,7);										//让0~7位数码管亮
	Light_ON();												//打开流水灯
	print_LED(0,0,0,0x7f,0x7f,0,0,0); //全部显示0
	set_LED_char(3,0x40);    					//3号位置显示横杠
	set_LED_char(4,0x40);              //4号位置显示横杠
	print_Light(WATER);								//流水灯显示0x01
	segment_ON();											//开启数码管
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
	print_Light(0);//流水灯初始化
	key_init();//按键初始化
	key_enable(1);//使能key1
	key_enable(2);//使能key2
	key_enable(3);//使能key2
	set_Beep(1500,1);//蜂鸣器1500hz，响1s
	Beep_ON();//开启蜂鸣器，立刻开始响
	count_1s=0;//时间计时
	WATER=0x01;//流水灯，流水灯开关在上一个模式已经开启
	segment_ON();//开启数码管
	print_Light(WATER);//流水灯显示
	WATER<<=1;
	
	
	while(1){
		if(flag_500us  ==1){Func_500us();}
		if(flag_1ms  ==1) {Func_1ms();   }
		if(flag_10ms ==1) {Func_10ms();   }
		if(flag_100ms==1) {Func_100ms(); }
	  if(flag_1s   ==1)	{Func_1s();print_Light(WATER);WATER<<=1;}
			//每1s，流水灯左移一位
		number=key_push();//获取按键信息
		if(number){game_rule(number);}
		//按键被按下，对应选手比分+1
		if(count_1s==8) break;//8s，比赛结束
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

