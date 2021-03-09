#ifndef _BEEP_
#define _BEEP_
/*
Bp_ON()   					//打开蜂鸣器
Bp_OFF()					  //关闭蜂鸣器
set_Beep(int,char)  //设置蜂鸣器频率和时长
Voice()							//驱动，在500us时调用
*/
#define uchar unsigned char 
#define uint unsigned int 
static struct Bp{
	uint frequency;//Hz
	uchar time;//s
	uint voice_judge;
}bp;
static uint bp_count=0;
static uint time_count=0;
sbit beep = P3^4;
bit bp_ON=0;

void Beep_ON()	{bp_ON=1;}//打开蜂鸣器
void Beep_OFF() {bp_ON=0;}//关闭蜂鸣器
char set_Beep(int _fre,char _time){
	if(_fre<800 || _fre>2000) return 0;
	//如果频率大于2000或者小于800，则报错，不支持以外的频率
	bp.frequency = _fre;//频率
	bp.time = _time;//蜂鸣器响的时长
	time_count=0;		
	bp_count=0;
	bp.voice_judge = 2000/_fre;
	//比如1000Hz，那么voice_judge=2，则2个500us电平反转
	return 1;							//以上为初始化蜂鸣器的参数
}
void Voice(){// just can be used in Func_500us 
	if(bp_ON==0 ) {beep=0;return;}//如果蜂鸣器关闭，则返回
	if(!bp.time) {beep=0;bp_ON=0;return;}
//如果时间到达，则将蜂鸣器置低电平，并关闭蜂鸣器
		if( ++time_count == 2000 ){
			bp.time--;
			time_count=0;
		}//2000个500us是1s，则将时长-1
		if( ++bp_count==bp.voice_judge ){
			bp_count = 0;
			beep =~ beep;
		}//电平反转
}

#endif