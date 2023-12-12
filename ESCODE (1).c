#include <LPC17xx.h>
#define RS 8 //P0.9 as Register Select
#define EN 9 //P0.8 as LCD Enable
#define DT 4 //P0.4 to P0.7 as Data Lines
float x, y, temp;
unsigned long int temp1,temp3;
unsigned char flag1 =0, flag2 =0;
unsigned long a, b, temp2, r1, i;
//LCD Initialization commands
unsigned long int init_command[] =
{0x30,0x30,0x30,0x20,0x28,0x0c,0x06,0x01,0x80};
unsigned int digits[] = {0, 0, 0, 0}; // will store the digit to be displayed
void display(void);
public void value(void);
void init_counter1(void)
{
LPC_PINCON->PINSEL3 = (3<<16);// cap 3.1 (P1.18)
LPC_TIM3->CTCR = 0x05; // Counter at +ve edge of CAP3.1
}
void delay(void)
{
LPC_TIM0->TCR = 0x00000002; // Timer0 Reset
LPC_TIM0->EMR = 0X20;//Set match bit upon match
LPC_TIM0->PR = 3000; //for 1 ms
LPC_TIM0->MR0 = 1000; //for 1 second
LPC_TIM0->MCR = 0x00000004; // stop PC and TC on MR0
LPC_TIM0->TCR = 0x00000001; // Timer0 Enable
while ( !(LPC_TIM0->EMR & 0x01)); // wait until match
}
//To generate the intended delay
void delay_lcd(unsigned int r1)
{
unsigned int r;
for(r=0; r<r1; r++);//Empty loop working as delay
return;
}
void port_write()
{
LPC_GPIO0->FIOPIN = 0;
LPC_GPIO0->FIOPIN = temp3;//Sending data to the pins
if (flag1 == 0) //checking for command or data
LPC_GPIO0->FIOCLR = 1<<RS;
else
LPC_GPIO0->FIOSET = 1<<RS;
LPC_GPIO0->FIOSET = 1<<EN;//Enabling the LCD to write data
delay_lcd(25);
LPC_GPIO0->FIOCLR = 1<<EN;
delay_lcd(1000000); //Generating delay
}
void lcd_init()
{
LPC_GPIO0->FIODIR = 1<<RS|1<<EN|0XF<<DT;//P0.4-P0.9 as Output
flag1 =0;
//Sending commands for Initialization of LCD
for (i=0; i<9; i++)
{
temp1 = init_command[i];
flag2 = (flag1 == 1) ? 0 :((temp1 == 0x30) || (temp1 == 0x20)) ? 1 : 0;
temp3 = temp1 & 0xf0;
port_write();
if (!flag2)//If LCD is in 4-bit mode sending the data accordingly
{
temp3 = temp1 & 0x0f;
temp3 = temp3 << DT;//Shifting the data to put it on the intended pins
port_write();
}
}
}
//To display the initial message
void disp_data()
{
unsigned char msg[16]="Waterflow is:"; //Initial Message
i=0;
flag1=1;
while(msg[i]!='\0') //Until the message ends
{
temp1=msg[i];
temp3 = temp1 & 0xf0; //Extracting the most significant 4 bits
port_write();
temp3 = temp1 & 0x0f;//Extracting the remaining bits
temp3 = temp3 << DT;
port_write();
i++;
}
flag1=0; //Command mode for lcd
temp3=0xc; //Sending the command to put the cursor on the 2nd row
temp3=temp3<<DT;
port_write();
temp3=0;
temp3=temp3<<DT;
port_write();
}
int main(void)
{
SystemInit();
SystemCoreClockUpdate();

lcd_init();
disp_data();
LPC_PINCON->PINSEL0 &= 0X0000000; //P0.4 to P0.9 as GPIO
LPC_PINCON->PINSEL1 |= 1 << 16; //P0.24 as ADC input (ADD0.1)
LPC_SC->PCONP |= (1 << 1 | 1<<2 | 1<<3); //Power to the ADC by enabling the 12th pin of
//PCONP (Power Control for Peripheral)
init_counter1();
while(1)
{
LPC_TIM3->TCR=2;//Reset Counter1
LPC_TIM3->TCR=1;//Start Counter1
delay(); // wait for 1 second
a = (long)LPC_TIM3->TC ;
value();
}
}
public void value()
{

b = ((long)a/7.5)*60; //The data is present on 4th to 15th bit
//to get the digital value in lower bit positions


digits[3] = ((int)b / 100); //Extracting 1st digit
digits[1] = ((int)(b) % 10); //Extracting 3rd digit
digits[2] = ((int)(b /10) % 10); //Extracting 2nd digit after decimal
//Display the temperature on the lcd
display();
}
//To Display the extracted Temperature
void display(void)
{
flag1=1;
temp3=3; //Sending the ASCII value of the digits one by one
temp3=temp3<<DT;
port_write();
temp3=digits[3];
temp3=temp3<<DT;
port_write();
temp3=3;
temp3=temp3<<DT;
port_write();

temp3=digits[2];
temp3=temp3<<DT;
port_write();

temp3=3;
temp3=temp3<<DT;
port_write();
temp3=digits[1];
temp3=temp3<<DT;
port_write();
temp3=4;
temp3=temp3<<DT;
port_write();
temp3=12;
temp3=temp3<<DT;
port_write();
temp3=2;
temp3=temp3<<DT;
port_write();
temp3=15;
temp3=temp3<<DT;
port_write();
temp3=4;
temp3=temp3<<DT;
port_write();
temp3=13;
temp3=temp3<<DT;
port_write();


flag1=0;
temp3=0xc; //Again sending the command to set cursor at starting of the 2nd row
temp3=temp3<<DT;
port_write();
temp3=0; //This helps to overwrite the new temperature over the old one
temp3=temp3<<DT;
port_write();
delay_lcd(3000000);
}
