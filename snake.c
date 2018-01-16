 /****** SNAKE ******************************************************************
 *
  * Authors: Connor Kerry and Guy Margalit 
  * Date Created: 12/7/2017
  * Date Last Edited: 12/10/2017
 * 
 * Program hierarchy 
 *
 * Mainline
 *   Initial
 * 
 * InitializeGLCD
 *
 * Draw Cell
 * 
 * Clear Cell
 *
 * UpdateGLCD
 * 
 * MoveSnake
 *
 * CheckKey
 *
 * HiPriISR (included just to show structure)
 *
 * LoPriISR
 *   TMR0handler
  * ***************************************************************************
  * Bugs to fix:
  *     - Food generates inside snake
  *     - Snake goes through itself
  *     - Need start up screen
  * 
 ******************************************************************************/
#include <delays.h>
#include <stdio.h>
#include <stdlib.h>
#include "GLCDroutinesEasyPic.h"

#pragma config FOSC=HS1, PWRTEN=ON, BOREN=ON, BORV=2, PLLCFG=OFF
#pragma config WDTEN=OFF, CCP2MX=PORTC, XINST=OFF

/******************************************************************************
 * Global variables
 ******************************************************************************/
unsigned char DIRECTION = 'R';//'R' = right, 'L' = left, 'U' = up, 'D' = down
unsigned char RandX;
unsigned char RandY;
char fromInit = 0;
char collision;
int EEcount;
unsigned char fi;
unsigned char fj;
struct Snake {
    unsigned char Length;
    unsigned char X[100];
    unsigned char Y[100];
} Snake;
struct Food {
    unsigned char X;
    unsigned char Y;
} Food;
unsigned char READ_CELL[8];
const rom far unsigned char START_SCREEN[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x40,0x40,0x60,0x20,0xA0,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0x80,0x80,0x80,0x80,0xA0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xF0,0xDC,
0x8E,0x82,0x02,0x03,0x01,0x01,0x31,0x3D,0x3D,0x31,0x10,0xC0,0xFF,0xFF,0xE1,0xC1,
0xC0,0xD0,0xB0,0xBC,0xB0,0xB0,0x80,0x80,0x80,0x80,0x81,0x81,0x81,0xC1,0xEE,0x3E,
0x3C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x80,0xC0,0xC0,0xC0,0x80,0x00,0x00,0xC0,0x80,0x00,0x00,0xC0,0x00,0x00,
0x00,0x80,0xC0,0x80,0x00,0x00,0x00,0xC0,0x00,0x00,0xC0,0xC0,0x00,0x00,0xC0,0xC0,
0xC0,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x07,0x7F,0x84,0x02,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0x40,0x01,0xC1,0xC1,0x81,0x81,0x00,0x00,0x00,0x80,0x80,0x80,
0x80,0x80,0x40,0x01,0x03,0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x83,
0xFF,0xFC,0x07,0x03,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x87,0x8F,0x18,0xF1,0xE1,0x00,0x00,0xFF,0x07,0x78,0xE0,0xFF,0x00,0x00,
0xFF,0x61,0x60,0x61,0xFF,0x00,0x00,0xFF,0x3E,0x77,0xC1,0x80,0x00,0x00,0xFF,0x18,
0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,0x04,0x04,
0x04,0x08,0x08,0x08,0x11,0x11,0x11,0x11,0xF1,0xE1,0xE0,0x60,0x20,0x20,0x20,0x21,
0xA1,0xA1,0xA1,0xE0,0xF0,0xD0,0x70,0xF0,0xF8,0x18,0x18,0x0C,0x06,0x02,0x01,0x01,
0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x01,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,
0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x01,0x01,
0x01,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0xC0,0x70,0x78,0x3E,0x0F,0x0F,0x07,0x00,0x00,0x80,0x80,0x86,
0x8F,0xCF,0xCF,0xCF,0xD9,0xF9,0x78,0x1F,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x40,0xC0,0x80,0x00,0x00,0x00,
0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x80,0x40,0x20,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0xF8,0xFE,0xDE,0xF9,0x06,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x0F,0x0F,
0x8D,0x8D,0xC8,0xFD,0xE3,0xC1,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,
0x80,0x80,0xC0,0xC0,0xC0,0xA0,0xA0,0xA0,0x90,0x90,0x90,0xF0,0xF0,0x68,0x68,0x08,
0x0C,0x04,0x1C,0x3C,0x3C,0x34,0x24,0x64,0x64,0x64,0x34,0x3C,0x3C,0x1C,0x08,0x08,
0x08,0x90,0xD0,0xF0,0xE0,0x60,0x40,0x40,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x02,0x80,0x80,
0x60,0xF0,0xFF,0xFF,0x3C,0x00,0x00,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x03,0x07,0x0E,0x09,0x3A,0x74,0x78,0x40,0xB0,0xB0,0xC0,0x80,0x00,0x00,0x80,
0x81,0x83,0x07,0x07,0x06,0x0E,0x0C,0x0C,0x0C,0x0E,0x0F,0x07,0x07,0x01,0x00,0x00,
0x00,0x00,0x80,0x81,0x01,0x81,0xC1,0xC1,0xF1,0x31,0x71,0x70,0x78,0x38,0x30,0x3C,
0x0C,0x0A,0x0E,0x04,0x04,0x04,0x04,0x04,0x06,0x06,0x06,0x0A,0x0C,0x0C,0x08,0x00,
0x38,0x70,0x41,0x73,0x03,0xC3,0x86,0x86,0x82,0x83,0x03,0x03,0x03,0x02,0x02,0x3E,
0x3E,0x7C,0x74,0x74,0x74,0x74,0xF4,0xFC,0xBC,0xBE,0xCE,0x72,0x71,0x79,0x3E,0x0E,
0x0F,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x05,0x07,0x07,0x07,
0x07,0x06,0x06,0x06,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x06,0x06,
0x06,0x07,0x07,0x05,0x05,0x05,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x05,0x05,0x07,0x07,0x07,0x07,0x06,0x06,
0x06,0x06,0x06,0x07,0x07,0x07,0x07,0x07,0x05,0x04,0x04,0x04,0x04,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const unsigned char SNAKE_CELL[] = {0b11111111,
                                0b11111111,
                                0b11111111,
                                0b11111111,
                                0b11111111,
                                0b11111111,
                                0b11111111,
                                0b11111111};
const unsigned char SNAKE_HEAD_U[] = {0x07,0x3F,0x73,0xBF,0xBF,0x73,0x3F,0x07};
const unsigned char SNAKE_HEAD_D[] = {0xE0,0xFC,0xCE,0xFD,0xFD,0xCE,0xFC,0xE0};
const unsigned char SNAKE_HEAD_R[] = {0xFF,0xFF,0xDB,0x5A,0x7E,0x7E,0x24,0x18};
const unsigned char SNAKE_HEAD_L[] = {0x18,0x24,0x7E,0x7E,0x5A,0xDB,0xFF,0xFF};
const unsigned char FOOD_CELL[] = {0x18,0x66,0x42,0x81,0x81,0x42,0x66,0x18};
/*const unsigned char FOOD_CELL[] = {0b11111111,
                                0b10000001,
                                0b10000001,
                                0b10000001,
                                0b10000001,
                                0b10000001,
                                0b10000001,
                                0b11111111};*/

const unsigned char BLANK_CELL[] = {0b00000000,
                                0b00000000,
                                0b00000000,
                                0b00000000,
                                0b00000000,
                                0b00000000,
                                0b00000000,
                                0b00000000};
/******************************************************************************
 * Function prototypes
 ******************************************************************************/
void Initial(void);         // Function to initialize hardware and interrupts
void DisplayStartScreen(void);
unsigned char ReadCell(char x, char y);
void DrawCell(char x, char y, char type);
void EraseCell(char x, char y);
void MoveSnake(char dir);
void CheckKey(void);
void UpdateGLCD(void);
void GenerateFood(void);
void HiPriISR(void);
void LoPriISR(void);
void TMR0handler(void);     // Interrupt handler for TMR0

#pragma code highVector=0x08
void atHighVector(void)
{
 _asm GOTO HiPriISR _endasm
}
#pragma code

#pragma code lowVector=0x18
void atLowVector(void)
{
 _asm GOTO LoPriISR _endasm
}
#pragma code

/******************************************************************************
 * main()
 *
 * This subroutine loops forever after calling Initial(). In the loop, the
 * GLCD is refreshed and if game over, display score and reset the board.
 ******************************************************************************/
void main() {
     Initial();                 // Initialize everything
      while(1) {
          CheckKey();
     }
}

/******************************************************************************
 * Initial()
 *
 * This subroutine performs all initializations of variables and registers.
 * It enables TMR0 and sets CCP0 for compare if desired, and enables LoPri 
 * interrupts for both.
 ******************************************************************************/
void Initial() {
    // Configure the IO ports
    TRISC  = 0b00000100;
    LATC = 0x00;
    
    //Set GLCD lines as outputs (for sending commands to GLCD)
    GLCD_CS0_TRIS = 0;//chip select 0
    GLCD_CS1_TRIS = 0;//chip select 1
    GLCD_RS_TRIS = 0;//
    GLCD_RW_TRIS = 0;//Read/Write
    GLCD_E_TRIS = 0;//Enable
    GLCD_RST_TRIS = 0;//
    GLCD_DATA_TRIS = 0;//Data byte (for reading/writing)
    
    InitGLCD();//initialize GLCD
    ClearGLCD();//Clear GLCD

    //Switch initialization
    TRISEbits.TRISE2 = 1;//DOWN button
    TRISEbits.TRISE3 = 1;//UP button
    TRISJbits.TRISJ7 = 1;//LEFT button
    TRISJbits.TRISJ3 = 1;//RIGHT button
    
    DisplayStartScreen();//display bitmap start screen until user presses button
    
    ClearGLCD();//clear GLCD again before starting game
    Snake.X[2] = 3;//initialize snake
    Snake.Y[2] = 4;
    Snake.X[1] = 4;
    Snake.Y[1] = 4;
    Snake.X[0] = 5;
    Snake.Y[0] = 4;
    Snake.Length = 4;
    srand ( TMR0L );//initializes random number generator to use TMR0L as seed
    GenerateFood();//generate random Food cell location to start
    DrawCell(Snake.X[0],Snake.Y[0],4);//draw snake head (going right)
    DrawCell(Snake.X[1],Snake.Y[1],1);//draw snake body cell
    DrawCell(Snake.X[2],Snake.Y[2],1);
    DrawCell(Food.X,Food.Y,0);//draw food cell
    

    // Initializing TMR0
    T0CON = 0x03;                   // 00000100: 16 bit with 32 prescaler
    TMR0L = 0;                      // Clearing TMR0 registers
    TMR0H = 0;                      // Clear high register if used

    // Configuring Interrupts
    RCONbits.IPEN = 1;              // Enable priority levels
    INTCON2bits.TMR0IP = 0;         // Assign low priority to TMR0 interrupt

    INTCONbits.TMR0IE = 1;          // Enable TMR0 interrupts
    INTCONbits.GIEL = 1;            // Enable low-priority interrupts to CPU
    INTCONbits.GIEH = 1;            // Enable all interrupts

    T0CONbits.TMR0ON = 1;           // Turning on TMR0
}

/******************************************************************************
 * DisplayStartScreen()
 *
 * This subroutine displays the startup screen for the game, and it waits until
 * the user presses a button, which then starts the game.
 ******************************************************************************/
void DisplayStartScreen(){
    unsigned char dispCount;
    unsigned char pgCnt;
    char buttonPressed = 0;
    unsigned int byteCnt = 0;
    for (pgCnt = 0;pgCnt<8;pgCnt++){//for each row in the GLCD...
        for (dispCount=0;dispCount<128;dispCount++){//and for each x location...
            SetCursor(dispCount,pgCnt);//set GLCD cursor
            WriteData(START_SCREEN[byteCnt++]);//write bitmap to GLCD
        }
    }
    while(!buttonPressed){//while no buttons are pressed...
        if(PORTEbits.RE3 || PORTEbits.RE2 || PORTJbits.RJ7 || PORTJbits.RJ3){
            buttonPressed = 1;//check each button and upon press begin game
        }
    }
    Delay10KTCYx(200);//delay 0.5s = 2000000 inst cycles
}

/******************************************************************************
 * ReadCell()
 *
 * This subroutine reads either a snake cell or a clear cell from the GLCD with
 * an input for the x and y position to read and returns the type of cell read.
 * type = 1(i.e. True) --> Snake Cell, type = 0(i.e. False) --> Blank Cell
 ******************************************************************************/
unsigned char ReadCell(char x, char y){
    char readtype = 0;//readtype = 0 if clear cell, readtype = 1 if snake
    unsigned char i;
    char cnt=0;
    if (fromInit<3){//only read next cell after start of movement of snake
        fromInit++;
        return readtype;
    }
    for(i=0;i<8;i++) {
        SetCursor((x*8)+i,y);
        //ReadData();
        READ_CELL[i] = ReadData();
        if (READ_CELL[i] == 0b11110111){//GLCD reads a snake cell as 0b11110111
            cnt++;//each time it reads one byte of a snake cell inc count
        }
    }
    //if read cell = snake cell, then set type correctly. else keep as 0 (blank)
    if (cnt >= 3){//if three bytes were read as a snake cell, set type correctly
        readtype = 1;
    }
    return readtype;
}


/******************************************************************************
 * DrawCell()
 *
 * This subroutine draws either a snake cell or a food cell to the GLCD with an
 * input for the x and y position along with the indication of the type.
 * type = 2-5 (Snake Head) type = 1 (Snake Cell), type = 0 (Food Cell)
 * x and y are used for the snake cell whereas the food cell is placed randomly
 ******************************************************************************/
void DrawCell(char x, char y, char type) {
    unsigned char i;
    if(type>=1) {
        for(i=0;i<8;i++){
            SetCursor((x*8)+i, y);
            if(type==1){//type 1 = snake body cell
                WriteData(SNAKE_CELL[i]);
            }
            else if(type==2){//type 2 = snake head going up
                WriteData(SNAKE_HEAD_U[i]);
            }
            else if(type==3){//type 3 = snake head going down
                WriteData(SNAKE_HEAD_D[i]);
            }
            else if(type==4){//type 4 = snake head going right
                WriteData(SNAKE_HEAD_R[i]);
            }
            else if(type==5){//type 5 = snake head going left
                WriteData(SNAKE_HEAD_L[i]);
            }
            
        }
    }
    else {//type 0 = food cell
        for(i=0;i<8;i++){
            SetCursor((x*8)+i, y);
            WriteData(FOOD_CELL[i]);
        }
    }   
}

/******************************************************************************
 * EraseCell()
 *
 * This subroutine clears cell on the GLCD given an input for the x and y position 
 ******************************************************************************/
void EraseCell(char x, char y) {
    unsigned char i;
    for(i=0;i<8;i++){
        SetCursor((x*8)+i, y);
        WriteData(BLANK_CELL[i]);
    } 
}

/******************************************************************************
 * MoveSnake()
 *
 * This subroutine performs the logic necessary to move the snake by adding to
 * the head and removing from the tail. 
 ******************************************************************************/
void MoveSnake(char dir) {
    unsigned char i;
    unsigned char j;
    EraseCell(Snake.X[Snake.Length-1],Snake.Y[Snake.Length-1]);//erase tail cell
    
    for(i=Snake.Length - 1; i>0; i--) {
        Snake.X[i] = Snake.X[i-1];
        Snake.Y[i] = Snake.Y[i-1];
        DrawCell(Snake.X[i],Snake.Y[i],1);//draw each body cell of the snake
    }
    switch(dir) {//check dir snake is going and draw cells accordingly
        case 'R':
            //if new snakes head coords = a current body coordinate--> game over
            if (ReadCell(++Snake.X[0],Snake.Y[0])==1){
                Reset();
            }
            DrawCell(Snake.X[0],Snake.Y[0],4);//draw new snake head going right
            break;
        case 'L':
            if (ReadCell(--Snake.X[0],Snake.Y[0])==1){
                Reset();
            }
            DrawCell(Snake.X[0],Snake.Y[0],5);//draw new snake head going left
            break;
        case 'U':
            if (ReadCell(Snake.X[0],++Snake.Y[0])==1){
                Reset();
            }
            DrawCell(Snake.X[0],Snake.Y[0],2);//draw new snake head going up
            break;
        case 'D':
            if (ReadCell(Snake.X[0],--Snake.Y[0])==1){
                Reset();
            }
            DrawCell(Snake.X[0],Snake.Y[0],3);//Draw new snake head going down
            break;
        default:
            if (ReadCell(++Snake.X[0],Snake.Y[0])){
                Reset();
            }
            DrawCell(Snake.X[0],Snake.Y[0],1);
            break;
    }
    //if new snake head coords = current food coords, then gen. new food cell
    if(Snake.X[0] == Food.X && Snake.Y[0] == Food.Y) {
        GenerateFood();//calls function to generate random coords for food cell
        DrawCell(Food.X,Food.Y,0);//draws new food cell
        Snake.Length++;//increase snakes length by one
        //now draw each snake cell until end of snake and add a body cell at end
        for(i=Snake.Length - 1; i>1; i--) {
            Snake.X[i] = Snake.X[i-1];
            Snake.Y[i] = Snake.Y[i-1];
            DrawCell(Snake.X[i],Snake.Y[i],1);
        }
    
    }
    
    //if snake head hits coordinate outside GLCD limits, reset game (game over)
    if(Snake.X[0] > 15) {
        Reset();
    }

    if(Snake.X[0] < 0) {
        Reset();
    }
    if(Snake.Y[0] > 7) {
        Reset();
    }
    if(Snake.Y[0] < 0) {
        Reset();
    }
}

/******************************************************************************
 * CheckKey()
 *
 * This subroutine checks which button was pressed and returns a value.
 ******************************************************************************/
void CheckKey() {//checks buttons and returns current direction
	
	if (PORTEbits.RE3 == 1 && DIRECTION != 'U' && DIRECTION != 'D') {
		Delay10KTCYx(100);//switch debounce
        DIRECTION = 'D';//update direction (down)
	}
	else if (PORTEbits.RE2 == 1 && DIRECTION != 'D' && DIRECTION != 'U') {
		Delay10KTCYx(100);//switch debounce
        DIRECTION = 'U';//Up
	}
	else if (PORTJbits.RJ7 == 1 && DIRECTION != 'L' && DIRECTION != 'R') {
		Delay10KTCYx(100); //switch debounce
        DIRECTION = 'L';//Left
	}
	else if (PORTJbits.RJ3 == 1 && DIRECTION != 'R' && DIRECTION != 'L') {
		Delay10KTCYx(100);//switch debounce 
        DIRECTION = 'R';//Right
	}

}

/******************************************************************************
 * GenerateFood()
 *
 * This subroutine generates a random food location, and also checks coordinates
 * of each snake cell to make sure food does not spawn inside snake.
 ******************************************************************************/
void GenerateFood(){
    RandX = (int) rand()%16;
    RandY = (int) rand()%8;
    /*below while loop continues randomly generating food cells until they are
     not generated inside the current snake body*/
    while(ReadCell(RandX,RandY)){
        RandX = (int) rand()%16;
        RandY = (int) rand()%8;
    }
    
    Food.X = RandX;
    Food.Y = RandY;
    
}

/******************************************************************************
 * HiPriISR interrupt service routine
 *
 * Included to show form, does nothing
 ******************************************************************************/
#pragma interrupt HiPriISR
void HiPriISR() {
    
}   // Supports retfie FAST automatically

/******************************************************************************
 * LoPriISR interrupt service routine
 *
 * Calls the individual interrupt routines. A flag is set and cleared if a
 * button is pressed.
 ******************************************************************************/
#pragma interruptlow LoPriISR 
void LoPriISR() {
    while(1) {
        if( INTCONbits.TMR0IF ) { //Check if TMR0 interrupt flag is set
            MoveSnake(DIRECTION);//updates snake cells at constant rate
            TMR0handler();//clears TMR0IF and reloads TMR0
            continue;
        }
        break;
    }
}

/******************************************************************************
 * TMR0handler interrupt service routine.
 *
 * Handles GLCD refresh
 ******************************************************************************/
void TMR0handler() {
    LATCbits.LATC4 = ~LATCbits.LATC4; //Toggle RD4
    
    TMR0H = 0x0B;               // Reset TMR0H
    TMR0L = 0xDC;               // Reset TMR0L     

    INTCONbits.TMR0IF = 0;      //Clear flag and return to polling routine
}
