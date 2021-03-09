#ifndef _ADC_
#define _ADC_
#include "STC15F2K60S2.H"
#include "Data.h"
#define LIGHT 0x00
#define TEMP  0x01
int code tempdata[]={
		51, 51, 50, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 
		43, 43, 42, 42, 41, 41, 41, 40, 40, 39, 39, 38, 38, 38, 37, 37, 
		36, 36, 36, 35, 35, 34, 34, 34, 33, 33, 32, 32, 32, 31, 31, 31, 
		30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25,
		24, 24, 24, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 
		19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 
		13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 
		 7, 7, 6, 6,5, 5, 54,4, 3, 3,3, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1, 
		-2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, 
		-10, -10, -11, -11, -12, -13, -13, -14, -14, -15, -16, -16, -17
};//温度转换
struct{
	uint ADC_Num[8];
	void (*ADC_Init[8])(void);//初始化函数指针数组
	uchar ADC_ON_FLAG[8];//ADC可用标志
	uint  ADC_Count[8];//ADC采集了多少次数据
	ulint ADC_Sum[8];//采集的数据之和，结合ADC_Count得到平均值
	uchar ADCs,ADC_Current,ADC_Live;
	//	目前ADC注册数量，当前运行的ADC号，活跃ADC的数量

}adc; 
void ADC_ON(char which_ADC){
	if(adc.ADC_ON_FLAG[which_ADC] == 0){
		adc.ADC_ON_FLAG[which_ADC] = 1;
		adc.ADC_Live++;//活跃的ADC数量-1
	}
}
void ADC_OFF(char which_ADC){
	if(adc.ADC_ON_FLAG[which_ADC] == 1){
		adc.ADC_ON_FLAG[which_ADC] = 0;
		adc.ADC_Live--;//活跃的ADC数量+1
	}
}	
void ADC_Guard(){//ADC哨兵，防止没有活跃的ADC导致的89行的死循环
	ADC_CONTR&=0x6f;
	EADC=0;
}
void Temp_Init(){//温度初始化函数
	P1ASF = 0x1f;               
  ADC_RES = 0;               
  ADC_RESL = 0;             
  ADC_CONTR = 0x8b;       
  CLK_DIV = 0x20;          
}
void Light_Init(){//光强初始化函数
	P1ASF=0x1f;
	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=0x8c;				
	CLK_DIV=0x20;
}
void ADC_Func(){//注册两个ADC
	adc.ADC_Init[0]=Light_Init;
	adc.ADC_Init[1]=Temp_Init;
	adc.ADCs=2;
	adc.ADC_Live=2;
	adc.ADC_ON_FLAG[0] = 1;
	adc.ADC_ON_FLAG[1] = 1;
	adc.ADC_Current=0;
}
void ADC_Driver(){
	//EA=0;
	EADC=0;
	adc.ADC_Sum[adc.ADC_Current]+=(ADC_RES*256+ADC_RESL);//当前ADC测量值，和放在sum中
	adc.ADC_Count[adc.ADC_Current]++;//当前ADC测量次数+1
	if(adc.ADC_Count[adc.ADC_Current]==5000){//当前ADC测量5000次，取平均值
		adc.ADC_Num[adc.ADC_Current]=adc.ADC_Sum[adc.ADC_Current]/5000;
		adc.ADC_Sum[adc.ADC_Current]=0;
		adc.ADC_Count[adc.ADC_Current]=0;
	}
	EADC=1;

	ADC_CONTR&=0xEF;  					 
	ADC_CONTR|=0X08;	 					 
	
}
void ADC_Continue(){//选择下一个ADC，并进行初始化
	adc.ADC_Current++;
	adc.ADC_Current%=adc.ADCs;
	if(!adc.ADC_Live){ADC_Guard();return;}//哨兵，如果没有ADC，则运行哨兵程序
	while(adc.ADC_ON_FLAG[adc.ADC_Current]==0){
		adc.ADC_Current++;
		adc.ADC_Current%=adc.ADCs;
	}
	adc.ADC_Init[adc.ADC_Current]();
	EA=1;
	EADC=1;
}
void ADC_Interrupt() interrupt 5
{
	ADC_Driver();
}
#endif