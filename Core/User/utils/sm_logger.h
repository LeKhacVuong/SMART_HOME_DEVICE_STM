//
// Created by vnbk on 24/03/2023.
//

#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
//#include <sm_hal_uart.h>

#define LOG_BUFFER_SIZE 1024

typedef void (*logger_output_fn_t)(const char*);

extern void sm_logger_put(const char*);

typedef enum {
    LOG_LEVEL_ERR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_TRACE,
    LOG_LEVEL_NUMBER
}LOG_LEVEL;

int32_t sm_logger_init(logger_output_fn_t _fn, LOG_LEVEL _level);
int32_t sm_logger_set_level(LOG_LEVEL _level);
LOG_LEVEL sm_logger_get_level();



/**
 * init logger
 * @param _level : level debug
 * @param _tag   : name
 * @param _log   : log data
 */
void sm_logger_show(LOG_LEVEL _level, const char* _tag, const char* _log, ...);

#define LOG_ERR(LOG_TAG, ...)       sm_logger_show(LOG_LEVEL_ERR,     LOG_TAG,    __VA_ARGS__)
#define LOG_WRN(LOG_TAG, ...)       sm_logger_show(LOG_LEVEL_WARN,    LOG_TAG,    __VA_ARGS__)
#define LOG_INF(LOG_TAG, ...)       sm_logger_show(LOG_LEVEL_INFO,    LOG_TAG,    __VA_ARGS__)
#define LOG_DBG(LOG_TAG, ...)       sm_logger_show(LOG_LEVEL_DEBUG,   LOG_TAG,    __VA_ARGS__)
#define LOG_TRACE(LOG_TAG, ...)     sm_logger_show(LOG_LEVEL_TRACE,   LOG_TAG,    __VA_ARGS__)

#ifdef __cplusplus
};
#endif

#endif //LOGGER_H
