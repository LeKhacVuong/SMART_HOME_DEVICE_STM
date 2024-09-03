//
// Created by vnbk on 21/03/2023.
//

#ifndef SM_HOST_H
#define SM_HOST_H

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
#include "sm_cmd.h"
#include "sm_elapsed_timer.h"

#define SM_HOST_ADDR_DEFAULT    0x01
#define SM_HOST_MAX_ADDR        0x7F
#define SM_HOST_MAX_BUFFER      (256)

#define SM_HOST_MAX_COMMUNICATION_BUFFER_SIZE 256

#define SM_HOST_ASYNC_MODE      0
#define SM_HOST_SYNC_MODE       1

#define SM_HOST_TIMEOUT_IF_DEFAULT  1000

typedef void sm_host_t;

typedef int32_t (*sm_host_cmd_callback_fn_t)(int32_t, const uint8_t*, int32_t, void*);

typedef int32_t (*sm_host_send_if)(const uint8_t*, int32_t, int32_t, void*);
typedef int32_t (*sm_host_recv_if)(uint8_t*, int32_t, int32_t, void*);

sm_host_t* sm_host_create(uint8_t _mode, uint8_t _addr, sm_host_send_if _host_send_if, sm_host_recv_if _host_receive_if, void* _if_arg);
sm_host_t* sm_host_create_default(uint8_t _mode, uint8_t _addr, sm_host_send_if _host_send_if, sm_host_recv_if _host_receive_if, void* _if_arg);
int32_t    sm_host_destroy(sm_host_t* _host);

int32_t sm_host_set_mode(sm_host_t* _host, uint8_t _mode);
int32_t sm_host_get_mode(sm_host_t* _host);

int32_t sm_host_set_addr(sm_host_t* _host, uint8_t _addr);
int32_t sm_host_reg_handle(sm_host_t* _host, sm_host_cmd_callback_fn_t _callback, void* _arg);
int32_t sm_host_set_host_if(sm_host_t* _host, sm_host_send_if _send_if, sm_host_recv_if _recv_if, void* _arg_if);

int32_t sm_host_send_cmd(sm_host_t* _host, int32_t _cmd, const uint8_t* _data, int32_t _len);
int32_t sm_host_send_cmd_sync(sm_host_t* _host, int32_t _cmd, const uint8_t* _data, int32_t _len, int32_t _timeout, sm_host_cmd_callback_fn_t _callback, void* _arg);

int32_t sm_host_send_response(sm_host_t* _host, int32_t _cmd, int32_t _ret, const uint8_t* _data, int32_t _len);

int sm_host_process(sm_host_t* _host);

/**
 * @brief This function is put in the Interrupt or other thread.
 * @param _data
 * @param _len
 * @param _user_data: is pointer sm_host_t
 * @return
 */
extern int32_t sm_host_asyn_feed(const uint8_t* _data, int32_t _len, void* _user_data);

#ifdef __cplusplus
};
#endif

#endif //SM_HOST_H
