#ifndef _APP_LAYER_
#define _APP_LAYER_
#include "Data.h"
#include "ModBus_Base_Layer.h"
#include "ModBus_High_Layer.h"
extern uchar Transmit;
void Error(uchar error_code);
uint CRC16,check_sum,CRC_tmp;
uchar i,j;
uchar E_CODE=0;
uint ModBusCRC(uchar* R,uchar length){//CRC校验
	CRC16=0xffff;
	for(i=0;i<length;i++){
		CRC16 = (*R)^CRC16;
		for(j=0;j<8;j++){
			CRC_tmp=CRC16 & 0x0001;
			CRC16=CRC16>>1;
			if(CRC_tmp)
				CRC16 = CRC16 ^ 0xa001;
		}
		R++;
	}
	check_sum = ((CRC16&0x00ff)*256 |(CRC16&0xff00)>>8);
	return check_sum;
}
void ModBus_Instruct_Handle(uchar* Request1){
	if(!packet_flag) return;//如果没有接收数据帧，则直接返回
		if(Request1[0]!=0x01){E_CODE=4;}//非本机号，ECODE=4，该错误码只是让从机忽略该帧
		else if(ModBusCRC(Request,len)){E_CODE=5;}//CRC检验错误，ECODE=5，该错误码只是让从机忽略该帧
		else{
			E_CODE=0;
			switch (Request1[1]){
				case 0x01:
				case 0x02:
					procedure0102(Request1);
					break;
				case 0x03:
				case 0x04:
					procedure0304(Request1);
					break;
				case 0x05:
					procedure05(Request1);
					break;
				case 0x06:
					procedure06(Request1);
					break;
				case 0x0f:
					procedure0F(Request1);
					break;
				case 0x10:
					procedure10(Request1);
					break;
				default:
					E_CODE=1;
			}
		}
		packet_flag=0;//数据帧处理完毕
		Transmit=1;//可以发送
	 Error(E_CODE);//异常码检查
		//Respond_length=3;
		//Respond length,Transmitting many _data withou it
		
}
void Error(uchar error_code){
	
	switch (error_code){
		case 0://无异常
			break;
		case 4://非本机
		case 5://CRC错误
			R1_T0=1;Res=0;Transmit=0;Busy=0;//直接返回监听状态，无数据包发送
			break;
		default://生成异常响应报文
			Respond[0]=0x01;
			Respond[1]=Request[1]|0x80;
			Respond[2]=error_code;
			Respond_length=3;
			crc=ModBusCRC(Respond,Respond_length);
			Respond[Respond_length++]=crc>>8;
			Respond[Respond_length++]=crc;
			break;
	}
	return;
}

#endif