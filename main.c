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
/*
                         Main application
 */
int main(void)
{
    // initialize the PIC device
    SYSTEM_Initialize();
    
    /*------INITIALIZATION CODE FOR THE BME680--------------*/
    struct bme68x_dev bme; // define the struct for the BME device
    int8_t rslt;            // define the result
    struct bme68x_conf conf; // define the struct for the BME configuration
    struct bme68x_heatr_conf heatr_conf; // struct for the heater configuration
    struct bme68x_data data; // struct for the data
    uint32_t del_period;
    uint32_t time_ms = 0;
    uint8_t n_fields;
    uint16_t sample_count = 1;
    
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
    
    
    while (1)
    {
        displayDateAndTime(); // display date and time to tera term
        // get the date and time values and store them in the time object
        getDateAndTime(&time); 
        
        Disp2String("\n\rTemperature(deg C), Pressure(Pa), Humidity(%%), Gas resistance(ohm), Status\n\r");
        rslt = bme68x_set_op_mode(BME68X_FORCED_MODE, &bme);

        /* Calculate delay period in microseconds */
        del_period = bme68x_get_meas_dur(BME68X_FORCED_MODE, &conf, &bme) + (heatr_conf.heatr_dur * 1000);
        bme.delay_us(del_period, bme.intf_ptr);

        /* Check if rslt == BME68X_OK, report or handle if otherwise */
        rslt = bme68x_get_data(BME68X_FORCED_MODE, &data, &n_fields, &bme);
        Disp2Dec(data.temperature / 100);
        Disp2String(",              ");
        Disp2Dec((long unsigned int)data.pressure);
        Disp2String(",          ");
        Disp2Dec((long unsigned int)(data.humidity / 1000));
        Disp2String(",         ");
        Disp2Dec((long unsigned int)data.gas_resistance);
        Disp2String(",       ");
        Disp2Dec(data.status);
    
    result = f_mount(&FatFs, "", 1);
        if (result == FR_OK) 
    {

        result = f_open(&Fil, fileName, FA_OPEN_APPEND | FA_READ | FA_WRITE);
        Disp2String("\n\rThe result of f_open is: ");
        Disp2Dec(result);
        if (result == FR_OK) /* Open or create a file */
        {	
            f_printf(&Fil,"20%d/%d/%d, %d:%d:%d, %d, %d, %d, %d\n",
                time.tm_year,time.tm_mon, time.tm_mday, time.tm_hour, 
                time.tm_min, time.tm_sec,data.temperature/100, 
                data.pressure,data.humidity / 1000,data.gas_resistance);
            //char* sentence = "This time, we put it in a function!!!\n"; // get a sentence
            //writeToEndOfFile(&Fil, sentence); // write this sentence to the card
			f_close(&Fil);
        }

    }
    
    //pwrOn(); // turn on the fan
    sendMessage(FAN_POWER_OFF_ECO_SPEED);
    bme68x_set_op_mode(BME68X_SLEEP_MODE,&bme); // put the BME680 in sleep mode
    Sleep(); // put the PIC to sleep
    }

    return 1;
}
/**
 End of File
*/

