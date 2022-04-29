/*
 * File:   main.c
 * Author: RELATOR
 *
 * Created on 8.09.2021 9:25
 */

#include <xc.h>
#include <pic16f676.h>
#include "bitsmacros.h"

/*
 * Configuration bits
*/
#pragma config FOSC = INTRCIO  // Oscillator Selection bits (INTOSC oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config BOREN = OFF     // Brown-out Detect Enable bit
#pragma config CPD = OFF       // Data Code Protection bit
#pragma config CP = OFF        // Code Protection bit 
#pragma config MCLRE = OFF     // RA3/MCLR pin function select (RA3/MCLR pin function is digital I/O, MCLR internally tied to VDD)
#pragma config WDTE = OFF      // Watchdog Timer Enable bit
#pragma config PWRTE = OFF     // Power-up Timer Enable bit 

/*
 * Variables
*/
#define _XTAL_FREQ 4000000

// DIGIT 7-SEGMENT PINS:
#define A        RA2             // SEGMENT A
#define A_DIR    TRISA2
#define B        RC0             // SEGMENT B
#define B_DIR    TRISC0
#define C        RC1             // SEGMENT C
#define C_DIR    TRISC1
#define D        RC2             // SEGMENT D
#define D_DIR    TRISC2
#define E        RC3             // SEGMENT E
#define E_DIR    TRISC3
#define F        RC4             // SEGMENT F
#define F_DIR    TRISC4
#define G        RC5             // SEGMENT G
#define G_DIR    TRISC5

// 4-DIGITS COMMON KATODE PINS:
#define DD0      RA0             // DIGIT 0
#define DD0_DIR  TRISA0           
#define DD1      RA1             // DIGIT 1
#define DD1_DIR  TRISA1           
#define DD2      RA4             // DIGIT 2
#define DD2_DIR  TRISA4           
#define DD3      RA5             // DIGIT 3
#define DD3_DIR  TRISA5

//DYNAMIC INDICATION DELAY
#define DELAY    25

// GLOBAL VARS:
unsigned char digitValue;
unsigned short counterValue;

// Functions prototypes
void PIC_Init(void);
void TakeNumAndSetDisplay(unsigned short); 

void SetCharToPorts(unsigned char value);
void Test(void);

// Configure PIC:
void PIC_Init(void)
{
    INTCON = 0x00;         //turn off interrupts
    
    //turn off comparators
    CMCON = 0x00;          // turn off comparators
    VRCON = 0x00;          // vref control off
    
    //ADC module configuration off
    ADCON1 = 0b00000000;   // AD Clock select as Fosc/2
    ADCON0 = 0b00011100;   // set channel 7 and turn off ADC  
    ANSEL = 0x00;          // set all inputs for digital i/o
    
    //Configure All Digital IO ports as input
    TRISA = 0xFF;          // 1 = input; 0 = output
    TRISC = 0xFF;          // 1 = input; 0 = output
    
    WPUA = 0x00;           // Weak pulup PORT A off
    
    // 7 SEGMENT PINS DIRECTION
    A_DIR = 0;             // 0 = OUTPUT
    B_DIR = 0;             // 0 = OUTPUT
    C_DIR = 0;             // 0 = OUTPUT
    D_DIR = 0;             // 0 = OUTPUT
    E_DIR = 0;             // 0 = OUTPUT
    F_DIR = 0;             // 0 = OUTPUT
    G_DIR = 0;             // 0 = OUTPUT
    
    // DIGITS COM KATODE PINS DIRECTION
    DD0_DIR = 0;           // FIRST DIGIT
    DD1_DIR = 0;           // SECOND DIGIT
    DD2_DIR = 0;           // THIRD DIGIT
    DD3_DIR = 0;           // FOURTH DIGIT
    
}

void SetCharToPorts(unsigned char value){
    switch(value){
        
        // SET segments
        //            A           B           C           D          E           F           G 
        //  PORT A    0b00000100  
        //  PORT C                0b00000001  0b00000010  0b00000100 0b00001000  0b00010000  0b00100000

        // SET digits 
        //            3           2           1           0
        // PORT A     0b00100000  0b00010000  0b00000010  0b00000001
    
        case 0:
            //G=0
            PORTC &= ~0b00100000;
            //A=B=C=D=E=F=1;
            PORTA |= 0b00000100;
            PORTC |= 0b00011111;
            break;
        
        case 1:
            //A=D=E=F=G=0;
            PORTA &= ~0b00000100;
            PORTC &= ~0b00111100;
            //B=C=1;
            PORTC |= 0b00000011;
            break;
            
        case 2:
            C=F=0;
            A=B=D=E=G=1;
            break;
            
        case 3:
            E=F=0;
            A=B=C=D=G=1;
            break;
            
        case 4:
            A=D=E=0;
            B=C=F=G=1;
            break;
            
        case 5:
            B=E=0;
            A=C=D=F=G=1;
            break;
            
        case 6:
            B=0;
            A=C=D=E=F=G=1;
            break;
            
        case 7:
            D=E=F=G=0;
            A=B=C=1;
            break;
            
        case 8:
            A=B=C=D=E=F=G=1;
            break;
            
        case 9:
            E=0;
            A=B=C=D=F=G=1;
            break;
            
        default:
            A=B=C=D=E=F=G=0;
            break;
    }
}

void TakeNumAndSetDisplay(unsigned short value){
    //      D5  D4  D3  D2  D1
    //Set                   D1
    digitValue = value%10;
    SetCharToPorts(digitValue);
    SetBit(PORTA, 0);
    __delay_ms(DELAY);
    ClearBit(PORTA, 0);
    
    //Set               D2
    digitValue = (value/10)%10;
    SetCharToPorts(digitValue);
    SetBit(PORTA, 1);
    __delay_ms(DELAY);
    ClearBit(PORTA, 1);
    
    //Set           D3
    digitValue = (value/100)%10;
    SetCharToPorts(digitValue);
    SetBit(PORTA, 4);
    __delay_ms(DELAY);
    ClearBit(PORTA, 4);
    
    //Set       D4
    digitValue = (value/1000)%10;
    SetCharToPorts(digitValue);
    SetBit(PORTA, 5);
    __delay_ms(DELAY);
    ClearBit(PORTA, 5);
    
    /*
    //Set   D5
    digitValue = (value/10000)%10;
    */
    
}


void Test (void) {
    
    counterValue = 12345;
//    
//    // Set "8" digit
//    SetBit(PORTA, 2);
//    SetBit(PORTC, 0);
//    SetBit(PORTC, 1);
//    SetBit(PORTC, 2);
//    SetBit(PORTC, 3);
//    SetBit(PORTC, 4);
//    SetBit(PORTC, 5);
//    
    
    TakeNumAndSetDisplay(counterValue);
    
}

void main(void) 
{
    //Initialization PIC
    PIC_Init();
    
    while (1)
    {
        //Test
        Test();
        
    }
    return;
}
