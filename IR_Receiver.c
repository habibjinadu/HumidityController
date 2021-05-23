/*
 * File:   IR_Receiver.c
 * Author: adeso
 *
 * Created on May 22, 2021, 8:03 PM
 */


#include "xc.h"
#include "IR_Receiver.h"
//#include "ChangeClk.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/pin_manager.h"




uint16_t start = 0; // start is 1 when a start signal has been detected, 0 when a start signal has not been detected.
uint16_t TMR_hi_duration = 0; // To keep track of the hi time
uint16_t TMR_lo_duration = 0; // To keep track of the lo time
uint16_t TMR2_time_out_duration = TIMEOUT_CYCLES; //signal time_out duration for decoding 20000 cycles(5ms for Samsung TVs)
uint16_t ready_1 = 0; // variable to keep track of the correct low input duration in the start bit
uint16_t ready_2 = 0; // variable to keep track of the correct hi input duration in the start bit
INT_M_SIZE message = 0; //keeps track of the IR message
uint16_t message_bit_position = MAX_BIT_SIZE; // 32 for samsung, 64 for seville
uint16_t decode_bit =  0; // decode flag
uint16_t start_duration = 0; // Keeps track of the start duration

extern int CN_interrupt_flag = 0; // Keeps track of the CN interrupt trigger

uint16_t T2_interrupt_flag = 0; // Keeps track of the T2 interrupt trigger


INT_M_SIZE decode_IR()
{   do 
    {
        if (CN_interrupt_flag == 1)
        {
            measure_pulse_width(); // measure and store the pulse width of the IR signal. 
            CN_interrupt_flag = 0; // Set the interrupt flag back to 0.
        }

        if (T2_interrupt_flag == 1)
        {
            start = 0; // Stop looking for a signal.
            ready_1 = 0;// set ready_1 to 0
            ready_2 = 0;// set ready_2 to 0
            message = 0; // Set message back to 0
            message_bit_position = MAX_BIT_SIZE; // reset message_bit_index to 32
            TMR_hi_duration = 0; // Set lo duration back to 0
            TMR_lo_duration = 0; // Set hi duration back to 0
            decode_bit = 0; // Set decode_bit back to 0
            T2_interrupt_flag = 0; // set T2_interrupt flag back to 0;
            CN_interrupt_flag = 0; // set the CN_interrupt_flag back to zero
        }

        if (start == 0 && ready_1 == 0 && ready_2 == 0)
        {
            //LATBbits.LATB8 = 0; //Led lo
            
        } 
        else if (decode_bit == 1 && message_bit_position >= 0)
        {       
            update_message(); // update our message as the receive signal is being decoded
        }
        else if (decode_bit == 0 && message_bit_position == 0)
        {

            start = 0; // Stop looking for a signal.
            ready_1 = 0;// set ready_1 to 0
            ready_2 = 0;// set ready_2 to 0
            message_bit_position = MAX_BIT_SIZE; // reset message_bit_index to 32
            TMR_hi_duration = 0; // Set lo duration back to 0
            TMR_lo_duration = 0; // Set hi duration back to 0
            T2CONbits.TON = 0; // Stop timer 2
            TMR2 = 0; // Set TMR2 to 0

        }
        else if (start == 1)
        {

            //LATBbits.LATB8 = 1; // Turn on led

        }
    }while (start|ready_1|ready_2); // while start or ready_1 or ready_2 is 1
    
    return message;
}

// Displays a message to the PC
void display_message()
{
        Disp2String("\n\r");
        Disp2Hex64(message);
        

        
        message_bit_position = MAX_BIT_SIZE; // reset message_bit_index to 32
        message = 0; // clear message
        TMR_hi_duration = 0; // Set lo duration back to 0
        TMR_lo_duration = 0; // Set hi duration back to 0

}

// Starts measuring the pulse duration
void measure_pulse_width()
{
    
    
    if(IR_RX_GetValue() == 0 && start == 0 && ready_1 == 0 && ready_2 == 0)// If the IR receiver is lo and pin_state == 1
    {   
        PR2 = TMR2_time_out_duration; // set PR2 to the time_out_duration      
        ready_1 = 1; // begin the flag sequence
        T2CONbits.TON = 0; // Stop timer
        TMR2 = 0; // Set TMR2 to 0
        T2CONbits.TON = 1; // Start timer
    }   
   else if (IR_RX_GetValue() == 1 && start == 0 && ready_1 == 1 && ready_2 == 0) // If the IR receiver is lo and pin_state == 0
    {

        start_duration = TMR2; // Store start duration
        T2CONbits.TON = 0; // Stop timer
        TMR2 = 0; // Set TMR2 to 0
        T2CONbits.TON = 1; // Start timer
        // if start duration is within 36000 cycles (9ms)
        if ((start_duration > (START_BIT_LO_CYCLES - START_OFFSET_ERR)) &&
                (start_duration < (START_BIT_LO_CYCLES + 2*OFFSET_ERR))) 
        {
            ready_2 = 1; // start 2nd half start bit verification.
            //ready_2 = 0; // initial message received.
        }
        else
        {

            ready_2 = 0; // stop decoding
            ready_1 = 0; // incorrect initial signal, set ready_1 back to 0
        }
    }
   else if (IR_RX_GetValue() == 0 && start == 0 && ready_1 == 1 && ready_2 == 1)
   {

       start_duration = TMR2; // store the 2nd half of the in start_duration
       T2CONbits.TON = 0; // Stop timer
       TMR2 = 0; // Set TMR2 to 0
       T2CONbits.TON = 1; // Start timer
       
       // if start duration is within 18000 cycles (4.5ms)
       if ((start_duration > (START_BIT_HI_CYCLES - 2*OFFSET_ERR)) && 
               (start_duration < (START_BIT_HI_CYCLES + 2*OFFSET_ERR))) 
       {
           start = 1;
           ready_1 = 0;
           ready_2 = 0;
           
       }
       else
       {
           start = 0;
           ready_1 = 0;
           ready_2 = 0;
       }
   }
    else if (IR_RX_GetValue() == 1 && start == 1) // If decoding is active
    {
        TMR_lo_duration = TMR2; // save lo duration
        T2CONbits.TON = 0; // Stop timer
        TMR2 = 0; // Set TMR2 to 0
        T2CONbits.TON = 1; // Start timer
        
        
    }
    else if (IR_RX_GetValue() == 0 && start == 1)
    {
        TMR_hi_duration = TMR2; //save lo duration
        T2CONbits.TON = 0; // Stop timer
        TMR2 = 0; // Set TMR2 to 0
        T2CONbits.TON = 1; // Start timer
        decode_bit = 1; // set the decode_bit flag to 1
    }
    
    
}

// Updates the message with the detected bit.
void update_message()
{
    message_bit_position -= 1; // decrease the index by 1
    message |= determine_bit(TMR_hi_duration, TMR_lo_duration) << message_bit_position; // Set the bit in message specified by message_bit_position
    decode_bit = 0; //set decode_bit back to 0
}
// Determine whether the pulse represents a 1 bit, or 0 bit
INT_M_SIZE determine_bit (uint16_t hi_cycles, uint16_t lo_cycles) 
{

    if (    (hi_cycles > (ONE_BIT_HI_CYCLES - OFFSET_ERR)) && 
            (hi_cycles < (ONE_BIT_HI_CYCLES + OFFSET_ERR)) && 
            (lo_cycles > (ONE_BIT_LO_CYCLES - OFFSET_ERR)) && 
            (lo_cycles < (ONE_BIT_LO_CYCLES + OFFSET_ERR))) // if lo time is 2240 cycles (560us) and hi time is 7760 cycles (1690us)
        return 1;
    else if ((hi_cycles > (ZERO_BIT_HI_CYCLES - OFFSET_ERR)) && 
            (hi_cycles < (ZERO_BIT_HI_CYCLES + OFFSET_ERR)) && 
            (lo_cycles > (ZERO_BIT_LO_CYCLES - OFFSET_ERR)) && 
            (lo_cycles < (ZERO_BIT_LO_CYCLES + OFFSET_ERR))) // if lo time is 2240 cycles (560us) and hi time is 2240 cycles (560us)
        return 0;
    else
        return 0;
}



// The is the CN interrupt which is called when a CN pin has changed value.
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void){
    
    CN_interrupt_flag = 1; // Set the interrupt flag to 1.

     IFS1bits.CNIF = 0; // clear the CN interrupt flag bit.
}

//This is the timer2 interrupt which is called when the TMR2 counter has reached 
// the value in PR2
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void){
    
    T2_interrupt_flag = 1; // set T2 interrupt flag to 1 (a timeout happned)

    T2CONbits.TON = 0;//Turns off the timer
    IFS0bits.T2IF = 0; //Clears the timer interrupt flag
        
}