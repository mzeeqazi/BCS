/***************************************************************************************************************************************************************
 code by: @mzeeqazi
 This code is for our COA project Titled 'BCS'
***************************************************************************************************************************************************************/
#ifndef F_CPU
#define F_CPU 8000000UL // "set the clock to 8MHz"
#endif

#include <avr/io.h>
#include <util/delay.h>  // Delay.h is time delay Header file
/***************************************************************************************************************************************************************
                                                            function prototypes begins
***************************************************************************************************************************************************************/

void getKeyPress(void);
void keyData(int keyWord);
void shiftout(unsigned char myDataOut);
void pinMode(int position, int value);

/***************************************************************************************************************************************************************
                                                            function prototypes ends
***************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************
                                                            Global Variable declaration begins
***************************************************************************************************************************************************************/
int keyRow,columnData,ledRow,resulte,reset_loop,time_to_check = 0;
unsigned char matrix_data[]={0,0,0,0};
unsigned char word_1=0,word_2=0;
/***************************************************************************************************************************************************************
                                                            Global Variable declaration ends
***************************************************************************************************************************************************************/
int main(void){
 DDRD = 0x0F;//pins PD0-PD3 outputs and pins PD4-PD7 inputs
 DDRA = 0xFF;//pins PA0-PA7 as input
  while(1){// keeps the program running all the time

   for(ledRow=0;ledRow<4;ledRow++){// this is the loop for scanning the LED display
	 time_to_check ++;//counts the time until the next key press check(to prevent flickering of the matrix)
	 PORTA = 1<<ledRow;//sets only one pin from PD0-PD3 HIGH
	 pinMode(5,0);//latch off
	 shiftout(matrix_data[ledRow]);//writes the data to the shift register
	 pinMode(5,1);//latch on
	 if(time_to_check == 125){//looks when the time is right to check for a key press
	  getKeyPress();// goes to the key press function
	  time_to_check =0;//resets the "counter"
	  }
     else
	  _delay_ms(1);// a delay for each LED row
	 pinMode(5,0);//latch off
	 shiftout(0x00);// sets the shift registers outputs to low
	 pinMode(5,1);//latch on
  }
 }
 return 1;
}

/***************************************************************************************************************************************************************
                                                         Function definition begins
***************************************************************************************************************************************************************/

void getKeyPress(){ // checks what key is being pressed
     for(keyRow=0;keyRow<4;keyRow++){ // scans the switch matrix(4 rows)
    PORTD = (1<<keyRow); // sends a HIGH to one row at a time
    columnData = (PIND & 0xF0);// reads the state of each column
	_delay_us(100);// a small delay
	if(columnData != 0){// if something is pressed columndata is bigger then zero
   	 keyData(columnData | (1<<keyRow));//goes to the data analysis function
	 _delay_us(400);//a small delay like in the LED scanning loop
   }
  }
 }

void keyData(int keyInput){//function for determining what each button does

   switch (keyInput){

    case 0b10000100:// first input row "1"
	 word_1 = word_1 << 1;
	 word_1 = word_1 | 0x01;
	 matrix_data[3] = word_1;
	 break;
    case 0b10000010://first input row "0"
	 word_1 = word_1 << 1;
	 matrix_data[3] = word_1;
     break;
    case 0b10001000://first input row "DEL"
	 word_1 = 0;
	 matrix_data[3] = word_1;
	 break;
    case 0b01000100://second input row "1"
	 word_2 = word_2 << 1;
	 word_2 = word_2 | 0x01;
	 matrix_data[2] = word_2;
	 break;
    case 0b01000010://second input row "0"
	 word_2 =  word_2<<1;
	 matrix_data[2] = word_2;
	 break;
    case 0b01001000://second input row "DEL"
	 word_2 = 0;
	 matrix_data[2] = word_2;
	 break;
    case 0b00010010://multiplication
	 resulte = word_1 * word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b00010100://subtraction
	 resulte = word_1 - word_2;
     matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b00011000://division
	 resulte = word_1 / word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b00010001://addition
	 resulte = word_1 + word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b00100010://modulo
	 resulte = word_1 % word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
     break;
    case 0b00100100://XOR
	 resulte = word_1 ^ word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b00101000://OR
	 resulte = word_1 | word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b00100001://AND
	 resulte = word_1 & word_2;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b10000001://NOT
	 resulte = ~resulte;
	 matrix_data[1] = resulte & 0x00FF;
	 matrix_data[0] = (resulte >> 8) & 0x00FF;
	 break;
    case 0b01000001://DEL ALL
	 word_1 = 0;
	 word_2 = 0;
	 resulte =0;
	 for(reset_loop=0;reset_loop<4;reset_loop++)
	  matrix_data[reset_loop] = 0;
     break;
    default:// do nothing at all if more then 1 button is pressed
	 break;
    }
   }


void shiftout(unsigned char myDataOut) { // the function for sending the data to the shift register
  int i;
  int pinState=0;
  pinMode(4,0);
  pinMode(6,0);
  for (i=7; i>=0; i--)  {
    pinMode(6,0);
    if ( myDataOut & (1<<i) )
      pinState = 1;
    else
      pinState = 0;
    pinMode(4,pinState);
    pinMode(6,1);
    pinMode(4,0);
  }
  pinMode(6,0);
}


void pinMode(int position, int value)
{
        if (value == 0)
          PORTA &= ~(1 << position);
        else
          PORTA |= (1 << position);
}
/***************************************************************************************************************************************************************
                                                         Function definition ends
***************************************************************************************************************************************************************/

