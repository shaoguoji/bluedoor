#include <STC12C2052AD.h>	//STC12C5AxxS2ϵ�е�Ƭ��ͷ�ļ�

sbit	clock_clk 	= 	P3 ^ 3;	//ds1302_clk��ʱ���ߣ�//DS1302ʱ�ӽӿ�
sbit	clock_dat 	= 	P3 ^ 4;	//ds1302_dat�������ߣ�//DS1302ʱ�ӽӿ�
sbit	clock_Rst 	= 	P3 ^ 5;	//ds1302_Rst����λ�ߣ�//DS1302ʱ�ӽӿ�

//�����ۼ���A�еĸ�λ
sbit a0   = ACC ^ 0;sbit a1   = ACC ^ 1;sbit a2   = ACC ^ 2;
sbit a3   = ACC ^ 3;sbit a4   = ACC ^ 4;sbit a5   = ACC ^ 5;
sbit a6   = ACC ^ 6;sbit a7   = ACC ^ 7;
/*********************************************************************************************
��������1302��������
��  �ã� 
��  ������
����ֵ����
��  ����
��  ע��������DS1302оƬ+32.768KHz6pF����
/**********************************************************************************************/
//1302�������򣨵ײ�Э�飩
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
//DS1302д���ݣ��ײ�Э�飩
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
//DS1302д���ֽڣ��ײ�Э�飩
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
//DS1302�����ݣ��ײ�Э�飩
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
//����ʱ�����ݶ�ȡ
void read_clockS(unsigned char *yy,unsigned char *mo,unsigned char *dd,unsigned char *xq,unsigned char *hh,unsigned char *mm,unsigned char *ss){

	*ss = read_clock(0x81);//��ȡ������
	*mm = read_clock(0x83);//��ȡ��������
	*hh = read_clock(0x85);//Сʱ
	*dd = read_clock(0x87);//��
	*mo = read_clock(0x89);//��
	*yy = read_clock(0x8d);//��
	*xq = read_clock(0x8b);//����
	*xq = *xq%16; //�ֽڵ�4λ��BCD�����xq

}
/*****************************************************************************/
/*********************************************************************************************
��������1302��ʼ������
��  �ã� 
��  ������
����ֵ����
��  ������ʼ��ʱ��оƬ���Զ���Ⲣ��ʼ��
��  ע��������DS1302оƬ+32.768KHz6pF����
/**********************************************************************************************/
//����1302�ĳ�ʼʱ�䣨�Զ���ʼ����
void Init_1302(void){//-����1302�ĳ�ʼʱ�䣨2016��1��1��00ʱ00��00�������壩

	unsigned char f;

	if(read_clock(0x81) == 0x80){
		write_clock(0x8e,0x00);//����д����
		write_clock(0x8c,0x16);//��
		write_clock(0x8a,0x05);//����
		write_clock(0x88,0x01);//��
		write_clock(0x86,0x01);//��
		write_clock(0x84,0x00);//Сʱ
		write_clock(0x82,0x00);//����
		write_clock(0x80,0x00);//��
		write_clock(0x90,0x05);//������ã�0x05����磬0xa5��磩
			
		for(f=0;f<60;f=f+2){//�������RAMλΪ0
			write_clock(0xc0+f,0x00);
		}

		write_clock(0x8e,0x80);//��ֹд����
	}

}
/**********************************************************************************************/

/*********************************************************************************************
��������1302����ʱ�����ó���
��  �ã�Set_1302(unsigned char *time) 
��  ����20yy-mm-dd w hh-ff-ss(yy-��, mm-��, dd-��, w-����, hh-ʱ, ff-��, ss-��)
����ֵ����
��  ��������ʱ��Ϊָ��ʱ������
��  ע��������DS1302оƬ+32.768KHz6pF����
/**********************************************************************************************/
//����1302������ʱ��
void Set_date_time(unsigned char *time){//-����1302�ĳ�ʼ����ʱ��

		unsigned char yy, mm, dd, ww, hh, ff, ss;

		yy = ((time[2] - 0x30) * 16) + (time[3] - 0x30);
		mm = ((time[5] - 0x30) * 16) + (time[6] - 0x30);
		dd = ((time[8] - 0x30) * 16) + (time[9] - 0x30);
		ww = time[11] - 0x30;
		hh = ((time[13] - 0x30) * 16) + (time[14] - 0x30);	
		ff = ((time[16] - 0x30) * 16) + (time[17] - 0x30);
		ss = ((time[19] - 0x30) * 16) + (time[20] - 0x30);

		write_clock(0x8e,0x00);//����д����

		write_clock(0x8c,yy);//��
		write_clock(0x88,mm);//��
		write_clock(0x86,dd);//��
		write_clock(0x8a,ww);//����

		write_clock(0x84,hh);//Сʱ
		write_clock(0x82,ff);//����
		write_clock(0x80,ss);//��

		write_clock(0x8e,0x80);//��ֹд����
}
/**********************************************************************************************/
/*********************************************************************************************
��������1302��ʱ����
��  �ã�Set_time(unsigned char *time) 
��  ����hh-ff-ss(hh-ʱ, ff-��, ss-��)
����ֵ����
��  ��������ʱ��Ϊָ��ʱ��
��  ע��������DS1302оƬ+32.768KHz6pF����
/**********************************************************************************************/
//����1302������ʱ��
void Set_time(unsigned char *time){//-����1302�ĳ�ʼ����ʱ��

		unsigned char hh, ff, ss;

		hh = ((time[0] - 0x30) * 16) + (time[1] - 0x30);	
		ff = ((time[3] - 0x30) * 16) + (time[4] - 0x30);
		ss = ((time[6] - 0x30) * 16) + (time[7] - 0x30);

		write_clock(0x8e,0x00);//����д����

		write_clock(0x84,hh);//Сʱ
		write_clock(0x82,ff);//����
		write_clock(0x80,ss);//��

		write_clock(0x8e,0x80);//��ֹд����

}
/**********************************************************************************************/
/*********************************************************************************************
�����������ù�������ʱ�����
��  �ã�Set_remind_time(��hh-ff-ss��) 
��  ����"hh-ff-ss"ʱ���ַ���
����ֵ����
��  �����ѹ�������ʱ��д��1302RAM��0xc0~0xc4��ַ
��  ע��(hh-ʱ, ff-��, ss-��)
/**********************************************************************************************/
void Set_remind_time(unsigned char *time)
{

	unsigned char hh, ff, ss;

	hh = ((time[0] - 0x30) * 16) + (time[1] - 0x30);
	ff = ((time[3] - 0x30) * 16) + (time[4] - 0x30);
	ss = ((time[6] - 0x30) * 16) + (time[7] - 0x30);

	write_clock(0x8e,0x00);//����д����

	write_clock(0xc0, hh);
	write_clock(0xc2, ff);
	write_clock(0xc4, ss);
	
	write_clock(0x8e,0x80);//��ֹд����
	
}
/**********************************************************************************************/

/*********************************************************************************************
�����������÷�������ʱ���������
��  �ã�Set_alarm_time("hh1-ff1-ss1", "hh2-ff2-ss2") 
��  ����"hh1-ff1-ss1", "hh2-ff2-ss2"ʱ���ַ���
����ֵ����
��  �����ѷ�������ʱ������д��1302RAM��0xc6~0xd0��ַ
��  ע��(hh1, ff1, ss1-�ֱ�Ϊ��ʼʱ���ʱ���֣��룬hh2, ff2, ss2�ֱ�Ϊ����ʱ���ʱ���֣���)
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

	write_clock(0x8e,0x00);//����д����

	write_clock(0xc6, hh1);
	write_clock(0xc8, ff1);
	write_clock(0xca, ss1);

	write_clock(0xcc, hh2);
	write_clock(0xce, ff2);
	write_clock(0xd0, ss2);
	
	write_clock(0x8e,0x80);//��ֹд����	

}
/**********************************************************************************************/
/*********************************************************************************************
����������ȡDS1302ʱ�����
��  �ã�Get_time()
��  ������
����ֵ��"hh-ff-ss"ʱ���ַ���
��  ������1302��RAM�ж�ȡDS1302��ǰʱ��
��  ע��(hh-ʱ, ff-��, ss-��)
/**********************************************************************************************/
unsigned char *Get_time()
{
	unsigned char time[9];

	time[0] = read_clock(0x85) / 16 + 0x30;  //��ȡСʱ����	  	 
	time[1] = read_clock(0x85) % 16 + 0x30;
	time[2] = '-';
	time[3] = read_clock(0x83) / 16 + 0x30;  //��ȡ��������
	time[4] = read_clock(0x83) % 16 + 0x30;
	time[5] = '-';
	time[6] = read_clock(0x81) / 16 + 0x30;  //��ȡ������
	time[7] = read_clock(0x81) % 16 + 0x30;
	time[8] = '\0';

	return time;
}
/**********************************************************************************************/
/*********************************************************************************************
����������ȡ��������ʱ�����
��  �ã�Get_remind_time()
��  ������
����ֵ��"hh-ff-ss"ʱ���ַ���
��  ������1302��RAM�ж�ȡ�û����õĹ�������ʱ��
��  ע��(hh-ʱ, ff-��, ss-��)
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
����������ȡ��������ʱ���������
��  �ã�Get_alarm_time()
��  ������
����ֵ��"hh1-ff1-ss1,hh2-ff2-ss2"
��  ������1302��RAM�ж�ȡ�û����õĵ�����ʱ������
��  ע��(hh1, ff1, ss1-�ֱ�Ϊ��ʼʱ���ʱ���֣��룬hh2, ff2, ss2�ֱ�Ϊ����ʱ���ʱ���֣���)
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

