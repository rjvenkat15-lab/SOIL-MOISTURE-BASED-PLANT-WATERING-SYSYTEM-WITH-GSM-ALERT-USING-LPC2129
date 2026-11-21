 #include<LPC21xx.H>

#define LCD_D 0xff<<8
#define RS 1<<16
#define E 1<<17

#define GREEN_LED 1<<5
#define RED_LED 1<<6

#define SENSOR 1<<2								

#define MOTOR1 (1<<21)|(1<<22)

#define DRY_STATUS 1
#define WET_STATUS 0
#define INITIAL_STATUS -1

void LCD_INIT(void);
void LCD_COMMAND(unsigned char);
void LCD_DATA(unsigned char);
void LCD_STR(unsigned char *);
void DELAY_MS(unsigned int);
void UART0_CONFIG(void);
void UART0_TX(unsigned char);
void UART0_STR(unsigned char *);
void SEND_MSG(unsigned char *);
void DRY(void);
void WET(void);

int main()
{	
  	int laststatus = INITIAL_STATUS;
	int currentstatus;
  	LCD_INIT();
	UART0_CONFIG();

	IODIR0 |= GREEN_LED|RED_LED|MOTOR1;
	IODIR0 &=~(SENSOR);
	while(1)
	{	
			if(IOPIN0&(SENSOR))
			currentstatus = DRY_STATUS;
			else
			currentstatus = WET_STATUS;
			if(currentstatus != laststatus)
			{
			 if(currentstatus == DRY_STATUS)
			{
			
				DRY();
			}
		 	else
			{
				WET();
			}
			laststatus = currentstatus;
			}
	DELAY_MS(500);
		
	}
}

void LCD_INIT(void)
{
	IODIR0|=LCD_D|RS|E;
	LCD_COMMAND(0x01);//clear the display
	LCD_COMMAND(0x02);//return cursor to home position
	LCD_COMMAND(0x0C);//display ON,cursor OFF
	LCD_COMMAND(0x38);//8bit interface mode with both line
}

void LCD_COMMAND(unsigned char cmd)
{
	IOCLR0=LCD_D;
	IOSET0=(cmd<<8);
	IOCLR0=RS;
	IOSET0=E;
	DELAY_MS(2);
	IOCLR0=E;
}

void LCD_DATA(unsigned char d)
{
	IOCLR0=LCD_D;
	IOSET0=(d<<8);
	IOSET0=RS;
	IOSET0=E;
	DELAY_MS(2);
	IOCLR0=E;
}

void DELAY_MS(unsigned int ms)
{
T0PR=15000-1;
T0TCR=0X01;
while(T0TC<ms);
T0TCR=0X03;
T0TCR=0X00;
}

void LCD_STR(unsigned char *s)
{
	while(*s)
		LCD_DATA(*s++);
}

void UART0_CONFIG(void)
{
	PINSEL0=0x05;//P0.0 as txd0 and p0.1 as RxD0
	U0LCR = 0x83;//DLAB=1 and "8N1" frame is configured
	U0DLL = 97;
	U0DLM = 0;
	U0LCR = 0x03;//DLAB=0
}

void UART0_TX(unsigned char ch)
{
	while(((U0LSR >>5)&1)==0);
	U0THR = ch;
}
 
void UART0_STR(unsigned char *str)
{
	while(*str)
		UART0_TX(*str++);
} 

void SEND_MSG(unsigned char *msg)
{
	UART0_STR("AT\r");
	DELAY_MS(100);
	UART0_STR("AT+CMGF=1\r");
	DELAY_MS(100);
	UART0_STR("AT+CMGS=\"9080005624\"\r");
	DELAY_MS(100);
	UART0_STR(msg);
	UART0_TX(26);
	DELAY_MS(2000);
}

void DRY(void)
{
	IOSET0 =(1<<21);
	IOCLR0=(1<<22);
	IOSET0 = RED_LED;
	IOCLR0 = GREEN_LED;
	LCD_COMMAND(0x01);

	LCD_COMMAND(0x80);
	LCD_STR("SOIL:DRY");

	LCD_COMMAND(0xC0);
	LCD_STR("PUMP:ON");

	SEND_MSG("SOIL DRY WATERING STARTED");
}

void WET(void)
{
	IOCLR0=MOTOR1;
	IOSET0 = GREEN_LED;
	IOCLR0 = RED_LED;
	LCD_COMMAND(0x01);
	LCD_COMMAND(0x80);
	LCD_STR("SOIL:WET");

	LCD_COMMAND(0xC0);
	LCD_STR("PUMP:OFF");

	SEND_MSG("SOIL WET WATERING STOPPED");
}




