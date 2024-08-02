//
// Created by mrgundam on 06/05/2024.
//

#ifndef SM_TIME_UTILS_H
#define SM_TIME_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

static inline int64_t getTickCount() {
    return get_tick_count();
}

static inline void delay(int32_t _second) {
    int64_t timeout = getTickCount() + _second * 1000;
    while (timeout > getTickCount());
}

static inline void delayMs(int32_t _ms) {
    int64_t timeout = getTickCount() + _ms;
    while (timeout > getTickCount());
}

#ifdef __cplusplus
};
#endif


#endif //SM_TIME_UTILS_H
