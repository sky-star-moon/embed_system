/*
set_pose(char,char)		��ʾ��Χ
Light_ON 							����ˮ��
LIGHT_OFF 						�ر���ˮ��
Segment_ON 						�������
Segment_OFF 					�ر������
print_LED(seven char) �������ʾ
print_Light(one char) ��ˮ����ʾ
set_LED_char(pose,char)�Զ�����ʾ
Display()             ������ɨ�������
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
		if(led.SEGMENT_ON){//��������ʹ�ܣ�����ʾ
			P0 = 0;
			sel_led = 0;
			P2 = (P2&0xf0) | (led.now_pose&0x0f);
			P0 = (led.display[led.now_pose]==0x7f)?led.LED_Char[led.now_pose]:(LED_Data[led.display[led.now_pose]]);
			//�����ʱ��Ҫ��ʾ�Զ����ַ�ʱ�����ַ�Ϊ0x7f��Ȼ��P0�����Ƹ��Զ���λѡ�ַ���
		}
		++led.now_pose;
	}
	else if(led.now_pose == led.end+1){
		if( led.LIGHT_ON ){//�����ˮ��ʹ�ܣ������ʾ
			P0 = 0;
			sel_led = 1;
			P0 = led.water;
		}
		led.now_pose=led.start;
	}
}

#endif