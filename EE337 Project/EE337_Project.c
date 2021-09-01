#include <at89c5131.h>
#include "lcd.h"		//Header file with LCD interfacing functions
#include "serial.c"	//C file with UART interfacing functions


bit b2=0, b1=0, b0=1;                     //state bits to store the value of current obstacle configuration
bit b_2=0,b_1=0,b_0=0;                    //temporary states for carrying operations 



void next_state (void);                   //function to generate next state of obstacle
void shift_obstacle(void) ;               //function to shift the obstacles to left by one LCD cell
void game_over(void);                     //function to be called after 'D' and '*' occupies same position
void send_all_char(void);                 //function use to send characters to lcd
void reset_lcd(void);                     //function to set lcd to default value

unsigned int current_score=0;
unsigned int high_score=0;
unsigned char d_location=0x82 ;
unsigned char lfsr_count=0;
unsigned char lcd_add_1[20] = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32} ;
unsigned char lcd_add_2[20] = {32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32} ;

	void lcd_score(void)                     //function to show the strings "score:" and "high score:" on lcd
{
	 lcd_cmd(0x80);
	 lcd_write_string("Score:");
	 lcd_cmd (0xC0) ;
	 lcd_write_string("High Score:");
	 
}	


void main(void)
{
	
	
	lcd_add_1[16]=32 ;                            //assigning obstacle corresponding to configuration no.1
	lcd_add_1[17]=42 ;
	lcd_add_1[18]=42 ;
	lcd_init();                                   //initialising lcd for printing characters
	uart_init();                                  //initialising UART for serial datas transfer
	lcd_cmd (0x82);
	lcd_write_char('D');
	
	
	while(1)                                          //loops continuosly forever
	{    
		
  	   msdelay(1000);                               //delay of 1 sec
		   if(d_locate==1){                             //assigning value to d_location according to latest key pressed
			    d_location=0x82;}
			 if(d_locate==0){
			    d_location=0xc2;}
			 lcd_cmd(d_location);
       lcd_write_char(68);
					
			 current_score++ ;                               //updating the current score on the basis of time elapsed in econd
		   
       if(lfsr_count==3){                              //lfsr count shows the time elapsed after last obstacle configuration update
				  lfsr_count=0;
		      next_state();                                //after every 3 sec we load new obstacle configuration at the end of array
		   }			
		   shift_obstacle();                               //we shift the values store in lcd_array by 1 to left
			 lfsr_count++ ;
		   if ((lcd_add_1[d_location-128]==42) || (lcd_add_2[d_location-192]==42) ){               //checking for game over
	   	     ES=0;                                                                               //disabling the serial interrupts during score display
				   current_score-- ;
				   if(current_score > high_score){                                                     //updating the new high score
					     high_score=current_score ;
					 }
				   game_over();                                //calling game over function which displays the current score and high score
					 reset_lcd();                                //reset function resets lcd screen and puts 'D' at 1st line 3rd row position 
				   current_score=0;                            //resetting current score to value 0
					 lfsr_count=0 ;
					 next_state();                               //next state is preserved for the next game
					 ES=1;                                       //enabling the serial interrupts again
					 
			 }
			 
			 send_all_char ();	                             //send all characters store in lcd_array to lcd screen
		   
			
		
		}	
	}


void game_over(void){                                       //game over function
	
	         unsigned char curr_score_array[5];               //temporary array to store digits of score
	         unsigned char high_score_array[5];               
					 int i;
	         lcd_cmd(0x01);                                   //clear lcd screen
					 lcd_score();
					 int_to_string(current_score, curr_score_array);  //converting integer to string
					 int_to_string(high_score, high_score_array);     //converting integer to string
	
					 
					 for(i=0;i<5;i++){                                //sending the score to lcd 
					    
						  lcd_cmd(0x8B + i);
						  lcd_write_char(curr_score_array[i]);
						  lcd_cmd(0xCB + i);
						  lcd_write_char(high_score_array[i]);
					 }
					 d_location=0x82;                                //updating d_location value with default value
					 msdelay(3000);
					 lcd_cmd(0x01);                                  //clear lcd screen
					 
					 
}



void reset_lcd(void){                                     //reset lcd function
   int k;
	 for(k=0;k<20;k++){                                     //storing space character in every position of lcd_array
	    lcd_add_1[k]=32;                                    //ASCII value of space=32
		  lcd_add_2[k]=32;
	 }
	 d_locate=1 ;                                           //'D' in 1st row, 3rd column
	 
}	

void next_state (void) {                                  //next state function
	
	
	unsigned char temp_b ;                                  //declaring character for generating switch cases
	b_0=b1 ;                                                //assigning updated values to state bits according to given LFSR for pseudo randomness
	temp_b = b_0 ;
	b_1=b2 ;
	temp_b = temp_b + 2*b_1 ;
	b_2= b2^b0 ;
	temp_b = temp_b + 4*b_2 ;                               //calculating temp_b from the next state values
	b0=b_0 ;
	b1=b_1 ;
	b2=b_2 ;
	
  switch(temp_b){                                        //choosing obstacle configuration on the basis of next state bits
	
    case 1 :  lcd_add_1[16]=32 ;                         //configuaration 1    " **"
		          lcd_add_1[17]=42 ;                         //                    "   "
		          lcd_add_1[18]=42 ;
		          lcd_add_2[16]=32 ;
		          lcd_add_2[17]=32 ;
		          lcd_add_2[18]=32 ;
		          break;
		          
		
		case 2 :  lcd_add_2[16]=32 ;                         //configuration 2    "   "
		          lcd_add_2[17]=42 ;                         //                   " **"
		          lcd_add_2[18]=42 ;
		          lcd_add_1[16]=32 ;
		          lcd_add_1[17]=32 ;
		          lcd_add_1[18]=32 ;
		          break;
		
		case 3 :  lcd_add_1[16]=32 ;                         //configuration 3    " * "
		          lcd_add_1[17]=42 ;                         //                   "   "
		          lcd_add_1[18]=32 ;
		          lcd_add_2[16]=32 ;
		          lcd_add_2[17]=32 ;
		          lcd_add_2[18]=32 ;
							break;
		
		case 4 :  lcd_add_2[16]=32 ;                         //configuration 4    "   "
		          lcd_add_2[17]=32 ;                         //                   "  *"
		          lcd_add_2[18]=42 ;
			        lcd_add_1[16]=32 ;
		          lcd_add_1[17]=32 ;
		          lcd_add_1[18]=32 ;
							break;
		
		case 5 :  lcd_add_2[16]=32 ;                         //configuration 5    "   "
		          lcd_add_2[17]=42 ;                         //                   " * "
		          lcd_add_2[18]=32 ;
			        lcd_add_1[16]=32 ;
		          lcd_add_1[17]=32 ;
		          lcd_add_1[18]=32 ;
							break;
		
		case 6 :  lcd_add_1[16]=32 ;                         //configuration 6    "  *"
		          lcd_add_1[17]=32 ;                         //                   "   "
		          lcd_add_1[18]=42 ;
			        lcd_add_2[16]=32 ;
		          lcd_add_2[17]=32 ;
		          lcd_add_2[18]=32 ;
							break;
							
		case 7 :  lcd_add_1[16]=32 ;                         //configuration 7    "   "
		          lcd_add_1[17]=32 ;                         //                   "   "
		          lcd_add_1[18]=32 ;					
		          lcd_add_2[16]=32 ;
		          lcd_add_2[17]=32 ;
		          lcd_add_2[18]=32 ;
							break;
		
	}	
	
	
}		

void shift_obstacle(void){                              //shift obstacle function
   int j;
	 for (j=0;j<19;j++){                                  //shifting every element in the array to left by one place
	    lcd_add_1[j]=lcd_add_1[j+1];
		  lcd_add_2[j]=lcd_add_2[j+1];
	 }
	 
}

void send_all_char(void){                               //send all characters function
   unsigned int m;
	 
	 for (m=0;m<16;m++){                                  //sending the characters stored in lcd_array i.e. lcd_add_1 and lcd_add_2 to the lcd
	    lcd_cmd(m + 0x80);
		  lcd_write_char(lcd_add_1[m]);
		  lcd_cmd(m + 0xc0);
		  lcd_write_char(lcd_add_2[m]);
	     
	 }
   lcd_cmd(d_location);                                 //1st row and 2nd column is always occupied by 'D'
   lcd_write_char(68);	 
}
