//
// Created by vnbk on 23/03/2023.
//

#ifndef ELAPSED_TIMER_H
#define ELAPSED_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief
 * @return
 */
extern int64_t get_tick_count();

typedef struct elapsed_timer{
    int32_t m_duration;
    int32_t m_start_time;
}elapsed_timer_t;
/**
 * @brief
 * @param _self
 */
static inline void elapsed_timer_reset(elapsed_timer_t* _self){
    _self->m_start_time = get_tick_count();
}
/**
 * @brief
 * @param _self
 * @param _duration
 */
static inline void elapsed_timer_resetz(elapsed_timer_t* _self, int32_t _duration){
    _self->m_duration = _duration;
    _self->m_start_time = get_tick_count();
}
/**
 * @brief
 * @param _self
 */
static inline void elapsed_timer_pass(elapsed_timer_t* _self){
    _self->m_duration = 0;
}
/**
 * @brief
 * @param _self
 * @return
 */
static inline int32_t elapsed_timer_get_remain(elapsed_timer_t* _self){
    int32_t remain_time = get_tick_count() - _self->m_start_time;
    if(remain_time >= _self->m_duration){
        return 0;
    }else{
        return _self->m_duration - remain_time;
    }
}

#ifdef __cplusplus
};
#endif

#endif //ELAPSED_TIMER_H
