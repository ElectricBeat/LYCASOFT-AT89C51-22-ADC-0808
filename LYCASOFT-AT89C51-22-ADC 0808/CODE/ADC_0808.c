#include<reg51.h>
#include "8051_delay.h"

sbit ale=P3^0;		//address latch enable
sbit sc=P3^1;			//start conversion
sbit eoc=P3^2;		//end of conversion
sbit oe=P3^3;			//output enable
sbit ADD_A=P3^4;	//Address pins for selecting input channels.
sbit ADD_B=P3^5;
sbit ADD_C=P3^6;
sbit clk=P3^7;		// clock

sfr lcd_data_pin=0x90;  //Port P1
sbit rs=P2^0;
sbit rw=P2^1;
sbit en=P2^2;
sfr input_port=0x80;  //Port P0

unsigned int key,value,number,ascii1,ascii2,ascii3,flag,temp1,key1/*,data1,data2,data3,data11,data12,data13,data21,data22,data23*/;

void timer0() interrupt 1  // Function to generate clock of frequency 500KHZ using Timer 0 interrupt.
{
clk=~clk;
}

void lcd_command(unsigned char comm)  //Function to send command to LCD.
{
lcd_data_pin=comm;
en=1;
rs=0;
rw=0;
delay(1);
en=0;
}
           
void lcd_data(unsigned char disp)  //Function to send data to LCD.
{
lcd_data_pin=disp;
en=1;
rs=1;
rw=0;
delay(1);
en=0;
}

lcd_dataa(unsigned char *disp)  //Function to send string data to LCD.
{
	int x;
	for(x=0;disp[x]!=0;x++)
	{
		lcd_data(disp[x]);
	}
}

void lcd_ini()			//Function to inisialize the LCD
{
lcd_command(0x38);	// for using 8-bit 2 row LCD
delay(5);
lcd_command(0x0F);	// for display on cursor blinking
delay(5);
lcd_command(0x80);
delay(5);						//Force cursor to blink at line 1 positon 0
}

void BCD()					// Binary to decimal conversion to send the data to LCD
{
  key1++;
  key=0;
  flag=0;
	
  number=input_port;
  value=number%10;
	number=number/10;
	ascii1=value+48;

	if(number!=0)
	{
		value=number%10;
		number=number/10;
		ascii2=value+48;
		flag=1;
	}
	else
	{
		ascii2=48;
		flag=1;
	}
	
	if(number!=0)
	{
		value=number%10;
		number=number/10;
		ascii3=value+48;
		key=2;
	}
	else
	{
		ascii3=48;
		key=2;
	}
	
	if(key1==1)    
	{
		lcd_command(0xC0);
	}
	if(key1==2)
	{
		lcd_command(0xC5);
	}
	if(key1==3)
	{
		lcd_command(0xCA);
	}
	
	if(key==2)
	lcd_data(ascii3);
	if(flag==1)
	lcd_data(ascii2);
	lcd_data(ascii1);
	if(key1==3)
	{
		key1=0;
		ascii3=0;
		ascii2=0;
		ascii1=0;
		delay(10);
	} 
}

void adc()  //Function to drive ADC
{
while(1)
  {
   if(temp1==0)
   {
    ADD_C=0;  // Selecting input channel 1 using address lines
    ADD_B=0;
    ADD_A=0;
   }
   if(temp1==1)
   {
    ADD_C=0;  // Selecting input channel 2 using address lines
    ADD_B=0;
    ADD_A=1;
   }
   if(temp1==2)
   {
    ADD_C=0;  // Selecting input channel 3 using address lines
    ADD_B=1;
    ADD_A=0;
   }
   delay(2);
   ale=1;
   delay(2);
   sc=1;
   delay(1);
   ale=0;
   delay(1);
   sc=0;
   while(eoc==1);
   while(eoc==0);
   oe=1;
   BCD();
   delay(2);
   oe=0;
   temp1++;
   if(temp1==3)
   temp1=0;
  }
}

void main()
{
  eoc=1;
  ale=0;
  oe=0;
  sc=0;
  TMOD=0x02;				//Timer0 setting for generating clock of 500KHz using interrupt enable mode.
  TH0=0xFD;
  IE=0x82;
  TR0=1;
  temp1=0;
	key1=0;
	lcd_ini();
	lcd_dataa("SN1:");
	lcd_dataa(" SN2:");
	lcd_dataa(" SN3:");
  adc();
}