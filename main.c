/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ128GA202
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
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


#include "mcc_generated_files/system.h"
#include "bme68x.h"
#include "bme68x_defs.h"
#include "BME680.h"
#include "diskio.h"
#include "ff.h"
#include "ffconf.h"
#include "SDFunctions.h"
#include "mcc_generated_files/rtcc.h"
#include "IR.h"
#include "tmr2.h"
#include "IR_Receiver.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/interrupt_manager.h"

#define FCY 4000000UL
#include <libpic30.h>
/*
                         Main application
 */

// function prototype for BME get data
void getBME680Data(struct bme68x_dev* bme, struct bme68x_conf* conf,
                    struct bme68x_heatr_conf* heatr_conf, uint32_t* del_period,
                    struct bme68x_data* data, uint8_t* n_fields); 

void writeBME680DataToSDCard(FIL* Fil, char* fileName, 
                                                    struct bme68x_data* data,
                                                    bcdTime_t* time);
typedef enum FAN_STATUS
{
    FAN_IS_OFF,      // (0) fan is off
    FAN_IS_ON       // (1) fan is on
}FAN_STATUS;

uint8_t fanStatus = FAN_IS_ON; // initialize fan status as off
int main(void)
{
    // initialize the PIC device
    SYSTEM_Initialize();
    __delay_ms(100); // delay for 100 ms
    
    /*------INITIALIZATION CODE FOR THE BME680--------------*/
    struct bme68x_dev bme; // define the struct for the BME device
    int8_t rslt;            // define the result
    struct bme68x_conf conf; // define the struct for the BME configuration
    struct bme68x_heatr_conf heatr_conf; // struct for the heater configuration
    struct bme68x_data data; // struct for the data
    uint32_t del_period;
    uint8_t n_fields;
    
    rslt = bme68x_init(&bme); // initialize the BME680
    
    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    conf.filter = BME68X_FILTER_OFF;
    conf.odr = BME68X_ODR_NONE;
    conf.os_hum = BME68X_OS_16X;
    conf.os_pres = BME68X_OS_1X;
    conf.os_temp = BME68X_OS_2X;
    rslt = bme68x_set_conf(&conf, &bme);

    
    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    heatr_conf.enable = BME68X_ENABLE;
    heatr_conf.heatr_temp = 300;
    heatr_conf.heatr_dur = 100;
    rslt = bme68x_set_heatr_conf(BME68X_FORCED_MODE, &heatr_conf, &bme);

    /*-----------INITIALIZATION CODE FOR THE SD CARD ----------------------*/
    FATFS FatFs;	/* FatFs work area needed for each volume */
    FIL Fil;		/* File object needed for each open file */
    char * fileName = "dataLog.txt"; // file name
    int result; // make a variable to store the result of the SD card functions
    bcdTime_t time; // define a time structure
    f_mount(&FatFs, "", 1); // mount the SD card
    
    /*--------------INITIALIZATION CODE FOR THE IR RECEIVER---------------*/
    uint64_t IR_Message; // create a variable to hold the IR message
    while (1)
    {
        if (CN_interrupt_flag == 1) // if there is a change notification 
        {
            disableRTCInterrupts(); // disable RTC interrupts
            uint8_t bitsReceived;
            IR_Message = decode_IR(&bitsReceived); // decode the IR message
            Disp2String("\n\r");
            Disp2Hex64(IR_Message); // display to UART
//            Disp2String("\n\rThe number of bits received is:");
//            Disp2Dec(bitsReceived);
            enableRTCInterrupts(); // enable RTC interrupts
        }
        
        disableCNInterrupts(); // disable CN Interrupts
        
        //displayDateAndTime(); // display date and time to tera term
        // get the date and time values and store them in the time object
        getDateAndTime(&time); 
        
        getBME680Data(&bme,&conf,&heatr_conf,&del_period,&data, &n_fields);
        

//        Disp2String("\n\r");
//        Disp2Dec(data.temperature / 100);
//        Disp2String(",              ");
//        Disp2Dec((long unsigned int)data.pressure);
//        Disp2String(",          ");
//        Disp2Dec((long unsigned int)(data.humidity / 1000));
//        Disp2String(",         ");
//        Disp2Dec((long unsigned int)data.gas_resistance);
//        Disp2String(",       ");
//        Disp2Dec(data.status);


        
        writeBME680DataToSDCard(&Fil, fileName, &data, &time);
        
        if ((long unsigned int)(data.humidity / 1000) > 50 && 
                                                        fanStatus == FAN_IS_OFF)
        {
            sendMessage(FAN_POWER_ON_ECO_SPEED_NATURAL_WIND);
            fanStatus = FAN_IS_ON; // update the fan status
        }
        else if ((long unsigned int)(data.humidity / 1000) < 45 && 
                                                        fanStatus == FAN_IS_ON)
        {
            sendMessage(FAN_POWER_OFF); // turn the fan off
            fanStatus = FAN_IS_OFF; // update the fan status
        }
        
        bme68x_set_op_mode(BME68X_SLEEP_MODE,&bme); // put the BME680 in sleep mode
        enableCNInterrupts();
        Sleep(); // put the PIC to sleep
    }

    return 1;
}

void getBME680Data(struct bme68x_dev* bme, struct bme68x_conf* conf,
                    struct bme68x_heatr_conf* heatr_conf, uint32_t* del_period,
                    struct bme68x_data* data, uint8_t* n_fields)
{
    bme68x_set_op_mode(BME68X_FORCED_MODE, bme); // change op mode to forced
    /* Calculate delay period in microseconds */
    *del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, conf, bme) + 
                                                (heatr_conf->heatr_dur * 1000);
    bme->delay_us(*del_period, bme->intf_ptr);
    
    /* Check if rslt == BME68X_OK, report or handle if otherwise */
    bme68x_get_data(BME68X_FORCED_MODE, data, n_fields, bme);
}

void writeBME680DataToSDCard(FIL* Fil, char* fileName, 
                                                    struct bme68x_data* data,
                                                    bcdTime_t* time)
{
    int result = f_open(Fil, fileName, FA_OPEN_APPEND | FA_READ | FA_WRITE);
    //Disp2String("\n\rThe result of f_open is: ");
    //Disp2Dec(result);
    if (result == FR_OK) /* Open or create a file */
    {	
        f_printf(Fil,"20%d/%d/%d, %d:%d:%d, %d, %lu, %lu, %lu\n",
            time->tm_year,time->tm_mon, time->tm_mday, time->tm_hour, 
            time->tm_min, time->tm_sec,data->temperature/100, 
            (long unsigned int)data->pressure,
            (long unsigned int)(data->humidity / 1000),
            (long unsigned int)data->gas_resistance);

        f_close(Fil);
    }

}
/**
 End of File
*/

