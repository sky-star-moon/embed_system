#include "STC15F2k60S2.H"
sbit SDA = P4 ^ 0;      //I2C总线的数据线
sbit SCL = P5 ^ 5;      //I2C总线的时钟线
void delay()        //延时4us
{
    ;;
}
void IIC_init()     //I2C总线初始化
{
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}

void start()        //主机启动信号
{
    SDA = 1;
    delay();
    SCL = 1;
    delay();
    SDA = 0;
    delay();
}
void stop()         //停止信号
{
    SDA = 0;
    delay();
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}

void respons()      //从机应答信号
{
    uchar i = 0;
    SCL = 1;
    delay();
    while( SDA == 1 && ( i < 255 ) ) //表示若在一段时间内没有收到从器件的应答则
        i++;                //主器件默认从期间已经收到数据而不再等待应答信号。
    SCL = 0;
    delay();
}

void writebyte( uchar date ) //对24C02写一个字节数据
{
    uchar i, temp;
    temp = date;
    for( i = 0; i < 8; i++ )
    {
        temp = temp << 1;
        SCL = 0;
        delay();
        SDA = CY;
        delay();
        SCL = 1;
        delay();
    }
    SCL = 0;
    delay();
    SDA = 1;
    delay();
}

uchar readbyte()            //从24C02读一个字节数据
{
    uchar i, k;
    SCL = 0;
    delay();
    SDA = 1;
    delay();
    for( i = 0; i < 8; i++ )
    {
        SCL = 1;
        delay();
        k = ( k << 1 ) | SDA;
        delay();
        SCL = 0;
        delay();
    }
    delay();
    return k;
}
void write_add( uchar addr, uchar date ) //对24C02的地址addr，写入一个数据date
{
    start();
    writebyte( 0xa0 );
    respons();
    writebyte( addr );
    respons();
    writebyte( date );
    respons();
    stop();
}
uchar read_add( uchar addr )      //从24C02的addr地址，读一个字节数据
{
    uchar date;
    start();
    writebyte( 0xa0 );
    respons();
    writebyte( addr );
    respons();
    start();
    writebyte( 0xa1 );
    respons();
    date = readbyte();
    stop();
    return date;
}
