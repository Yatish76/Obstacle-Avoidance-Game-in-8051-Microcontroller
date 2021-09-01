#include <at89c5131.h>

bit d_locate = 1; //Bit flags for interrupts

/**********************************************************
   uart_init(): 
	Initialization function to be completed
	Initializes UART peripheral for 8-bit transfer, 
	1 start and 1 stop bits. 
	
	Please write TH1 value for required baud rate
***********************************************************/	
void uart_init(void)
{
	TMOD=0x21;			//Configure Timer 1 in Mode 2 and Timer 0 in mode 1
	TH1=243;					//Load TH1 to obtain require Baudrate (Refer Serial.pdf for calculations)
	SCON=0x50;			//Configure UART peripheral for 8-bit data transfer 
							//Start Timer 1
	ES=1;									//Enable Serial Interrupt
	EA=1;	                //Enable Global Interrupt
	
  TR1=1	;		
}









/**********************************************************
   receive_char(): 
	Receives a character through UART. Returns a 
	character.
***********************************************************/		
void receive_char(void)
{
	unsigned char ch = 0;
	ch = SBUF;					//Read data from SBUF
	switch(ch)
			   {
				       case 'q':lcd_cmd (0x82);
	                      lcd_write_char(68);
					              lcd_cmd (0xC2);
	                      lcd_write_char(' ');
				                d_locate=1 ;
								        break;
				
				       case 'a':lcd_cmd (0xC2);
	                      lcd_write_char(68);
					              lcd_cmd (0x82);
	                      lcd_write_char(' ');
				                d_locate=0 ;
							        	break;	
				       
					     
			  }
}


/**********************************************************
  Serial_ISR(): 
	Interrupt service routine for UART interrupt.
	Determines whether it is a transmit or receive
	interrupt and raise corresponding flag.
	Transmit or receive functions (defined above) monitor
	for these flags to check if data transfer is done.
***********************************************************/	
void serial_ISR(void) interrupt 4
{
		 if(RI==1)			//check whether RI is set
		{
			RI = 0;			//Clear RI flag
			
		}
		 receive_char();
}
