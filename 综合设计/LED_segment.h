/*
set_pose(char,char)		显示范围
Light_ON 							打开流水灯
LIGHT_OFF 						关闭流水灯
Segment_ON 						打开数码管
Segment_OFF 					关闭数码管
print_LED(seven char) 数码管显示
print_Light(one char) 流水灯显示
set_LED_char(pose,char)自定义显示
Display()             驱动，扫描数码管
*/

#ifndef _LED_SEGMENT_
#define _LED_SEGMENT_
code char LED_Data[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,
										 0x7d,0x07,0x7f,0x6f,0x40,0x00};
#define sel_led P23
static struct LED{
	char start,end;
	char display[8];
	char water;
	char now_pose;
	char LED_Char[8];
	char LIGHT_ON;
	char SEGMENT_ON;
}led;

void set_pose(char _begin,char _end){
	led.start= _begin;
	led.end  = _end;
	led.now_pose = _begin;
}

void print_LED(char x0,char x1,char x2,char x3,char x4,char x5,char x6,char x7)
{
	led.display[0]=x0;
	led.display[1]=x1;
	led.display[2]=x2;
	led.display[3]=x3;
	led.display[4]=x4;
	led.display[5]=x5;
	led.display[6]=x6;
	led.display[7]=x7;
}
void set_LED(char _pose,char _light){
	led.display[_pose] = _light;
}
void set_LED_char(char _pose,char _char){
	led.display[_pose] = 0x7f;
	led.LED_Char[_pose]    = _char;
}
void Light_ON(){
	led.LIGHT_ON = 1;
}
void Light_OFF(){
	led.LIGHT_ON = 0;
}
void Segment_ON(){
	led.SEGMENT_ON = 1;
}
void Segment_OFF(){
	led.SEGMENT_ON = 0;
}

void print_Light(char _light){
	led.water = _light;
}

void Display(){
	if(led.now_pose <= led.end ) {
		if(led.SEGMENT_ON){//如果数码管使能，则显示
			P0 = 0;
			sel_led = 0;
			P2 = (P2&0xf0) | (led.now_pose&0x0f);
			P0 = (led.display[led.now_pose]==0x7f)?led.LED_Char[led.now_pose]:(LED_Data[led.display[led.now_pose]]);
			//该语句时当要显示自定义字符时，该字符为0x7f，然后P0被复制给自定义位选字符。
		}
		++led.now_pose;
	}
	else if(led.now_pose == led.end+1){
		if( led.LIGHT_ON ){//如果流水灯使能，则可显示
			P0 = 0;
			sel_led = 1;
			P0 = led.water;
		}
		led.now_pose=led.start;
	}
}

#endif