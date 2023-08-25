/*
 * AVRGCC3.c
 *
 * Created: 08.11.2020 20:24:06
 *  Author: HP
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>


volatile static unsigned long temp_v;
volatile static unsigned char color_r, color_g, color_b,status,temp_in, cloc_dim;

int main (void)
{
    #define PIN_DATA PB4
	#define PORT_DATA PORTB
	#define DDRDATA DDRB
	#define DDR_DIN DDRD
	#define PORT_DIN PORTD
	#define DIN 0
	#define EXT_TAKT_PIN 2
	#define EXT_INT_PIN 3
	#define EXT_CLOC_PIN 4
	#define EXT_PIN PIND
	#define SUBI 1024
	#define cloc_of_dimer 1
	#define EXT_CLOC 16000000


	//настройка таймера для пикания
  	TCCR1B=1<<WGM12|1<<CS10|1<<CS12;
	OCR1A=(EXT_CLOC/(2*1024*cloc_of_dimer))-1;

	//настройка АЦП

  	ADMUX=0<<ADLAR|1<<MUX1|1<<MUX0;
	
	//массив значений температуры от АЦП 
	
	//настройка вывода под led
	DDRDATA = 1<<PIN_DATA;
	
	DDR_DIN = 1<<DIN|0<<EXT_TAKT_PIN|0<<EXT_INT_PIN|0<<EXT_CLOC_PIN;
	PORT_DIN = 1<<DIN|1<<EXT_TAKT_PIN|1<<EXT_INT_PIN|1<<EXT_CLOC_PIN;
	
	//настройка прерываний для энкодера
	MCUCR=1<<ISC01|1<<ISC10;
	GICR=1<<INT0|1<<INT1;
	
	color_g =255;
	color_b = 255;
	color_r = 255;
	temp_in = 34;
//	OUT_TEMP (0x59);
	while(1)
	{		
		
		sei();
	}
	return 0;//TODO:: Please write your application code 
}

ISR (INT0_vect)
{
	DELAY(65536);
//	DELAY(65536);
status++;
	if (status==1)
	{
		ADCSRA=1<<ADEN|1<<ADPS2|1<<ADPS1|1<<ADPS0|1<<ADSC|1<<ADIE|1<<ADATE;
	}
	if (status==2)
	{
		ADCSRA=0;
	OUT_TEMP(temp_in);

	}		
	if(status==3)
	{
		TIMSK=1<<OCIE1A;
		ADCSRA=1<<ADEN|1<<ADPS2|1<<ADPS1|1<<ADPS0|1<<ADSC|1<<ADIE|1<<ADATE;
	}
	
	if (status==4)
	{
		TIMSK=0<<OCIE1A;
		ADCSRA=0;
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		WRITEBYTE(0,0,0);
		status=0;
	}
}

ISR (INT1_vect)
{
	if (status==2)
	{
		volatile char a,b;
		a=(EXT_PIN &(1<<EXT_CLOC_PIN))/2;
		b=EXT_PIN &(1<<EXT_INT_PIN);

				ADCSRA=0;
		if(a==b)
		{
			temp_in++;
			if(temp_in==99)
			{
				temp_in=0;
			}
			}
		else
		{
			temp_in--;
			if(temp_in==0)
			{
				temp_in=99;
			}
		}
		//temp_in=45;
	OUT_TEMP(temp_in);
	}
	//DELAY(6553);
}

ISR (ADC_vect)
{
	volatile static unsigned int  addr, temp_c, time;
		unsigned int TEMPO [110] = {513,  524,  535,  546,  557,  567,  578,  588,  598,  608,  618,  619,  623,  630,  639,  650,  661,  673,  685,  696,  708,  719,  729,  740,  749,  758,  767,  775,  783,  790,  797,  804,  810,  817,  823,  828,  834,  840,  845,  850,  855,  860,  865,  870,  875,  879,  884,  888,  892,  896,  900,  904,  908,  912,  916,  919,  923,  926,  930,  933,  937,  938,  940,  942,  944,  947,  949,  952,  954,  956,  959,  961,  963,  965,  967,  969,  971,  972,  974,  975,  977,  978,  979,  981,  982,  983,  984,  985,  986,  987,  988,  990,  991,  992,  993,  994,  995,  996,  997,  998, 999, 1000, 1001, 1002}; 
		unsigned char Nomer	[110]=	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,101, 102};
	time++;
	addr=0;
	temp_c=0;
	temp_v=temp_v+ADC;   
	if(time==SUBI)
	{
	temp_v=temp_v/SUBI;

	while(temp_v>=temp_c)
	{
		addr++;
		temp_c=TEMPO[addr];
	}
	temp_c=Nomer[addr];
		if (temp_c==temp_in)
	{
		cloc_dim=5;
	}
	OUT_TEMP(temp_c);
	time=0;
	}	
//	reti ();
}


void WRITEBYTE (volatile unsigned char DATA_COLOR_1,volatile unsigned char DATA_COLOR_2,volatile unsigned char DATA_COLOR_3 )
{
	volatile unsigned char i;
//	SREG = 0;
	for(i=0; i<=7; i++)
	{
		SREG &=~(1<<0);
		DATA_COLOR_1 = DATA_COLOR_1*2;
		if ((SREG &1)==1)
		{
			one();
		}
		else
		{
			zero();
		}
		
	}
	
	for(i=0; i<=7; i++)
	{
		SREG &=~(1<<0);
		DATA_COLOR_2 = DATA_COLOR_2*2;
		if ((SREG &1)==1)
		{
			one();
		}
		else
		{
			zero();
		}
		
	}
	
	for(i=0; i<=7; i++)
	{
		SREG &=~(1<<0);
		DATA_COLOR_3 = DATA_COLOR_3*2;
		if ((SREG &1)==1)
		{
			one();
		}
		else
		{
			zero();
		}
		
	}
	return 0;
}

int zero (void)
{
	PORT_DATA = 1<<PIN_DATA;
	PORT_DATA = 1<<PIN_DATA;
	PORT_DATA = 1<<PIN_DATA;
	PORT_DATA = 1<<PIN_DATA;
	PORT_DATA = 1<<PIN_DATA;
	PORT_DATA = 1<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	PORT_DATA =0<<PIN_DATA;
	return 0;
}

int one (void)
{
	PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA = 1<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		PORT_DATA =0<<PIN_DATA;
		return 0;
}

/*void HSV_TO_RGB (volatile int H, volatile unsigned char S, volatile unsigned char V)
{
	char RED, GREEN, BLUO;
	volatile long f;
	volatile int H1;
	H1=H;
	while(H1>=60)
	{
		H1=H1-60;
	}
	f=H/60-H1;
	while(H1>=6)
	{
		H1=H1-6;
	}
	if(H1==0)
	{
		RED=V;
		GREEN=V*(1-(1-f)*S);
		BLUO=V*(1-S);
		WRITEBYTE(GREEN, RED, BLUO);
	}
	if(H1==1)
	{
		RED=V*(1-f*S);
		GREEN=V;
		BLUO=V*(1-S);
		WRITEBYTE(GREEN, RED, BLUO);
	}
	if(H1==2)
	{
		RED=V*(1-S);
		GREEN=V;
		BLUO=V*(1-(1-f)*S);
		WRITEBYTE(GREEN, RED, BLUO);
	}
	if(H1==3)
	{
		RED=V*(1-S);
		GREEN=V*(1-f*S);
		BLUO=V;
		WRITEBYTE(GREEN, RED, BLUO);
	}
	if(H1==4)
	{
		RED=V*(1-(1-f)*S);
		GREEN=V*(1-S);
		BLUO=V;
		WRITEBYTE(GREEN, RED, BLUO);
	}
	if(H1==5)
	{
		RED=V;
		GREEN=V*(1-S);
		BLUO=V*(1-f*S);
		WRITEBYTE(GREEN, RED, BLUO);
	}
	return 0;
}
*/
void DELAY (volatile unsigned int cloc)
{
	cli();
	volatile long int a;
	while(cloc>0)
	{
		a=0xffff;
		while(a>=0)
		{
			a--;
		}
		cloc--;
	}
	return 0;
}

void OUT_TEMP(volatile unsigned char temp)
{
	volatile int decem, onen;
	decem=0;
	onen=0;
	decem=temp/10;
	onen=temp-decem*10;
	decem=decem*16;
	temp=decem+onen;
	volatile unsigned char NUMBERS[10] = {0b11111100,0b00011000,0b10110110,0b00111110,0b01011010,0b01101110,0b11101110, 0b00111000, 0b11111110, 0b01111110};
	volatile unsigned char i,temp_low,temp_high,ext_temp;
	RAINBOW(temp);
	temp_high=temp;
	temp_low=temp*16;
	temp_low=temp_low/16;
	temp_high=temp_high/16;

	
	
//	temp_high=2;
	ext_temp=NUMBERS[temp_high];
	for(i=0; i<=6; i++)
	{
	SREG &=~(1<<0);
	ext_temp=ext_temp*2;
	if((SREG &1)==1)
	{
		WRITEBYTE(color_g, color_r, color_b);
	}
	else
	{
			WRITEBYTE(0,0,0);
	}
	}		
			
		ext_temp=NUMBERS[temp_low];
		for(i=0; i<=6; i++)
		{
		SREG &=~(1<<0);
		ext_temp=ext_temp*2;
		if((SREG &1)==1)
		{
			WRITEBYTE(color_g, color_r, color_b);
		}
		else
		{
				WRITEBYTE(0,0,0);
		}
		}		
	
		
		return 0;
}

void RAINBOW(unsigned int level)
{
	level=level*5;
	color_b=255;
	color_g=0;
	color_r=0;
	volatile unsigned int timer=0;
	while (timer<=level)
	{
		if (color_b>0)
		{
			color_b--;
			color_g++;
		}
		else 
		{
			if (color_g>0)
			{
				color_g--;
				color_r++;
			}
		}
		timer++;
	}
}

ISR(TIMER1_COMPA_vect)
{
//	WRITEBYTE(0,255,255);
	if (cloc_dim==0)
	{
		PORT_DIN = 1<<DIN|1<<EXT_TAKT_PIN|1<<EXT_INT_PIN|1<<EXT_CLOC_PIN;
		
	}
	else
	{
		PORT_DIN=PORT_DIN^(1<<DIN);
		//PORT_DIN = 0<<DIN|1<<EXT_TAKT_PIN|1<<EXT_INT_PIN|1<<EXT_CLOC_PIN;
		cloc_dim--;	
	}		
	
}