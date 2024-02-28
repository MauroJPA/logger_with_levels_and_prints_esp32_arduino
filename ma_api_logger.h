/**
    ******************************************************************************
    * @Company    : Mauro Almeida.
    * @file       : ma_rain_sensor.h
    * @author     : Mauro Almeida
    * @version	  : V0.0 
    * @date       : 09/02/2024
    * @brief      : Header file of resistive rain sensor
    ******************************************************************************
*/ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MA_API_LOGGER_H
#define __MA_API_LOGGER_H

/* Includes ------------------------------------------------------------------*/  
#ifdef ARDUINO
#include <Arduino.h>
#define PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define PRINTF(...) printf(__VA_ARGS__)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ESP32Time.h>
#include <SPIFFS.h>

extern "C" {
/* Define --------------------------------------------------------------------*/
/* Typedef -------------------------------------------------------------------*/
typedef enum {
    ALL,
    DEBUG,
    GENERIC,
    INFO,
    ERROR,
    NONE,
} LogLevel;

/* Public objects ------------------------------------------------------------*/

//Initilize logger configs
extern int8_t ma_api_logger_init(LogLevel logLevel, size_t maxFileSizeKB, size_t maxFileCount, const char *logFileName, int printEnabled);

//Update or initilize logger configs
extern void ma_api_log_update_configs(LogLevel newLogLevel, size_t newMaxFileSizeKB, size_t newMaxFileCount, int newPrintEnabled);

//Write and save log message
extern int8_t ma_api_log_message(LogLevel level, const char *format, ...);
}

#endif /* __MA_API_LOGGER_H */
/*****************************END OF FILE**************************************/