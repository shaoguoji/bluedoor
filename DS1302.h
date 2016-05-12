#include <STC12C2052AD.h>	//STC12C5AxxS2系列单片机头文件

sbit	clock_clk 	= 	P3 ^ 3;	//ds1302_clk（时钟线）//DS1302时钟接口
sbit	clock_dat 	= 	P3 ^ 4;	//ds1302_dat（数据线）//DS1302时钟接口
sbit	clock_Rst 	= 	P3 ^ 5;	//ds1302_Rst（复位线）//DS1302时钟接口

//定义累加器A中的各位
sbit a0   = ACC ^ 0;sbit a1   = ACC ^ 1;sbit a2   = ACC ^ 2;
sbit a3   = ACC ^ 3;sbit a4   = ACC ^ 4;sbit a5   = ACC ^ 5;
sbit a6   = ACC ^ 6;sbit a7   = ACC ^ 7;
/*********************************************************************************************
函数名：1302驱动程序
调  用： 
参  数：无
返回值：无
结  果：
备  注：适用于DS1302芯片+32.768KHz6pF晶体
/**********************************************************************************************/
//1302驱动程序（底层协议）
void clock_out(unsigned char dd){
	ACC=dd;// 0x80 = 1000 0000
	clock_dat=a0; clock_clk=1; clock_clk=0;
	clock_dat=a1; clock_clk=1; clock_clk=0;
	clock_dat=a2; clock_clk=1; clock_clk=0;
	clock_dat=a3; clock_clk=1; clock_clk=0;
	clock_dat=a4; clock_clk=1; clock_clk=0;
	clock_dat=a5; clock_clk=1; clock_clk=0;
	clock_dat=a6; clock_clk=1; clock_clk=0;
	clock_dat=a7; clock_clk=1; clock_clk=0;
}
/*****************************************************************************/
//DS1302写数据（底层协议）
void write_clock(unsigned char ord, unsigned char dd){
	clock_clk=0;
	clock_Rst=0;
	clock_Rst=1;
	clock_out(ord);
	clock_out(dd);
	clock_Rst=0;
	clock_clk=1;
}
/*****************************************************************************/
//DS1302写入字节（底层协议）
unsigned char clock_in(void){
	clock_dat=1;
	a0=clock_dat;
	clock_clk=1; clock_clk=0; a1=clock_dat;
	clock_clk=1; clock_clk=0; a2=clock_dat;
	clock_clk=1; clock_clk=0; a3=clock_dat;
	clock_clk=1; clock_clk=0; a4=clock_dat;
	clock_clk=1; clock_clk=0; a5=clock_dat;
	clock_clk=1; clock_clk=0; a6=clock_dat;
	clock_clk=1; clock_clk=0; a7=clock_dat;
	return(ACC);
}
/*****************************************************************************/
//DS1302读数据（底层协议）
unsigned char read_clock(unsigned char ord){
	unsigned char dd=0;
	clock_clk=0;
	clock_Rst=0;
	clock_Rst=1;
	clock_out(ord);
	dd=clock_in();
	clock_Rst=0;
	clock_clk=1;
	return(dd);
}
/*****************************************************************************/
//常用时钟数据读取
void read_clockS(unsigned char *yy,unsigned char *mo,unsigned char *dd,unsigned char *xq,unsigned char *hh,unsigned char *mm,unsigned char *ss){

	*ss = read_clock(0x81);//读取秒数据
	*mm = read_clock(0x83);//读取分钟数据
	*hh = read_clock(0x85);//小时
	*dd = read_clock(0x87);//日
	*mo = read_clock(0x89);//月
	*yy = read_clock(0x8d);//年
	*xq = read_clock(0x8b);//星期
	*xq = *xq%16; //字节低4位的BCD码放入xq

}
/*****************************************************************************/
/*********************************************************************************************
函数名：1302初始化程序
调  用： 
参  数：无
返回值：无
结  果：初始化时钟芯片，自动检测并初始化
备  注：适用于DS1302芯片+32.768KHz6pF晶体
/**********************************************************************************************/
//设置1302的初始时间（自动初始化）
void Init_1302(void){//-设置1302的初始时间（2016年1月1日00时00分00秒星期五）

	unsigned char f;

	if(read_clock(0x81) == 0x80){
		write_clock(0x8e,0x00);//允许写操作
		write_clock(0x8c,0x16);//年
		write_clock(0x8a,0x05);//星期
		write_clock(0x88,0x01);//月
		write_clock(0x86,0x01);//日
		write_clock(0x84,0x00);//小时
		write_clock(0x82,0x00);//分钟
		write_clock(0x80,0x00);//秒
		write_clock(0x90,0x05);//充电设置（0x05不充电，0xa5充电）
			
		for(f=0;f<60;f=f+2){//清除闹钟RAM位为0
			write_clock(0xc0+f,0x00);
		}

		write_clock(0x8e,0x80);//禁止写操作
	}

}
/**********************************************************************************************/

/*********************************************************************************************
函数名：1302日期时间设置程序
调  用：Set_1302(unsigned char *time) 
参  数：20yy-mm-dd w hh-ff-ss(yy-年, mm-月, dd-日, w-星期, hh-时, ff-分, ss-秒)
返回值：无
结  果：设置时钟为指定时间日期
备  注：适用于DS1302芯片+32.768KHz6pF晶体
/**********************************************************************************************/
//设置1302的日期时间
void Set_date_time(unsigned char *time){//-设置1302的初始日期时间

		unsigned char yy, mm, dd, ww, hh, ff, ss;

		yy = ((time[2] - 0x30) * 16) + (time[3] - 0x30);
		mm = ((time[5] - 0x30) * 16) + (time[6] - 0x30);
		dd = ((time[8] - 0x30) * 16) + (time[9] - 0x30);
		ww = time[11] - 0x30;
		hh = ((time[13] - 0x30) * 16) + (time[14] - 0x30);	
		ff = ((time[16] - 0x30) * 16) + (time[17] - 0x30);
		ss = ((time[19] - 0x30) * 16) + (time[20] - 0x30);

		write_clock(0x8e,0x00);//允许写操作

		write_clock(0x8c,yy);//年
		write_clock(0x88,mm);//月
		write_clock(0x86,dd);//日
		write_clock(0x8a,ww);//星期

		write_clock(0x84,hh);//小时
		write_clock(0x82,ff);//分钟
		write_clock(0x80,ss);//秒

		write_clock(0x8e,0x80);//禁止写操作
}
/**********************************************************************************************/
/*********************************************************************************************
函数名：1302调时程序
调  用：Set_time(unsigned char *time) 
参  数：hh-ff-ss(hh-时, ff-分, ss-秒)
返回值：无
结  果：设置时钟为指定时间
备  注：适用于DS1302芯片+32.768KHz6pF晶体
/**********************************************************************************************/
//设置1302的日期时间
void Set_time(unsigned char *time){//-设置1302的初始日期时间

		unsigned char hh, ff, ss;

		hh = ((time[0] - 0x30) * 16) + (time[1] - 0x30);	
		ff = ((time[3] - 0x30) * 16) + (time[4] - 0x30);
		ss = ((time[6] - 0x30) * 16) + (time[7] - 0x30);

		write_clock(0x8e,0x00);//允许写操作

		write_clock(0x84,hh);//小时
		write_clock(0x82,ff);//分钟
		write_clock(0x80,ss);//秒

		write_clock(0x8e,0x80);//禁止写操作

}
/**********************************************************************************************/
/*********************************************************************************************
函数名：设置关门提醒时间程序
调  用：Set_remind_time(“hh-ff-ss”) 
参  数："hh-ff-ss"时间字符串
返回值：无
结  果：把关门提醒时间写入1302RAM的0xc0~0xc4地址
备  注：(hh-时, ff-分, ss-秒)
/**********************************************************************************************/
void Set_remind_time(unsigned char *time)
{

	unsigned char hh, ff, ss;

	hh = ((time[0] - 0x30) * 16) + (time[1] - 0x30);
	ff = ((time[3] - 0x30) * 16) + (time[4] - 0x30);
	ss = ((time[6] - 0x30) * 16) + (time[7] - 0x30);

	write_clock(0x8e,0x00);//允许写操作

	write_clock(0xc0, hh);
	write_clock(0xc2, ff);
	write_clock(0xc4, ss);
	
	write_clock(0x8e,0x80);//禁止写操作
	
}
/**********************************************************************************************/

/*********************************************************************************************
函数名：设置防盗报警时间区间程序
调  用：Set_alarm_time("hh1-ff1-ss1", "hh2-ff2-ss2") 
参  数："hh1-ff1-ss1", "hh2-ff2-ss2"时间字符串
返回值：无
结  果：把防盗报警时间区间写入1302RAM的0xc6~0xd0地址
备  注：(hh1, ff1, ss1-分别为开始时间的时，分，秒，hh2, ff2, ss2分别为结束时间的时，分，秒)
/**********************************************************************************************/
void Set_alarm_time(unsigned char *time)
{

	unsigned char hh1, ff1, ss1, hh2, ff2, ss2;

	hh1 = ((time[0] - 0x30) * 16) + (time[1] - 0x30);
	ff1 = ((time[3] - 0x30) * 16) + (time[4] - 0x30);
	ss1 = ((time[6] - 0x30) * 16) + (time[7] - 0x30);

	hh2 = ((time[9] - 0x30) * 16) + (time[10] - 0x30);
	ff2 = ((time[12] - 0x30) * 16) + (time[13] - 0x30);
	ss2 = ((time[15] - 0x30) * 16) + (time[16] - 0x30);

	write_clock(0x8e,0x00);//允许写操作

	write_clock(0xc6, hh1);
	write_clock(0xc8, ff1);
	write_clock(0xca, ss1);

	write_clock(0xcc, hh2);
	write_clock(0xce, ff2);
	write_clock(0xd0, ss2);
	
	write_clock(0x8e,0x80);//禁止写操作	

}
/**********************************************************************************************/
/*********************************************************************************************
函数名：读取DS1302时间程序
调  用：Get_time()
参  数：无
返回值："hh-ff-ss"时间字符串
结  果：从1302的RAM中读取DS1302当前时间
备  注：(hh-时, ff-分, ss-秒)
/**********************************************************************************************/
unsigned char *Get_time()
{
	unsigned char time[9];

	time[0] = read_clock(0x85) / 16 + 0x30;  //读取小时数据	  	 
	time[1] = read_clock(0x85) % 16 + 0x30;
	time[2] = '-';
	time[3] = read_clock(0x83) / 16 + 0x30;  //读取分钟数据
	time[4] = read_clock(0x83) % 16 + 0x30;
	time[5] = '-';
	time[6] = read_clock(0x81) / 16 + 0x30;  //读取秒数据
	time[7] = read_clock(0x81) % 16 + 0x30;
	time[8] = '\0';

	return time;
}
/**********************************************************************************************/
/*********************************************************************************************
函数名：读取关门提醒时间程序
调  用：Get_remind_time()
参  数：无
返回值："hh-ff-ss"时间字符串
结  果：从1302的RAM中读取用户设置的关门提醒时间
备  注：(hh-时, ff-分, ss-秒)
/**********************************************************************************************/
unsigned char *Get_remind_time()
{

	unsigned char time[9];

	time[0] = read_clock(0xc1) / 16 + 0x30;	  
	time[1] = read_clock(0xc1) % 16 + 0x30;
	time[2] = '-';
	time[3] = read_clock(0xc3) / 16 + 0x30;
	time[4] = read_clock(0xc3) % 16 + 0x30;
	time[5] = '-';
	time[6] = read_clock(0xc5) / 16 + 0x30;
	time[7] = read_clock(0xc5) % 16 + 0x30;
	time[8] = '\0';

	return time;
}
/**********************************************************************************************/

/*********************************************************************************************
函数名：读取防盗报警时间区间程序
调  用：Get_alarm_time()
参  数：无
返回值："hh1-ff1-ss1,hh2-ff2-ss2"
结  果：从1302的RAM中读取用户设置的盗报警时间区间
备  注：(hh1, ff1, ss1-分别为开始时间的时，分，秒，hh2, ff2, ss2分别为结束时间的时，分，秒)
/**********************************************************************************************/
unsigned char *Get_alarm_time()
{
    
	unsigned char time[18];

	time[0] = read_clock(0xc7) / 16 + 0x30;	  
	time[1] = read_clock(0xc7) % 16 + 0x30;
	time[2] = '-';
	time[3] = read_clock(0xc9) / 16 + 0x30;
	time[4] = read_clock(0xc9) % 16 + 0x30;
	time[5] = '-';
	time[6] = read_clock(0xcb) / 16 + 0x30;
	time[7] = read_clock(0xcb) % 16 + 0x30;
	time[8] = ',';
	time[9] = read_clock(0xcd) / 16 + 0x30;	  
	time[10] = read_clock(0xcd) % 16 + 0x30;
	time[11] = '-';
	time[12] = read_clock(0xcf) / 16 + 0x30;
	time[13] = read_clock(0xcf) % 16 + 0x30;
	time[14] = '-';
	time[15] = read_clock(0xd1) / 16 + 0x30;
	time[16] = read_clock(0xd1) % 16 + 0x30;
	time[17] = '\0';

	return time;
}
/**********************************************************************************************/

