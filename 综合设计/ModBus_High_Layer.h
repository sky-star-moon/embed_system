#include "Data.h"
#define Coil_x(x) 0x01<<x
uchar Pi;
uint  Regs[8];//8���Ĵ���
uchar Regs_flag=0x38;//�Ĵ����ɶ���д��־�����0λΪ1�����0λ�ɶ���д����0λΪ0��ֻ��
uint  Regs_num=8;//�Ĵ�������

extern uint player1,player2,player3;
uchar Coil=0x0A;//4����Ȧ
uchar Coil_flag=0x0f;//��Ȧ��־λ��1�ɶ���д��0ֻ��
uchar Coil_num=4;//��Ȧ����
uchar Ans=0x01;
extern bit p1,p2,p3;
uint crc;//CRC���
uint addr,num,temp;
extern uchar E_CODE;//�쳣��
extern uint ModBusCRC(uchar* R,uchar length);
void procedure0102(uchar* Request1){
	//if(len!=8){E_CODE=6;return;}
	addr = Request1[2]*256+Request1[3];
	num  = Request1[4]*256+Request1[5];
	if(addr+num>Coil_num||num==0) {E_CODE=2;return;}
	//��ȡ����Ȧ��ַԽ�磬�Ƿ����ݵ�ַ
	Ans=0x01;
	Ans=Ans<<addr;
	for(Pi=1;Pi<num;Pi++){
		Ans=(Ans<<1)|Ans;
	}//��ȡ��λ��1��Ȼ��ans��Coil���룬��ɶ�ȡ����������Щλ
	Respond_length=0;
	Respond[Respond_length++]=Request1[0];
	Respond[Respond_length++]=Request1[1];
	Respond[Respond_length++]=0x01;
	Respond[Respond_length++]= Coil&Ans;
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;//������Ӧ����
	
}
void procedure0304(uchar* Request1){
	addr = Request1[2]*256+Request1[3];
	num  = Request1[4]*256+Request1[5];;
	if(addr+num>Regs_num||num==0) {E_CODE=2;return;}
	//��ȡ�ļĴ�����ַԽ�磬�Ƿ����ݵ�ַ
	Respond_length=0;
	Respond[Respond_length++]=Request[0];
	Respond[Respond_length++]=Request[1];
	Respond[Respond_length++]=num*2;
	for(Pi=addr;Pi<addr+num;Pi++){
		Respond[Respond_length++]=Regs[Pi]>>8;
		Respond[Respond_length++]=Regs[Pi];
	}//���Ĵ������η�����Ӧ����
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;
}
void procedure05(uchar* Request1){
	addr = Request1[2]*256+Request1[3];
	temp = Request1[4]*256+Request1[5];
	if(addr>=Coil_num) {E_CODE=2;return;}
	//��ȡ����Ȧ��ַԽ�磬�Ƿ����ݵ�ַ
	if(Coil_flag & (0x01<<addr)){//�����ǰ��Ȧ����д
		if(temp==0xff00){//д1
			Coil=Coil | (0x01<<addr);
			if(addr==0) p1=1;
			else if(addr==1) p2=1;
			else if(addr==2) p3=1;
			else if(addr==3) Beep_ON();//ͬ�����µ���Ӧ������������
		}
		else if(temp==0x0000){//д0
			Coil=Coil & ~(0x01<<addr);
			if(addr==0) p1=0;
			else if(addr==1) p2=0;
			else if(addr==2) p3=0;
			else if(addr==3) Beep_OFF();//ͬ�����µ���Ӧ������������
		}
		else {E_CODE=3;return;}//д������ݲ���ff00����0000���Ƿ�����
	}
	else {E_CODE=2;return;}//���д�Ĺ����з���ĳ����Ȧֻ�����Ƿ����ݵ�ַ
	Respond_length=0;
	Respond[Respond_length++]=Request1[0];
	Respond[Respond_length++]=Request1[1];
	Respond[Respond_length++]=Request1[2];
	Respond[Respond_length++]=Request1[3];
	Respond[Respond_length++]=Request1[4];
	Respond[Respond_length++]=Request1[5];
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;
}

void procedure06(uchar* Request1){
	//if(len!=5){E_CODE=6;return;}
	addr = Request1[2]*256+Request1[3];
	if(addr>=Regs_num) {E_CODE=2;return;}
	if(Regs_flag&Coil_x(addr)){//�жϵ�ǰ�Ĵ�����д
		Regs[addr]=Request[4]*256+Request[5];
		if(addr==3) player1=Regs[3];
		else if(addr==4) player2=Regs[4];
		else if(addr==5) player3=Regs[5];
	}
		
	else {E_CODE=2;return;}//����д���Ƿ����ݵ�ַ
	Respond_length=0;
	Respond[Respond_length++]=Request1[0];
	Respond[Respond_length++]=Request1[1];
	Respond[Respond_length++]=Request1[2];
	Respond[Respond_length++]=Request1[3];
	Respond[Respond_length++]=Request1[4];
	Respond[Respond_length++]=Request1[5];
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;
}

void procedure0F(uchar* Request1){
	
	addr = Request1[2]*256+Request1[3];
	num  = Request1[4]*256+Request1[5];
	temp = Request1[6]*256+Request1[7];
	if(addr>=Coil_num) {E_CODE=2;return;}

	Ans = addr;
	while(Ans<num+addr){//д�����Ȧ
		if(Coil_flag&Coil_x(Ans)){
			if(temp&Coil_x(Ans)){
				Coil=Coil | Coil_x(Ans);
				if(Ans==0) p1=1;
				else if(Ans==1) p2=1;
				else if(Ans==2) p3=1;
				else if(Ans==3) Beep_ON();//ͬ�����µ���Ӧ������������
			}
			else{
				Coil=Coil & ~(Coil_x(Ans));
				if(Ans==0) p1=0;
				else if(Ans==1) p2=0;
				else if(Ans==2) p3=0;
				else if(Ans==3) Beep_OFF();//ͬ�����µ���Ӧ������������
			}
			Ans++;
		}
		else {E_CODE=2;return;}
	}
	Respond_length=0;
	Respond[Respond_length++]=Request1[0];
	Respond[Respond_length++]=Request1[1];
	Respond[Respond_length++]=Request1[2];
	Respond[Respond_length++]=Request1[3];
	Respond[Respond_length++]=Request1[4];
	Respond[Respond_length++]=Request1[5];
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;
	return;
}

void procedure10(uchar* Request1){
	addr = Request1[2]*256+Request1[3];
	num  = Request1[4]*256+Request1[5];
	if(addr+num>=Regs_num) {E_CODE=2;return;}
	Ans = addr;
	temp=0;
	while(Ans<num+addr){
		if(Regs_flag&Coil_x(Ans)){
			Regs[Ans]=Request[7+temp]*256+Request[8+temp];
			if(Ans==3) player1=Regs[3];
			else if(Ans==4) player2=Regs[4];
			else if(Ans==5) player3=Regs[5];
			temp=temp+2;Ans++;
		}
		else {E_CODE=2;return ;}
	}
	Respond_length=6;
	Respond[0]=Request[0];
	Respond[1]=Request[1];
	Respond[2]=Request[2];
	Respond[3]=Request[3];
	Respond[4]=Request[4];
	Respond[5]=Request[5];
	
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;
}

