/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: Habib Jinadu
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef BME680_H
#define	BME680_H

#include <xc.h> // include processor files - each processor file is guarded.  


#define Config_REGISTER         0x75
#define Ctrl_meas_REGISTER      0x74
#define Ctrl_hum_REGISTER       0x72
#define Ctrl_gas_1_REGISTER     0x71
#define Ctrl_gas_0_REGISTER     0x70
#define Gas_wait_0_REGISTER     0x64
#define gas_r_lsb_REGISTER      0x2B
#define gas_r_msb_REGISTER      0x2A
#define hum_lsb_REGISTER        0x26
#define hum_msb_REGISTER        0x25
#define temp_xlsb_REGISTER      0x24
#define temp_lsb_REGISTER       0x23
#define press_xlsb_REGISTER     0x21
#define press_lsb_REGISTER      0x20
#define press_mab_REGISTER      0x1F
#define eas_status_0_REGISTER   0x1D



#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef	__cplusplus
}
#endif /* __cplusplus */

void BME680_SendInitialAddress(uint8_t value);
void BME680_WriteData(uint8_t *regAddress, uint8_t *data);
void BME680_Write(uint8_t *value);
void BME680_StopTransmission();
char BME680_ReadFrom(uint8_t *reg);
char BME680_Read();
void BME680_SetOsrsH(uint8_t setPoint);
void BME680_SetOsrsT(uint8_t setPoint);
void BME680_SetOsrsP(uint8_t setPoint);
void BME680_WriteRegisterSetPoint (uint8_t reg, uint8_t setPoint, 
                                                        uint8_t startPosition, 
                                                        uint8_t endPosition);
uint8_t insertValue(uint8_t currentValue, uint8_t insertValue, 
                                                        uint8_t startPosition, 
                                                        uint8_t endPosition);
uint8_t getIntRepresentationSet1(uint8_t start, uint8_t end);

uint8_t BME680API_WriteData(uint8_t reg_addr, const uint8_t *reg_data, 
                                                                uint32_t length,
                                                                void *intf_ptr);

uint8_t BME680API_ReadData(uint8_t reg_addr, uint8_t *reg_data, uint32_t length,
                                                                void *intf_ptr);
void BME680API_Delay_us(uint32_t period, void *intf_ptr);
#endif	/* BME680_H */

