/**
  UART2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    uart2.c

  @Summary
    This is the generated driver implementation file for the UART2 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for UART2.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ128GA202
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB             :  MPLAB X v5.45
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include <xc.h>
#include "uart2.h"
#include "math.h"
#include "string.h"
/**
  Section: UART2 APIs
*/

void UART2_Initialize(void)
{
/**    
     Set the UART2 module to the options selected in the user interface.
     Make sure to set LAT bit corresponding to TxPin as high before UART initialization
*/
    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U2MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; URXEN disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U2STA = 0b1010000000000000;
    // BaudRate = 9600; Frequency = 4000000 Hz; U2BRG 103; 
    U2BRG = 0x67;
    // ADMADDR 0; ADMMASK 0; 
    U2ADMD = 0x00;
    // T0PD 1 ETU; PTRCL T0; TXRPT Retransmits the error byte once; CONV Direct; SCEN disabled; 
    U2SCCON = 0x00;
    // TXRPTIF disabled; TXRPTIE disabled; WTCIF disabled; WTCIE disabled; PARIE disabled; GTCIF disabled; GTCIE disabled; RXRPTIE disabled; RXRPTIF disabled; 
    U2SCINT = 0x00;
    // GTC 0; 
    U2GTC = 0x00;
    // WTCL 0; 
    U2WTCL = 0x00;
    // WTCH 0; 
    U2WTCH = 0x00;
    IFS1bits.U2TXIF = 0;	// Clear the Transmit Interrupt Flag
    IPC7bits.U2TXIP = 1; // UART2 TX interrupt has interrupt priority 3-4th highest priority
    
	IEC1bits.U2TXIE = 0;	// Disable Transmit Interrupts
	IFS1bits.U2RXIF = 0;	// Clear the Recieve Interrupt Flag
	IPC7bits.U2RXIP = 4; //UART2 Rx interrupt has 2nd highest priority
    IEC1bits.U2RXIE = 0;	// Disable Recieve Interrupts
    
    U2MODEbits.UARTEN = 1;   // enabling UART ON bit
    U2STAbits.UTXEN = 1;
}

///// Xmit UART2: 
///// Displays 'DispData' on realterm 'repeatNo' of times using UART to PC. 
///// Adjust Baud on real term as per clock: 32kHz clock - Baud=300 // 500kHz clock - Baud=4800 

void XmitUART2(char CharNum, unsigned int repeatNo)
{	
	
	UART2_Initialize();	//Initialize UART2 module and turn it on
    
	while(repeatNo!=0) 
	{
		while(U2STAbits.UTXBF==1)	//Just loop here till the FIFO buffers have room for one more entry
		{
			// Idle();  //commented to try out serialplot app
		}	
		U2TXREG=CharNum;	//Move Data to be displayed in UART FIFO buffer
		repeatNo--;
	}
	while(U2STAbits.TRMT==0)	//Turn off UART2 upon transmission of last character; also can be Verified in interrupt subroutine U2TXInterrupt()
	{
		//Idle();
	}
	U2MODEbits.UARTEN = 0;	
//	LATBbits.LATB9=1;
	return;
}

// Displays 16 bit number in Hex form using UART2
void Disp2Hex(unsigned int DispData)   
{
    char i;
    char nib = 0x00;
    XmitUART2(' ',1);  // Disp Gap
    XmitUART2('0',1);  // Disp Hex notation 0x
    XmitUART2('x',1);
    
    for (i=3; i>=0; i--)
    {
        nib = ((DispData >> (4*i)) & 0x000F);
        if (nib >= 0x0A)
        {
            nib = nib +0x37;  //For Hex values A-F
        }
        else 
        {
            nib = nib+0x30;  //For hex values 0-9
        }
        XmitUART2(nib,1);
    }
    
    XmitUART2(' ',1);
    DispData = 0x0000;  // Clear DispData
    return;
}

void Disp2BCDDigit(uint8_t digit)
{
    uint8_t nibble = digit & 0x0F; // get the first 4 bits
    XmitUART2(nibble + 0x30, 1);
    
    return;
}

void Disp2Hex64(uint64_t DispData64)
{
    char i;
    char nib = 0x00;
    XmitUART2(' ',1);  // Disp Gap
    XmitUART2('0',1);  // Disp Hex notation 0x
    XmitUART2('x',1);
    
    for (i=15; i>=0; i--)
    {
        nib = ((DispData64 >> (4*i)) & 0x000F);
        if (nib >= 0x0A)
        {
            nib = nib +0x37;  //For Hex values A-F
        }
        else 
        {
            nib = nib+0x30;  //For hex values 0-9
        }
        XmitUART2(nib,1);
    }
    
    XmitUART2(' ',1);
    DispData64 = 0x00000000;  // Clear DispData
    return;
}
void Disp2Hex32(unsigned long int DispData32)   // Displays 32 bit number in Hex form using UART2
{
    char i;
    char nib = 0x00;
    XmitUART2(' ',1);  // Disp Gap
    XmitUART2('0',1);  // Disp Hex notation 0x
    XmitUART2('x',1);
    
    for (i=7; i>=0; i--)
    {
        nib = ((DispData32 >> (4*i)) & 0x000F);
        if (nib >= 0x0A)
        {
            nib = nib +0x37;  //For Hex values A-F
        }
        else 
        {
            nib = nib+0x30;  //For hex values 0-9
        }
        XmitUART2(nib,1);
    }
    
    XmitUART2(' ',1);
    DispData32 = 0x00000000;  // Clear DispData
    return;
}

uint8_t UART2_Read(void)
{
    while(!(U2STAbits.URXDA == 1))
    {
        
    }

    if ((U2STAbits.OERR == 1))
    {
        U2STAbits.OERR = 0;
    }
    
    return U2RXREG;
}

// Displays 16 bit unsigned in in decimal form
void Disp2Dec(uint16_t Dec_num)
{
    uint8_t rem;  //remainder in div by 10
    uint16_t quot; 
    uint8_t ctr = 0;  //counter
    XmitUART2(' ',1);  // Disp Gap
    while(ctr<5)
    {
        quot = Dec_num/(pow(10,(4-ctr)));
        rem = quot%10;
        XmitUART2(rem + 0x30 , 1);
        ctr = ctr + 1;
    }
    XmitUART2(' ',1);  // Disp Gap
    // XmitUART2('\n',1);  // new line
    // XmitUART2('\r',1);  // carriage return
   
    return;
}

void Disp2String(char *str) //Displays String of characters
{
    unsigned int i;
   // XmitUART2(0x0A,2);  //LF
   // XmitUART2(0x0D,1);  //CR 
    for (i=0; i<= strlen(str); i++)
    {
          
        XmitUART2(str[i],1);
    }
    // XmitUART2(0x0A,2);  //LF
    // XmitUART2(0x0D,1);  //CR 
    
    return;
}

void UART2_Write(uint8_t txData)
{
    while(U2STAbits.UTXBF == 1)
    {
        
    }

    U2TXREG = txData;    // Write the data byte to the USART.
}



bool UART2_IsRxReady(void)
{
    return U2STAbits.URXDA;
}

bool UART2_IsTxReady(void)
{
    return ((!U2STAbits.UTXBF) && U2STAbits.UTXEN );
}

bool UART2_IsTxDone(void)
{
    return U2STAbits.TRMT;
}

void __attribute__ ((interrupt, no_auto_psv)) _U2RXInterrupt(void) {
//	LATA = U2RXREG;
	IFS1bits.U2RXIF = 0;
}
void __attribute__ ((interrupt, no_auto_psv)) _U2TXInterrupt(void) {
	IFS1bits.U2TXIF = 0;
}


/*******************************************************************************

  !!! Deprecated API !!!
  !!! These functions will not be supported in future releases !!!

*******************************************************************************/

uint16_t __attribute__((deprecated)) UART2_StatusGet (void)
{
    return U2STA;
}

void __attribute__((deprecated)) UART2_Enable(void)
{
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;
}

void __attribute__((deprecated)) UART2_Disable(void)
{
    U2MODEbits.UARTEN = 0;
    U2STAbits.UTXEN = 0;
}
