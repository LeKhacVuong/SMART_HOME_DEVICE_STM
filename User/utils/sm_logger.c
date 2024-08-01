//
// Created by vnbk on 24/03/2023.
//
#include <sm_logger.h>
#include <stdio.h>
#include <stdarg.h>


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

static const char *log_level_colors[LOG_LEVEL_NUMBER] = {RED,YEL,GRN,RESET, WHT };

static const char *log_level_names[LOG_LEVEL_NUMBER] = {"ERROR","WARN ", "INFO ", "DEBUG", "TRACE" };

static void logger_put(const char *str){

}

#define LOG_QUEUE_SIZE      10

typedef struct logger{
    LOG_LEVEL m_level;
    logger_output_fn_t m_output_fn;
}logger_t;

static logger_t g_logger = {
        .m_level = LOG_LEVEL_TRACE,
        .m_output_fn = NULL
};
/**
 *
 * @param _fn
 * @param _level
 * @return
 */
int32_t sm_logger_init(logger_output_fn_t _fn, LOG_LEVEL _level){
    g_logger.m_level= _level;
    g_logger.m_output_fn = _fn;

    return 0;
}
/**
 *
 * @param _level
 * @return
 */
int32_t sm_logger_set_level(LOG_LEVEL _level){
    g_logger.m_level = _level;
    return 0;
}
/**
 *
 * @return
 */
LOG_LEVEL sm_logger_get_level(){
    return g_logger.m_level;
}
/**
 *
 * @param _level
 * @param _tag
 * @param _log
 */
void sm_logger_show(LOG_LEVEL _level, const char* _tag, const char* _log, ...){
    int len = 0;
    char buf[LOG_BUFFER_SIZE + 2] = {0};
    va_list args;

    if(_level > g_logger.m_level){
        return;
    }

    va_start(args, _log);

    len = snprintf(buf, LOG_BUFFER_SIZE, "%s#%s: %s: ",log_level_colors[_level], log_level_names[_level], _tag);
    len += vsnprintf(buf + len, LOG_BUFFER_SIZE - len, _log, args);
    va_end(args);

    if(len > LOG_BUFFER_SIZE){
        len = LOG_BUFFER_SIZE;
    }
    buf[len] = '\0';

    if(g_logger.m_output_fn)
        g_logger.m_output_fn(buf);
    else{
        logger_put (buf);
    }
}
