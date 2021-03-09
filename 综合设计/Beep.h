#ifndef _BEEP_
#define _BEEP_
/*
Bp_ON()   					//�򿪷�����
Bp_OFF()					  //�رշ�����
set_Beep(int,char)  //���÷�����Ƶ�ʺ�ʱ��

Voice()							//��������500usʱ����
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

void Beep_ON()	{bp_ON=1;}//�򿪷�����
void Beep_OFF() {bp_ON=0;}//�رշ�����
char set_Beep(int _fre,char _time){
	if(_fre<500 || _fre>2000) return 0;
	//���Ƶ�ʴ���2000����С��800���򱨴���֧�������Ƶ��
	bp.frequency = _fre;//Ƶ��
	bp.time = _time;//���������ʱ��
	time_count=0;		
	bp_count=0;
	bp.voice_judge = 2000/_fre;
	//����1000Hz����ôvoice_judge=2����2��500us��ƽ��ת
	return 1;							//����Ϊ��ʼ���������Ĳ���
}
void Voice(){// just can be used in Func_500us 
	if(bp_ON==0 ) {beep=0;return;}//����������رգ��򷵻�
	if(!bp.time) {beep=0;bp_ON=0;return;}
//���ʱ�䵽��򽫷������õ͵�ƽ�����رշ�����
		if( ++time_count == 2000&&bp.time!=0xff){
			bp.time--;
			time_count=0;
		}//2000��500us��1s����ʱ��-1
		if( ++bp_count==bp.voice_judge ){
			bp_count = 0;
			beep =~ beep;
		}//��ƽ��ת
}

#endif