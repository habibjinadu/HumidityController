/*
 * File:   tmr2.c
 * Author: adeso
 *
 * Created on May 22, 2021, 8:50 PM
 */


#include "xc.h"

void TMR2_Initialize (void)
{
    //TMR2 0; 
    TMR2 = 0x00;
    //Period = 0 s; Frequency = 4000000 Hz; PR2 0; 
    PR2 = 0x00;
    //TCKPS 1:1; T32 16 Bit; TON disabled; TSIDL disabled; TCS FOSC/2; TECS SOSC; TGATE disabled; 
    T2CON = 0x0000;
    

    IFS0bits.T2IF = 0; // clear the interrrupt flag
    IEC0bits.T2IE = 1; // enable timer 2 interrupt
    
    // set the timer2 interrupt priority to level 5 default is 2
    IPC1bits.T2IP = 5; 

}
