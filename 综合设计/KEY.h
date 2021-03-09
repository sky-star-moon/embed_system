/*
key_init() 初始化按键缓存区
key_enable(char ) 使能某个按键，1、2、3
int key_push() 一旦有按键按下，则返回按下的第几个键
count_key() 驱动，轮询按键的状态，如果按下则将该按键按下的信息存入循环队列中
*/
#ifndef _KEY_
#define _KEY_
#include <STC15F2K60S2.H>
sbit key1=P3^2;
sbit key2=P3^3;
sbit key3=P1^7;
char key1_C1=0,key1_C2=0;
char key2_C1=0,key2_C2=0;
char key3_C1=0,key3_C2=0;
static char PUSH;
unsigned char k1_status=0,k2_status=0,k3_status=0;
static char key_on[4];
struct Queue{
	char head,tail;
	char queue[20];
}Q;
void key_unable(char _key){
	key_on[_key] = 0;
}
void key_init(){
	for(Q.head=0;Q.head<20;Q.head++) Q.queue[Q.head]=0;
	Q.head=0;Q.tail=0;
}
void key_enable(char _key){
	key_on[_key] = 1;
	switch (_key){
		case 1: key1_C1=0;key1_C2=0;break;
		case 2: key2_C1=0;key2_C2=0;break;
		case 3: key3_C1=0;key3_C2=0;break;
	}
}
void judge_push(){
	if((Q.tail+1)%20 == Q.head )
		return;
	//如果队列为空，则直接返回
	if(key1_C1==30){
		if(key1_C2>=20){
			if(k1_status==1)
				Q.queue[Q.tail++] = 1;
				//将按键信息存入队列中
				Q.tail=Q.tail%20;
				//循环队列，所以对尾指针上述处理
				k1_status=0;
			}
		else k1_status=1;
		key1_C1=0;
		key1_C2=0;
	}
		
	if(key2_C1==30){
		if(key2_C2>=20){
				if(k2_status==1)
					Q.queue[Q.tail++] = 2;
					Q.tail=Q.tail%20;
					k2_status=0;
			}
		else k2_status=1;
		key2_C1=0;
		key2_C2=0;
	}
		
	if(key3_C1==30){
		if(key3_C2>=20){
			if(k3_status==1)
				Q.queue[Q.tail++] = 3;
				Q.tail=Q.tail%20;
				k3_status=0;
			}
		else k3_status=1;
		key3_C1=0;
		key3_C2=0;
	}
}
void count_key(){
	if(!key_on[0] && !key_on[1] && !key_on[2]) return;
	//如果没有按键被使能，则直接返回
	if(key_on[1]){
		key1_C1++;
		if(key1==0)
			key1_C2++;
	}//消抖计数
	if(key_on[2]){
		key2_C1++;
		if(key2==0)
			key2_C2++;
	}
	if(key_on[3]){
		key3_C1++;
		if(key3==0)
			key3_C2++;
	}
	judge_push();
} 

int key_push(){
	if(Q.head!=Q.tail){
		PUSH = Q.queue[Q.head];
		Q.head++;
		Q.head=Q.head%20;
		return PUSH;
	}
	else return 0;
}
#endif