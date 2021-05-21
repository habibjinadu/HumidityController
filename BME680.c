/*
 * File:   BME680.c
 * Author: adeso
 *
 * Created on May 12, 2021, 9:52 AM
 */


#include "xc.h"
#include <p24FJ128GA202.h>
#include "mcc_generated_files/i2c1_driver.h"
//#include "uart2.h"
#include "BME680.h"
#include "mcc_generated_files/uart2.h" // needs the i2c1 driver to work
#include <stdlib.h>
#include <math.h> // include the math library to do exponentials
//#include "bme68x_defs.h"
//#include "bme68x.h"
/*-----------NEEDED FOR DELAY--------------*/
#ifndef FCY
#define FCY 4000000UL       // define the instruction clock as 4MHZ
#endif
#include <libpic30.h>

#define BME680_RETRY_MAX        100  // define the retry count
#define BME680_WRITE_ADDRESS    0xEE // slave device address in write mode
#define BME680_READ_ADDRESS     0xEF // slave device address in read mode
#define BME680_DEVICE_TIMEOUT   50   // define slave timeout



/**
 * Sends a write command the the BME680
 */
void BME680_SendInitialAddress(uint8_t value)
{
    // write to the BME680
    i2c1_enableIRQ(); //enable interrupts
    i2c1_driver_driver_open(); // open the I2C bus
    status = I2C_START_MESSAGE; // change the status to start the message.
    i2c1_driver_start(); // start the I2C bus
    i2c1_waitForEvent(0); // wait for an interrupt event, no timeout
    if (status == I2C_TRANSMIT_MESSAGE)
    {
        
        i2c1_driver_TXData(value); // transmit the message
        i2c1_waitForEvent(0); // wait for event, no time out
        
    }
    
}

/**
 * writes the data to a register in the BME 680
 * @param regAddress pointer to the 8-bit address of the register
 * @param data pointer to the 8-bit data you wish to write
 */
void BME680_WriteData(uint8_t* regAddress, uint8_t* data)
{
    BME680_SendInitialAddress(BME680_WRITE_ADDRESS); // write to the address of the BME
    if (i2c1_driver_isNACK() == 0) // if an ACK was sent by the slave
    {// write to the BME680
        BME680_Write(regAddress); // write the register address
        if (i2c1_driver_isNACK() == 0) // if an ACK was sent by the slave
        {
            BME680_Write(data); // write the data to the register
        } 
    }
    else
    {
        status = I2C_SLAVE_NACK; // set the error flag
    }
    BME680_StopTransmission(); // stop the i2c module
    
    
}

/**
 * sends a value though the I2C bus
 * @param value pointer to the 8-bit value you want to send
 */
void BME680_Write(uint8_t* value)
{
    status = I2C_TRANSMIT_MESSAGE; // set the status to transmit a message
    i2c1_driver_TXData(*value); // transmit the register address
    i2c1_waitForEvent(0); // wait for an interrupt event, no timeout
}

char BME680_ReadFrom(uint8_t* reg)
{
    BME680_SendInitialAddress(BME680_WRITE_ADDRESS); // start by sending a write
    char receivedData = 0;
    if (i2c1_driver_isNACK() == 0) // if the slave sends and ACK
    {
        BME680_Write(reg); // send the address of the register you wish to read
        
        if (i2c1_driver_isNACK() == 0) // if the slave sends and ACK
        {
            BME680_StopTransmission(); // stop the transmission
            
            // send device read command
            BME680_SendInitialAddress(BME680_READ_ADDRESS); 
            receivedData = BME680_Read(); // read the data
            
        }
        return receivedData; // return the received data from the I2C bus
    }
    else
    {
        return receivedData; // return 0
    }
 
}

char BME680_Read()
{
    status = I2C_READ; // set the status to I2C_READ
    i2c1_driver_startRX(); // start receiving on the I2C bus
    i2c1_waitForEvent(0); // wait for an interrupt, no timeout
    char receiveData = i2c1_driver_getRXData(); // store the receive data
    i2c1_driver_sendNACK(); // send a NACK
    i2c1_waitForEvent(0); // wait for an interrupt
    BME680_StopTransmission(); // stop tranmission
    return receiveData;
}
/**
 * sends a stop condition to the I2C bus
 */
void BME680_StopTransmission()
{
    status = I2C_IDLE; // set the status to idle
    i2c1_driver_stop(); // send the stop condition
    i2c1_waitForEvent(0); // wait for an interrupt
}

void BME680_SetOsrsH(uint8_t setPoint)
{
    uint8_t reg = Ctrl_hum_REGISTER; // get the value of the register
    BME680_WriteRegisterSetPoint(reg, setPoint, 0,2);
 
}

void BME680_SetOsrsT(uint8_t setPoint)
{
    uint8_t reg = Ctrl_meas_REGISTER; // get the value of the register
    BME680_WriteRegisterSetPoint(reg, setPoint, 5,7);
}

void BME680_SetOsrsP(uint8_t setPoint)
{
    uint8_t reg = Ctrl_meas_REGISTER; // get the value of the register
    BME680_WriteRegisterSetPoint(reg, setPoint, 2,4);
   
}

void BME680_SetGasWait0 (uint8_t setPoint)
{
    uint8_t reg = Gas_wait_0_REGISTER; // get the value of the register
    BME680_WriteRegisterSetPoint(reg,setPoint, 0, 7);
}

void BME680_WriteRegisterSetPoint (uint8_t reg, uint8_t setPoint, 
                                                        uint8_t startPosition, 
                                                        uint8_t endPosition)
{
    char currentValue = BME680_ReadFrom(&reg); // read from the register
    uint8_t registerContent = insertValue(currentValue, setPoint, startPosition, 
                                                                endPosition);
    BME680_WriteData(&reg,&registerContent);
    Disp2String("\n\rUpdated register content in register ");
    Disp2Hex(reg);
    Disp2String(" is: ");
    Disp2Hex(BME680_ReadFrom(&reg));
}

uint8_t insertValue(uint8_t currentValue, uint8_t insertValue, 
                                                        uint8_t startPosition, 
                                                        uint8_t endPosition)
{
    // get the int representation if bit range is set to 1, and others set to 0
    uint8_t intValue = getIntRepresentationSet1(startPosition, endPosition);
    // shift the int value to the left by start position
    intValue = intValue << startPosition; 
    uint8_t mask = currentValue & intValue; // and the temp and intValue
    // shift the inserted value by the endPosition 
    insertValue = insertValue << startPosition; 
    return mask | insertValue; // return the OR of the mask and insert value
}

uint8_t getIntRepresentationSet1(uint8_t start, uint8_t end)
{
    double value = 0; // create a variable to hold the final value
    uint8_t i = 0; // set up the loop counter
    while (i < (end - start) + 1)
    {
        value += pow(2,i); // add the decimal value of this bit position
        i++; // increment i
    }
    return ~(uint8_t)value; // return the final answer
}

/**
* @brief Bus communication function pointer which should be mapped to
 * the platform specific write functions of the user. 
 * NOTE: The first argument to this function is essentially useless because
 * the second argument is the interleaved array containing both the register 
 * addresses and the register data
 *
 * @param[in]     reg_addr : 8bit register address of the sensor
 * @param[out]    reg_data : Data to the specified address
 * @param[in]     length   : Length of the reg_data array
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 * @retval 0 for Success
 * @retval Non-zero for Failure
 */
uint8_t BME680API_WriteData(uint8_t reg_addr, const uint8_t *reg_data, 
                                                                uint32_t length,
                                                                void *intf_ptr)
{
    
    // reg_addr will is the 1st element of the interleaved array
    uint8_t* addrPointer = reg_data - 1; 
    int signedLength = length;
    while(signedLength > 0)
    {

        BME680_WriteData(addrPointer, addrPointer + 1); // write to the BME
        signedLength-=2; // decrement the length
        addrPointer += 2; // advance reg_addr by 2 data spaces
    }
    return i2c1_driver_isNACK(); // return 0 if slave sends ACK, 1 if NACK
    
}

/*!
 * @brief Bus communication function pointer which should be mapped to
 * the platform specific read functions of the user
 *
 * @param[in]     reg_addr : 8bit register address of the sensor
 * @param[out]    reg_data : Data from the specified address
 * @param[in]     length   : Length of the reg_data array
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 * @retval 0 for Success
 * @retval Non-zero for Failure
 */
uint8_t BME680API_ReadData(uint8_t reg_addr, uint8_t *reg_data, uint32_t length,
                                                                void *intf_ptr)
{
    uint8_t index = 0; // create the current index variable
    while (length > 0)
    {
//        Disp2String("\n\rReg addr in ReadData is:");
//        Disp2Hex(reg_addr);
        // read from reg_addr and put result into correct index in reg_data
        reg_data[index] = BME680_ReadFrom(&reg_addr);
//        Disp2String("\n\rReg data in Read Data is:");
//        Disp2Hex(reg_data[index]);
        reg_addr++;
        index++; // increment the index
        length--; // decrement the length
    }
    if (i2c1_driver_isNACK() == 1)
    {
        return 0;       // return 0 if master sends an NACK
    }
    else
    {
        return 1;
    }
}

/*!
 * @brief Delay function pointer which should be mapped to
 * delay function of the user
 *
 * @param period - The time period in microseconds
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 */
void BME680API_Delay_us(uint32_t period, void *intf_ptr)
{
    __delay_us(period); // delay for the microseconds specified by period
}