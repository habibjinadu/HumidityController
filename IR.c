/*
 This library implements the functions needed to send an IR signal that conforms
 * to the NEC IR protocol.
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "IR.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/pin_manager.h"

#define FCY 4000000UL       // define the instruction clock as 4MHZ
#include <libpic30.h>



//-----------------------------------------------------------------------------------------------------------//

//This function will create the 38KHz carrier for the IR signals
void carrier(uint16_t clkCycles){
    //This line sets the number of 38KHz cycles to occur within a specified time
    int waitTime = (clkCycles)/14;
    int i =0;
    // This loop sets the period of the 38KHz clock cycle and flips between 0 and 1 each half period
    for(i; i < waitTime;i++)
    {
     __delay32(30);

     IR_TX_Toggle(); // set the IR_TX pin high
    }
}

//-----------------------------------------------------------------------------------------------------------//

//This function creates the one bit as specified by the samsung protocol
void oneBit(){
    //creates the carrier for 0.56ms
    carrier(560);
    //goes low for 1.69ms
//     LATBbits.LATB9 = 0;
    IR_TX_SetLow(); // set the IR_TX pin low
    __delay32(6760);
}


//This function creates the zero bit as specified by the samsung protocol
void zeroBit(){
    //creates the carrier for 0.56ms
     carrier(560);
     //goes low for 0.56ms
//     LATBbits.LATB9 = 0;
     IR_TX_SetLow(); // set the IR_TX pin low
    __delay32(2240);
}

//This function creates the start bit as specified for the samsung protocol
void startBit(){
    //creates the carrier for 9ms
    carrier(9000);
    //goes low for 4.5ms 
//    LATBbits.LATB9 = 0;
    IR_TX_SetLow(); // set the IR_TX pin low
    __delay32(18000);
}

//----------------------------------------------------------------------------------------------------------------//


/**
 * sends the bit value in the array via Infrared
 * @param cmdArray boolean array that represents the bit code
 */
void txIRCommand(bool *cmdArray)
{
    //Begins with start bit
    startBit();
    int i=0;
    //Sends each of the bits as specified by the arrays,
    for(i ; i<MAX_IR_BIT_SIZE;i++){
        if(cmdArray[i])
            oneBit();
        else
            zeroBit();      
    }
    //ends the transmission with a zero bit as a stop bit
    zeroBit();
}

/**
 * takes in a custom IR hex command and creates the bit array that is then
 * transmitted VIA the txIRCommand function
 * @param command Hex command to be transmitted via IR
 */
void sendMessage(INT_MSG_SIZE command)
{
    bool bitArray[MAX_IR_BIT_SIZE] = {0}; // create empty bit array
    int index = MAX_IR_BIT_SIZE - 1; // create the loop counter index
    for (;index>=0; index--)
    {
        bitArray[index] = command & 1; // isolate LSB and store it in bitArray
        command>>=1; // shift the command by 1
    }
    
    txIRCommand(bitArray); // transmit the bit array via infrared
}
//--------------------------------------------------------------------------------------------------------------------/
