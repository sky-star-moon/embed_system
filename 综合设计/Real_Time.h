#ifndef _REAL_TIME_
#define _REAL_TIME_
#include "intrins.h" //头文件
#include "Data.h"
#include <STC15F2K60S2.H>
#define DS1302_second_write  0X80
#define DS1302_minutes_write  0X82
#define DS1302_hour_write  0X84

#define DS1302_second_read  0X81
#define DS1302_minutes_read  0X83
#define DS1302_hour_read  0X85

sbit RTC_sclk=P1^5;//时钟控制引脚，控制数据的输入输出
sbit RTC_rst=P1^6;//CE引脚，读写时必须置高电平
sbit RTC_io=P5^4;//数据引脚
typedef struct _systemtime_
{
   uchar second;
	 uchar minute;
	 uchar hour;
}systemtime;
systemtime realtime;
uchar realtimei;
uchar realtimetemp;
void DS1302WriteByte(uchar dat)
{
	uchar i;
  RTC_sclk=0;//将时钟信号压低
	_nop_();
	_nop_();
	for(i=0;i<8;i++)//开始传输8位数据
	{
		RTC_io=dat&0x01;//将数据送到RTC_io 端口
		_nop_();
	  _nop_();
		RTC_sclk=1;//时钟线拉高，制造上升沿，数据被传输 
		_nop_();
	  _nop_();
		RTC_sclk=0;//时钟线拉低，为下一个上升沿做准备
		dat>>=1;//数据右移一位，准备传输下一位数据
	}
}
uchar DS1302ReadByte()
{
	uchar i,dat;
		_nop_();
	  _nop_();
	for(i=0;i<8;i++)
	{
		dat>>=1;//要返回的数据右移一位
		if(RTC_io==1)//当数据线为高时，证明该位数据为1
		dat|=0x80;
		RTC_sclk=1;
		_nop_();
	  _nop_();
		RTC_sclk=0;//制造下降沿
		_nop_();
	  _nop_();
	}
	return dat;//返回读取出的数据
}
uchar DS1302Read(uchar cmd)
{
	uchar dat;
	RTC_rst=0;//初始CE置0
	RTC_sclk=0;//初始时钟置0
	RTC_rst=1;//初始CE置1，传输开始
	DS1302WriteByte(cmd);//传输命令字，
	dat=DS1302ReadByte();//读取得到的时间
	RTC_sclk=1;//时钟线拉高
	RTC_rst=0;//读取结束，CE置0，结束数据传输
	return dat;//返回得到的时间日期
}
void DS1302Write(uchar cmd,uchar dat)
{
	RTC_rst=0;//初始CE置0
	RTC_sclk=0;//初始时钟置0
	RTC_rst=1;//置1，传输开始
	DS1302WriteByte(cmd);//传输命令字，要写入的时间的地址
	DS1302WriteByte(dat);//写入修改的时间
	RTC_sclk=1;//时钟线拉高
	RTC_rst=0;//读取结束，CE=0，结束数据的传输
}
systemtime GetDA1302()
{
	systemtime time;
	uchar realvalue;
	realvalue=DS1302Read(DS1302_second_read);
	time.second=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
	//76543210, 654 are tens and 3210 are units
		realvalue=DS1302Read(DS1302_minutes_read);
	time.minute=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
	//76543210, 654 are tens and 3210 are units
		realvalue=DS1302Read(DS1302_hour_read );
	time.hour=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
	return time;
}
void Init_DS1302()
{
	DS1302Write(0X8E,0X00);//close the write protect
	realtimei=DS1302Read(DS1302_second_read)&0x7f;//save the 0-6 bit_
	DS1302Write(DS1302_second_write,realtimei);//CH=0,start the clock
	DS1302Write(0X8E,0X80);//0x8e must be 1 if we want to write data_ to Ds1302
}
#endif
