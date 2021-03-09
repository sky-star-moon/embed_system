#include "Data.h"
#define Coil_x(x) 0x01<<x
uchar Pi;
uint  Regs[8];//8个寄存器
uchar Regs_flag=0x38;//寄存器可读可写标志，如第0位为1，则第0位可读可写，第0位为0，只读
uint  Regs_num=8;//寄存器数量

extern uint player1,player2,player3;
uchar Coil=0x0A;//4个线圈
uchar Coil_flag=0x0f;//线圈标志位，1可读可写，0只读
uchar Coil_num=4;//线圈数量
uchar Ans=0x01;
extern bit p1,p2,p3;
uint crc;//CRC结果
uint addr,num,temp;
extern uchar E_CODE;//异常码
extern uint ModBusCRC(uchar* R,uchar length);
void procedure0102(uchar* Request1){
	//if(len!=8){E_CODE=6;return;}
	addr = Request1[2]*256+Request1[3];
	num  = Request1[4]*256+Request1[5];
	if(addr+num>Coil_num||num==0) {E_CODE=2;return;}
	//读取的线圈地址越界，非法数据地址
	Ans=0x01;
	Ans=Ans<<addr;
	for(Pi=1;Pi<num;Pi++){
		Ans=(Ans<<1)|Ans;
	}//读取的位置1，然后将ans与Coil相与，则可读取期望读的那些位
	Respond_length=0;
	Respond[Respond_length++]=Request1[0];
	Respond[Respond_length++]=Request1[1];
	Respond[Respond_length++]=0x01;
	Respond[Respond_length++]= Coil&Ans;
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;//生成响应报文
	
}
void procedure0304(uchar* Request1){
	addr = Request1[2]*256+Request1[3];
	num  = Request1[4]*256+Request1[5];;
	if(addr+num>Regs_num||num==0) {E_CODE=2;return;}
	//读取的寄存器地址越界，非法数据地址
	Respond_length=0;
	Respond[Respond_length++]=Request[0];
	Respond[Respond_length++]=Request[1];
	Respond[Respond_length++]=num*2;
	for(Pi=addr;Pi<addr+num;Pi++){
		Respond[Respond_length++]=Regs[Pi]>>8;
		Respond[Respond_length++]=Regs[Pi];
	}//将寄存器依次放入响应报文
	crc=ModBusCRC(Respond,Respond_length);
	Respond[Respond_length++]=crc>>8;
	Respond[Respond_length++]=crc;
}
void procedure05(uchar* Request1){
	addr = Request1[2]*256+Request1[3];
	temp = Request1[4]*256+Request1[5];
	if(addr>=Coil_num) {E_CODE=2;return;}
	//读取的线圈地址越界，非法数据地址
	if(Coil_flag & (0x01<<addr)){//如果当前线圈可以写
		if(temp==0xff00){//写1
			Coil=Coil | (0x01<<addr);
			if(addr==0) p1=1;
			else if(addr==1) p2=1;
			else if(addr==2) p3=1;
			else if(addr==3) Beep_ON();//同步更新到对应的物理现象上
		}
		else if(temp==0x0000){//写0
			Coil=Coil & ~(0x01<<addr);
			if(addr==0) p1=0;
			else if(addr==1) p2=0;
			else if(addr==2) p3=0;
			else if(addr==3) Beep_OFF();//同步更新到对应的物理现象上
		}
		else {E_CODE=3;return;}//写入的数据不是ff00或者0000，非法数据
	}
	else {E_CODE=2;return;}//如果写的过程中发现某个线圈只读，非法数据地址
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
	if(Regs_flag&Coil_x(addr)){//判断当前寄存器可写
		Regs[addr]=Request[4]*256+Request[5];
		if(addr==3) player1=Regs[3];
		else if(addr==4) player2=Regs[4];
		else if(addr==5) player3=Regs[5];
	}
		
	else {E_CODE=2;return;}//不可写，非法数据地址
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
	while(Ans<num+addr){//写多个线圈
		if(Coil_flag&Coil_x(Ans)){
			if(temp&Coil_x(Ans)){
				Coil=Coil | Coil_x(Ans);
				if(Ans==0) p1=1;
				else if(Ans==1) p2=1;
				else if(Ans==2) p3=1;
				else if(Ans==3) Beep_ON();//同步更新到对应的物理现象上
			}
			else{
				Coil=Coil & ~(Coil_x(Ans));
				if(Ans==0) p1=0;
				else if(Ans==1) p2=0;
				else if(Ans==2) p3=0;
				else if(Ans==3) Beep_OFF();//同步更新到对应的物理现象上
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

