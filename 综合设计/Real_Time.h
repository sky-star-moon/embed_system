#ifndef _REAL_TIME_
#define _REAL_TIME_
#include "intrins.h" //ͷ�ļ�
#include "Data.h"
#include <STC15F2K60S2.H>
#define DS1302_second_write  0X80
#define DS1302_minutes_write  0X82
#define DS1302_hour_write  0X84

#define DS1302_second_read  0X81
#define DS1302_minutes_read  0X83
#define DS1302_hour_read  0X85

sbit RTC_sclk=P1^5;//ʱ�ӿ������ţ��������ݵ��������
sbit RTC_rst=P1^6;//CE���ţ���дʱ�����øߵ�ƽ
sbit RTC_io=P5^4;//��������
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
  RTC_sclk=0;//��ʱ���ź�ѹ��
	_nop_();
	_nop_();
	for(i=0;i<8;i++)//��ʼ����8λ����
	{
		RTC_io=dat&0x01;//�������͵�RTC_io �˿�
		_nop_();
	  _nop_();
		RTC_sclk=1;//ʱ�������ߣ����������أ����ݱ����� 
		_nop_();
	  _nop_();
		RTC_sclk=0;//ʱ�������ͣ�Ϊ��һ����������׼��
		dat>>=1;//��������һλ��׼��������һλ����
	}
}
uchar DS1302ReadByte()
{
	uchar i,dat;
		_nop_();
	  _nop_();
	for(i=0;i<8;i++)
	{
		dat>>=1;//Ҫ���ص���������һλ
		if(RTC_io==1)//��������Ϊ��ʱ��֤����λ����Ϊ1
		dat|=0x80;
		RTC_sclk=1;
		_nop_();
	  _nop_();
		RTC_sclk=0;//�����½���
		_nop_();
	  _nop_();
	}
	return dat;//���ض�ȡ��������
}
uchar DS1302Read(uchar cmd)
{
	uchar dat;
	RTC_rst=0;//��ʼCE��0
	RTC_sclk=0;//��ʼʱ����0
	RTC_rst=1;//��ʼCE��1�����俪ʼ
	DS1302WriteByte(cmd);//���������֣�
	dat=DS1302ReadByte();//��ȡ�õ���ʱ��
	RTC_sclk=1;//ʱ��������
	RTC_rst=0;//��ȡ������CE��0���������ݴ���
	return dat;//���صõ���ʱ������
}
void DS1302Write(uchar cmd,uchar dat)
{
	RTC_rst=0;//��ʼCE��0
	RTC_sclk=0;//��ʼʱ����0
	RTC_rst=1;//��1�����俪ʼ
	DS1302WriteByte(cmd);//���������֣�Ҫд���ʱ��ĵ�ַ
	DS1302WriteByte(dat);//д���޸ĵ�ʱ��
	RTC_sclk=1;//ʱ��������
	RTC_rst=0;//��ȡ������CE=0���������ݵĴ���
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
