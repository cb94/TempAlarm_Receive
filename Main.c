#include "STC15F2K60S2.H"

/**********************
���ű�������
***********************/
sbit LED_SEL = P2^3;						//����ܺ�LED��ѡ���ź�
sbit beep = P3^4;								//������
sbit KEY1 = P3^2;								//����1  �¶ȼ�
sbit KEY2 = P3^3;								//����2	 �¶ȼ�
sbit KEY3 = P1^7;								//����3  �л�ģʽ


/**********************
ȫ�ֱ�������
***********************/
unsigned int receive_buf = 11;				//���ջ��壬�����շ���־0xca���ж�
unsigned int receive_data = 20;				//���ܵ�����
		
unsigned int temp=20;									//ʵ���¶�
unsigned int tempset=30;							//��ʾ�趨�¶�
unsigned int tempset_h=30;						//�����趨
unsigned int tempset_l=10;						//�����趨
unsigned int mode=1;									//�ߵ�����ʾ,1Ϊ����,0Ϊ����
unsigned int Status=1;										//����״̬,1Ϊ����״̬,0Ϊ����״̬

unsigned int position = 1;			//�������ʾλ
unsigned char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x76,0x38};//��ѡ
unsigned char weixuan[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};//λѡ
/**********************
��������(�жϺ�������)
***********************/
void TimerInit();			//��ʱ������
void UartInit();			//����1����
void init();					//��ʼ�������죬�����жϿ���
void Delay(int n);

/********************************
 * ��ʱ����
********************************/
void Delay(int n)
{
	int y;
	while(n--)
	{
		y=60;
		while(y--);
	}
}

/********************************
 * �������죬�жϿ�������
********************************/
void init()
{
	//�����������
	P0M1 = 0x00;
	P0M0 = 0xff;
	P2M1 = 0x00;
	P2M0 = 0x08;
	P3M1 = 0x00;
	P3M0 = 0x10;		
	ET1 = 0;			//��ֹT1�ж�
	ET0 = 1;			//�򿪶�ʱ��T0�ж�
	ES = 1;				//�򿪴���1�ж�
	IE2 = 0X04;		//�򿪶�ʱ��2�ж�
	EA = 1;				//�����ж�
}

/********************************
 * ����1���������
********************************/
void UartInit()
{	
	PCON &= 0x7F;		//�����ʲ����٣�SMOD=0
	SCON = 0x50;		//����1ʹ�ù�����ʽ1��REN=1(�����н���)
	AUXR &= 0xFE;		//����1ѡ��ʱ��T1��Ϊ�����ʷ�������S1ST2=0
	AUXR1 = 0x40;		//����1��P3.6���գ���P3.7����
	PS = 1;					//���ô����ж�Ϊ������ȼ�
}

/********************************
 * ��ʱ�����������
********************************/
void TimerInit()	
{
	AUXR |= 0x40;		//��ʱ��T1Ϊ1Tģʽ���ٶ��Ǵ�ͳ8051��12��������Ƶ��
	TMOD &= 0x0F;		//���T1ģʽλ
	TMOD |= 0x20;		//����T1ģʽλ��ʹ��8λ�Զ���װģʽ
	TL1 = 0x70;			//���ó�ֵ
	TH1 = 0x70;			//����T1��װֵ
	TR1 = 1;				//T1���п���λ��1������T1����
	
	AUXR |= 0x80;		//��ʱ��T0Ϊ1Tģʽ�����ٶ��Ǵ�ͳ8051��12��������Ƶ��
	TMOD &= 0xF0;		//���ö�ʱ��ģʽΪ16λ�Զ���װ
	TL0 = 0xCD;			//���ö�ʱ��ֵ
	TH0 = 0xF4;			//���ö�ʱ��ֵ
	TF0 = 0;				//���TF0��־
	TF0 = 0;				//T0�����־λ����
	TR0 = 1;				//T0���п���λ��1������T0����
	
//��ʱ��T2Ϊ12Tģʽ�������������ʾ
	T2L = (65536-2500)%256;			//��λ��װֵ
	T2H = (65536-2500)/256;			//��λ��װֵ
	AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
}




/********************************
 * ��ʱ��0�жϵĲ��������ڷ�����
********************************/
void Time0() interrupt 1
{
	if(Status&&(temp>=tempset_h||temp<=tempset_l))
		beep=~beep;
}

/********************************
* ����1�жϵĲ������������RIֵ1�������ж�
********************************/
void URAT1() interrupt 4
{
	if(RI)							//�ж��Ƿ�����ж�
	{
		RI = 0;						//�����ж������־λ��0
		if(receive_buf == 0xca)	//�ж���һ���Ƿ��յ�0xca��־
		{
			receive_data = SBUF;	//��ʽ��������
			if(receive_data/10<10) //����һ�����쳣����
				temp=receive_data;
		}
		receive_buf = SBUF;			//����ν��յ������ݴ����Զ���Ļ����У��ȴ���һ�εıȽ�
	}
}

/********************************
* ��ʱ��2�жϵĲ�����������ʾ����ܺ�LED��
********************************/
void Timer2() interrupt 12
{
	LED_SEL=0;
	position++;
	if(position==8) 
		position=0;
	P2=weixuan[position];
	switch(position)
	{
		case 1:P0=segtable[temp/10];break; 
		case 2:P0=segtable[temp%10];break; 
		case 4:P0=(mode?segtable[16]:segtable[17]);break; 
		case 5:P0=segtable[tempset%1000/100];break; 
		case 6:P0=segtable[tempset%100/10];break; 
		case 7:P0=segtable[tempset%10];break; 
		default:P0=0x00;break; 
	}	
	Delay(10);
	P0=0x00;
	
	if(Status&&(temp>=tempset_h||temp<=tempset_l))//���ڱ���״̬ʱ������ʾ����ܺ�LED��
	{
		LED_SEL=1;
		P0=0xff;
		Delay(1);
		P0=0x00;
		LED_SEL=0;
	}
}


/********************************
 * ������
********************************/
void main()
{
	TimerInit();		//���ö�ʱ��
	UartInit();			//���ô���1
	init();					//��ʼ��
	while(1){
	if(KEY2==0)  //�����¶�+1
	{
		Delay(20);
		if(KEY2==0)
		{
			while(KEY2==0);
			if(mode) 
				tempset_h+=1;
			else 
				tempset_l+=1;
		}
	}
	if(KEY1==0)  //�����¶�-1
	{
		Delay(20);
		if(KEY1==0)
		{
			while(KEY1==0);
			if(mode) 
				tempset_h-=1;
			else 
				tempset_l-=1;
		}
	}
	if(KEY3==0)  //�ߵ����л�
	{
		Delay(20);
		if(KEY3==0)
		{
			while(KEY3==0);
			if(Status&&(temp>=tempset_h||temp<=tempset_l)) 
				Status=0;
			else
				mode=!mode;	
		} //�ı�ߵ���״̬����
	}
	if(mode)     //�ı��趨�¶�
		tempset=tempset_h;
	else 
		tempset=tempset_l;
	if(temp<tempset_h&&temp>tempset_l) //�ı䱨��״̬
		Status=1; 
	}
}

