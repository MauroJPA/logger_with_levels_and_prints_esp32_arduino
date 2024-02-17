/**
  ******************************************************************************
  * @Company    : Mauro Almeida.
  * @file       : ma_api_logger.cpp
  * @author     : Mauro Almeida
  * @version	  : V0.0 
  * @date       : 09/02/2024
  * @brief      : API of resistive rain sensor
  ******************************************************************************
*/ 

/* Includes ------------------------------------------------------------------*/ 
// C language standard library

// Mauro Almeida driver library

// API library
#include "ma_api_logger.h"


/*******************************************************************************
							HOW TO USE THIS API
********************************************************************************

1. 	First, you should include in your .c file the 
    "ma_api_rain_sensor.h" file.

2.  Call ma_api_rain_sensor_init(DESIRED-PIN) to initialize and setup 
    the sensor parameters.

3.  Call ma_api_rain_sensor_value() to get the sensor raw value.

4.  Call ma_api_rain_sensor_average_percentage_value() to get the sensor 
    average in percentage value.

*******************************************************************************/

/* Private define ------------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef struct {
    LogLevel logLevel;
    size_t maxFileSizeKB;
    size_t maxFileCount;
    const char *logFileName;
    int printEnabled;
    char cSpiffsInitilized;
} Logger;

/* Private variables ---------------------------------------------------------*/
Logger logger;

/* Private function prototypes -----------------------------------------------*/  
const char *ma_api_log_get_level_to_string(LogLevel level);
const char *ma_api_log_get_timestamp(void);
void ma_api_log_write_to_file(const char *logEntry);
void ma_api_log_rotate_files(void);
int8_t ma_api_initialize_spiffs(void) ;

/* Public objects ------------------------------------------------------------*/

/* Body of private functions -------------------------------------------------*/

/**
  * @Func       : ma_api_logger_init    
  * @brief      : Init setup for logger
  * @pre-cond.  : Installation of ESP32Time library and setup RTC time
  * @post-cond. : Logger initialized and ready to be used
  * @parameters : Log level desired, log file maximum size, log file maximun files, log file name and enable if you desired print in serial
  * @retval     : 0 = Succesfully; -1 = Failed to initialize SPIFFS
  */
int8_t ma_api_logger_init(LogLevel logLevel, size_t maxFileSizeKB, size_t maxFileCount, const char *logFileName, int printEnabled) 
{
    (ma_api_initialize_spiffs() == -1) ? return -1;

    if (printEnabled) 
    {
        PRINTF("Logger initialized! - Nível de Log: %s, Tamanho Máximo do Arquivo: %ld KB, Quantidade Máxima de Arquivos: %ld, Impressão Habilitada: %d\n",
               ma_api_log_get_level_to_string(logLevel), maxFileSizeKB, maxFileCount, printEnabled);
    }
    logger.logLevel = logLevel;
    logger.maxFileSizeKB = maxFileSizeKB;
    logger.maxFileCount = maxFileCount;
    logger.printEnabled = printEnabled;
    ma_api_log_message(ALL, "Logger initialized! Nível de Log: %s. Tamanho Máximo do Arquivo: %ldkb. Quantidade Máxima de Arquivos: %ld. Impressão Habilitada: %d.",
               ma_api_log_get_level_to_string(logger.logLevel), logger.maxFileSizeKB, logger.maxFileCount, logger.printEnabled);
    
    return 0;
}

/**
  * @Func       : ma_api_log_message    
  * @brief      : Generate a message log based on especified level log.
  * @pre-cond.  : ma_api_logger_init
  * @post-cond. : The log message is save in file and if applied print in serial.
  * @parameters : 
  *               - level: Level of message log.
  *               - format: Format of messa log, followed for your args for formating.
  * @retval     : void.
  */
void ma_api_log_message(LogLevel level, const char *format, ...) 
{
  if(logger.cSpiffsInitilized == -1) 
  {
      (ma_api_initialize_spiffs() == -1) ? return;
  }

  if (level >= logger.logLevel) 
  {
    va_list args;
    va_start(args, format);

    // Size for logEntry
    size_t size = vsnprintf(nullptr, 0, format, args) + 1; // +1 for \0
    
    char *logEntry = (char *)malloc(size);

    if (!logEntry) 
    {
      PRINTF("Failed to allocate memory\n");
      va_end(args);
      return;
    }

    vsnprintf(logEntry, size, format, args);
    va_end(args);

    // Size for logMessage
    size_t logMessageSize = strlen(ma_api_log_get_timestamp()) + 4 + strlen(ma_api_log_get_level_to_string(level)) + 4 + strlen(logEntry) + 1;

    char *logMessage = (char *)malloc((logMessageSize +1) * sizeof(char));
    if (!logMessage) 
    {
        PRINTF("Failed to allocate memory\n");
        free(logEntry);
        return;
    }

    snprintf(logMessage, logMessageSize, "%s - [%s] - %s", getTimestamp(), ma_api_log_get_level_to_string(level), logEntry);
    
    if (logger.printEnabled) 
    {
      PRINTF("%s\n",logMessage);
    }

    ma_api_log_write_to_file(logMessage);

    free(logEntry);
    free(logMessage);
  }
}

/**
  * @Func       : ma_api_log_update_configs    
  * @brief      : Update configs for logger
  * @pre-cond.  : Installation of ESP32Time library and setup RTC time
  * @post-cond. : Logger updated and ready to be used
  * @parameters : New Log level desired, New log file maximum size, New log file maximun files, New log file name and enable if you desired print in serial
  * @retval     : void
  */
void ma_api_log_update_configs(LogLevel newLogLevel, size_t newMaxFileSizeKB, size_t newMaxFileCount, int newPrintEnabled) {
    if (logger.printEnabled || newPrintEnabled) {
        PRINTF("Novo - Nível de Log: %s, Tamanho Máximo do Arquivo: %ld KB, Quantidade Máxima de Arquivos: %ld, Impressão Habilitada: %d\n",
               ma_api_log_get_level_to_string(newLogLevel), newMaxFileSizeKB, newMaxFileCount, newPrintEnabled);
    }
    logger.logLevel = newLogLevel;
    logger.maxFileSizeKB = newMaxFileSizeKB;
    logger.maxFileCount = newMaxFileCount;
    logger.printEnabled = newPrintEnabled;
    ma_api_log_message(ALL, "Nova configuração! Nível de Log: %s. Tamanho Máximo do Arquivo: %ldkb. Quantidade Máxima de Arquivos: %ld. Impressão Habilitada: %d.",
               ma_api_log_get_level_to_string(logger.logLevel), logger.maxFileSizeKB, logger.maxFileCount, logger.printEnabled);
}

/**
  * @Func       : am_api_get_level_to_string    
  * @brief      : Converts a log level enum value to its corresponding string representation.
  * @pre-cond.  : None.
  * @post-cond. : None.
  * @parameters : 
  *               - level: The log level enum value.
  * @retval     : A pointer to a constant string representing the log level. If the log level
  *               is not recognized, an empty string is returned.
  */
const char *ma_api_log_get_level_to_string(LogLevel level) {
    switch (level) {
        case NONE: return "NONE";
        case DEBUG: return "DEBUG";
        case INFO: return "INFO";
        case ERROR: return "ERROR";
        case GENERIC: return "GENERIC";
        case ALL: return "ALL";
        default: return "";
    }
}

/**
  * @Func       : ma_api_log_get_timestamp    
  * @brief      : Returns a string representing the current timestamp in the format "dd/mm/yyyy-hh:mm:ss".
  * @pre-cond.  : Installation of ESP32Time library and seted RTC time
  * @post-cond. : Read tu be used.
  * @parameters : Void.
  * @retval     : A pointer to a constant string representing the current timestamp.
  */

const char *ma_api_log_get_timestamp(void) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    static char timestamp[20];
    sprintf(timestamp, "%02d/%02d/%04d-%02d:%02d:%02d",
            timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return timestamp;
}

/**
  * @Func       : ma_api_log_write_to_file    
  * @brief      : Writes a log entry to a file.
  * @pre-cond.  : ma_api_logger_init an ma_api_log_message.
  * @post-cond. : The log entry is appended to the specified log file.
  * @parameters : 
  *               - logEntry: A pointer to a constant string representing the log entry to be written.
  * @retval     : Void.
  */
void ma_api_log_write_to_file(const char *logEntry) {
    File file = SPIFFS.open(logger.logFileName, "a");
    if (!file) {
        if (logger.printEnabled) {
            PRINTF("Falha ao abrir arquivo de log\n");
        }
        return;
    }

    if (file.size() >= logger.maxFileSizeKB * 1024) {
        ma_api_log_rotate_files();
    }
    file.println(logEntry);
    file.close();
}

/**
  * @Func       : ma_api_log_rotate_files    
  * @brief      : Rotates log files by renaming them with sequential numbers.
  * @pre-cond.  : ma_api_logger_init.
  * @post-cond. : Log files are rotated according to the specified maximum file count.
  * @parameters : Void.
  * @retval     : Void.
  */
void ma_api_log_rotate_files(void) 
{
    for (size_t i = logger.maxFileCount - 1; i > 0; --i) 
    {
        char oldFileName[strlen(logger.logFileName) + 5];
        char newFileName[strlen(logger.logFileName) + 5];
        sprintf(oldFileName, "%s.%ld", logger.logFileName, i - 1);
        sprintf(newFileName, "%s.%ld", logger.logFileName, i);
        SPIFFS.rename(oldFileName, newFileName);
    }

    char newFileName[strlen(logger.logFileName) + 3];
    sprintf(newFileName, "%s.0", logger.logFileName);
    SPIFFS.rename(logger.logFileName, newFileName);
}

/**
  * @Func       : ma_api_initialize_spiffs    
  * @brief      : Initialize SPIFFS.
  * @pre-cond.  : #include <SPIFFS.h>.
  * @post-cond. : SPIFFS Initialized and read to be used.
  * @parameters : Void.
  * @retval     : 0 = Succesfully and -1 = Fail to initialize SPIFFS.
  */
int8_t ma_api_initialize_spiffs(void) 
{
    if (!SPIFFS.begin(true)) 
    {
        PRINTF("Failed to mount file system\n");
        logger.cSpiffsInitilized = -1;
        return -1;
    }
    logger.cSpiffsInitilized = 0;
    return 0;
}

/*****************************END OF FILE**************************************/