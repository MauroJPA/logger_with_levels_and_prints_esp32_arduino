#include <Arduino.h>
#include "ma_api_logger.h"

#define SERIAL_BAUD_RATE 115200

#define LOG_FILE_NAME "/appLog.log" //Name of your choice
#define PRINT_ENABLED true //If true Need Serial.begin() initialized

ESP32Time rtc(OFFSET_FOR_LOCAL_TIME);  // offset in seconds GMT+1

void setup() 
{
  Serial.begin(SERIAL_BAUD_RATE);
  PRINTF("Initializing...\n");

  ma_api_logger_init(DEBUG, 200, 5, LOG_FILE_NAME, PRINT_ENABLED);
  
  ma_api_log_message(DEBUG, "This is an example of a DEBUG-level log message");

  ma_api_log_message(GENERIC, "This is an example of an GENERIC-level log message");

  ma_api_log_message(INFO, "This is an example of a INFO-level log message");

  ma_api_log_message(ERROR, "This is an example of an ERROR-level log message");


void loop() 
{

}