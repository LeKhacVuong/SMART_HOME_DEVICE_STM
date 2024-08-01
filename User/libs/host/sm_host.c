//
// Created by vnbk on 21/03/2023.
//
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "sm_host.h"
#include "sm_crc.h"

#define SM_PROTO_START_BYTE     0xAA
#define SM_PROTO_STOP_BYTE      0x55

#define SM_PROTO_START_BYTE_INDEX       0
#define SM_PROTO_LENGTH_HIGH_INDEX      1
#define SM_PROTO_LENGTH_LOW_INDEX       2
#define SM_PROTO_DEVICE_ADDR_INDEX      3
#define SM_PROTO_CMD_INDEX              4
#define SM_PROTO_CMD_DATA_INDEX         5
#define SM_PROTO_RET_INDEX              5

#define WORD(msb,lsb)                  ((msb)*256 + (lsb))

#define _impl(host)                     ((sm_host_impl_t*)(host))

typedef struct {
    uint8_t m_mode;

    uint8_t m_addr;
    uint8_t m_buffer[SM_HOST_MAX_BUFFER];
    int32_t m_buffer_index;
    int32_t m_process_index;
    uint8_t m_packet[SM_HOST_MAX_BUFFER];
    int32_t m_packet_index;

    sm_host_send_if m_host_send_if;
    sm_host_recv_if m_host_recv_if;
    void* m_arg_if;

    sm_host_cmd_callback_fn_t m_cmd_callback;
    void* m_cmd_arg;
}sm_host_impl_t;

static sm_host_impl_t g_host_default;

sm_host_t* sm_host_create(uint8_t _mode, uint8_t _addr, sm_host_send_if _host_send_if, sm_host_recv_if _host_recv_if, void* _if_arg){
    sm_host_impl_t* host = (sm_host_impl_t*)malloc(sizeof(sm_host_impl_t));
    if(!host){
        return NULL;
    }
    if(_addr > SM_HOST_MAX_ADDR){
        host->m_addr = SM_HOST_ADDR_DEFAULT;
    } else {
        host->m_addr = _addr;
    }
    host->m_mode = _mode;

    host->m_host_send_if = _host_send_if;
    host->m_host_recv_if = _host_recv_if;
    host->m_arg_if = _if_arg;

    memset(host->m_buffer, 0, SM_HOST_MAX_BUFFER);
    memset(host->m_packet, 0, SM_HOST_MAX_BUFFER);
    host->m_buffer_index = 0;
    host->m_packet_index = 0;
    host->m_process_index = 0;

    host->m_cmd_callback = NULL;
    host->m_cmd_arg = NULL;

    return host;
}

sm_host_t* sm_host_create_default(uint8_t _mode, uint8_t _addr, sm_host_send_if _host_send_if, sm_host_recv_if _host_recv_if, void* _if_arg){
    if(_addr > SM_HOST_MAX_ADDR){
        g_host_default.m_addr = SM_HOST_ADDR_DEFAULT;
    } else {
        g_host_default.m_addr = _addr;
    }
    g_host_default.m_mode = _mode;

    g_host_default.m_host_send_if = _host_send_if;
    g_host_default.m_host_recv_if = _host_recv_if;
    g_host_default.m_arg_if = _if_arg;

    memset(g_host_default.m_buffer, 0, SM_HOST_MAX_BUFFER);
    memset(g_host_default.m_packet, 0, SM_HOST_MAX_BUFFER);
    g_host_default.m_buffer_index = 0;
    g_host_default.m_packet_index = 0;
    g_host_default.m_process_index = 0;

    g_host_default.m_cmd_callback = NULL;
    g_host_default.m_cmd_arg = NULL;

    return &g_host_default;
}

int32_t sm_host_destroy(sm_host_t* _host){
    if(_impl(_host)){
        _impl(_host)->m_cmd_arg = NULL;
        _impl(_host)->m_cmd_callback = NULL;

        memset(_impl(_host)->m_buffer, 0, SM_HOST_MAX_BUFFER);
        memset(_impl(_host)->m_packet, 0, SM_HOST_MAX_BUFFER);
        _impl(_host)->m_buffer_index = 0;
        _impl(_host)->m_packet_index = 0;
        _impl(_host)->m_process_index = 0;

        _impl(_host)->m_host_send_if = NULL;
        _impl(_host)->m_host_recv_if = NULL;
        _impl(_host)->m_arg_if = NULL;

//        free(_host);

        return 0;
    }
    return -1;
}

int32_t sm_host_set_addr(sm_host_t* _host, uint8_t _addr){
    if(!_host){
        return -1;
    }
    _impl(_host)->m_addr = _addr;
    return 0;
}

int32_t sm_host_reg_handle(sm_host_t* _host, sm_host_cmd_callback_fn_t _callback, void* _arg){
    if(!_host || !_callback){
        return -1;
    }
    _impl(_host)->m_cmd_callback = _callback;
    _impl(_host)->m_cmd_arg = _arg;

    return 0;
}

int32_t sm_host_set_host_if(sm_host_t* _host, sm_host_send_if _send_if, sm_host_recv_if _recv_if, void* _arg_if){
    if(!_host){
        return -1;
    }
    _impl(_host)->m_host_send_if = _send_if;
    _impl(_host)->m_host_recv_if = _recv_if;
    _impl(_host)->m_arg_if = _arg_if;
    return 0;
}

int32_t sm_host_set_mode(sm_host_t* _host, uint8_t _mode){
    if(!_host){
        return -1;
    }
    _impl(_host)->m_mode = _mode;
    return 0;
}

int32_t sm_host_get_mode(sm_host_t* _host){
    if(!_host){
        return -1;
    }
    return _impl(_host)->m_mode;
}

int32_t sm_host_send_cmd(sm_host_t* _host, int32_t _cmd, const uint8_t* _data, int32_t _len){
    uint8_t packet[SM_HOST_MAX_BUFFER];
    int32_t len = 0;
    int32_t data_len = _len+1+2+1; // 1-cmd, 2-crc, 1-addr
    int32_t index = 0;
    uint16_t crc;

    if(!_host) {
        return -1;
    }

    packet[len++] = SM_PROTO_START_BYTE;
    packet[len++] = (data_len >> 8) & 0xFF;
    packet[len++] = (data_len & 0xff);
    packet[len++] = _impl(_host)->m_addr;
    packet[len++] = _cmd;
    for(index = 0; index < _len; index++){
        packet[len++] = _data[index];
    }
    crc = sm_CRC_CalculateCRC16(packet, len);
    packet[len++] = (crc >> 8) & 0xFF;
    packet[len++] = (crc & 0xFF);
    packet[len++] = SM_PROTO_STOP_BYTE;

    if(_impl(_host)->m_host_send_if(packet, len, SM_HOST_TIMEOUT_IF_DEFAULT, _impl(_host)->m_arg_if) <= 0){
        return -1;
    }

    return len;
}

int32_t sm_host_send_cmd_sync(sm_host_t* _host, int32_t _cmd, const uint8_t* _data, int32_t _len, int32_t _timeout,
                              sm_host_cmd_callback_fn_t _callback, void* _arg){
    if(!_host || !_impl(_host)->m_host_recv_if){
        return -1;
    }

    uint8_t current_mode = _impl(_host)->m_mode;
    _impl(_host)->m_mode = SM_HOST_SYNC_MODE;

    int32_t ret = -1;

    sm_host_cmd_callback_fn_t current_callback = _impl(_host)->m_cmd_callback;
    void* current_arg = _impl(_host)->m_cmd_arg;

    _impl(_host)->m_cmd_callback = _callback;
    _impl(_host)->m_cmd_arg = _arg;

    if(sm_host_send_cmd(_host, _cmd, _data, _len) <= 0){
        return -1;
    }

    uint32_t poll_timeout = get_tick_count() + _timeout;
    while(poll_timeout >= get_tick_count()){
        ret = sm_host_process(_host);
        if(ret == 1){
            break;
        }
    }

    _impl(_host)->m_cmd_callback = current_callback;
    _impl(_host)->m_cmd_arg = current_arg;
    _impl(_host)->m_mode = current_mode;

    _impl(_host)->m_buffer_index = 0;
    _impl(_host)->m_packet_index = 0;
    _impl(_host)->m_process_index = 0;
    return ret;
}


int32_t sm_host_send_response(sm_host_t* _host, int32_t _cmd, int32_t _ret, const uint8_t* _data, int32_t _len){
    uint8_t packet[SM_HOST_MAX_BUFFER];
    int32_t len = 0;
    int32_t data_len = _len+1+2+1+1; // 1-cmd, 2-crc, 1-ret, 1-addr
    int32_t index = 0;
    uint16_t crc;
    if(!_host) {
        return -1;
    }
    packet[len++] = SM_PROTO_START_BYTE;
    packet[len++] = (data_len >> 8) & 0xFF;
    packet[len++] = (data_len & 0xff);
    packet[len++] = _impl(_host)->m_addr;
    packet[len++] = _cmd;
    packet[len++] = _ret;
    for(index = 0; index < _len; index++){
        packet[len++] = _data[index];
    }
    crc = sm_CRC_CalculateCRC16(packet, len);
    packet[len++] = (crc >> 8) & 0xFF;
    packet[len++] = (crc & 0xFF);
    packet[len++] = SM_PROTO_STOP_BYTE;
    if(_impl(_host)->m_host_send_if(packet, len, SM_HOST_TIMEOUT_IF_DEFAULT, _impl(_host)->m_arg_if) <= 0){
        return -1;
    }
    return len;
}

int sm_host_process(sm_host_t* _host){

    if(!_host){
        return -1;
    }

    sm_host_impl_t* host = _impl(_host);

    if(host->m_mode == SM_HOST_SYNC_MODE && host->m_host_recv_if) {
        uint8_t data;
        if (host->m_host_recv_if(&data, 1, 0, _impl(_host)->m_arg_if) > 0) {
            host->m_buffer[host->m_buffer_index++] = data;
            if(host->m_buffer_index >= SM_HOST_MAX_BUFFER){
                host->m_buffer_index = 0;
            }
        }
    }

    int32_t cmd = 0;
    if (host->m_buffer_index != host->m_process_index){
        host->m_packet[host->m_packet_index] = host->m_buffer[host->m_process_index];

        host->m_process_index++;
        if(host->m_process_index >= SM_HOST_MAX_BUFFER){
            host->m_process_index = 0;
        }


        if(host->m_packet[host->m_packet_index] == SM_PROTO_STOP_BYTE && (host->m_packet_index >= 7)){

            uint32_t buffer_len = (WORD(host->m_packet[SM_PROTO_LENGTH_HIGH_INDEX], host->m_packet[SM_PROTO_LENGTH_LOW_INDEX]) + 3);

            if(buffer_len >= SM_HOST_MAX_COMMUNICATION_BUFFER_SIZE){
                host->m_packet_index = 0;
                return -1;
            }

            if(host->m_packet_index > buffer_len){
                host->m_packet_index = 0;
                return -1;
            }else if(host->m_packet_index == buffer_len){
                if(host->m_packet[SM_PROTO_DEVICE_ADDR_INDEX] != host->m_addr){
                    host->m_packet_index = 0;
                    return -1;
                }

                if(!sm_CRC_CalculateCRC16(&host->m_packet[SM_PROTO_START_BYTE_INDEX], host->m_packet_index)){
                    if(host->m_cmd_callback){
                        cmd = host->m_packet[SM_PROTO_CMD_INDEX];
                        host->m_cmd_callback(cmd,
                                             &host->m_packet[SM_PROTO_CMD_DATA_INDEX],
                                             host->m_packet_index - 7,
                                             host->m_cmd_arg);

                    }
                }
                host->m_packet_index = 0;
                return 1;
            }

        }else if((host->m_packet[host->m_packet_index] == SM_PROTO_START_BYTE && host->m_packet_index == SM_PROTO_START_BYTE_INDEX) ||
                 (host->m_packet_index > 0)){

            host->m_packet_index++;
            if(host->m_packet_index >= SM_HOST_MAX_BUFFER){
                host->m_packet_index = 0;
            }
        }
    }
    return 0;
}

int32_t sm_host_asyn_feed(const uint8_t* _data, int32_t _len, void* _user_data){
    sm_host_impl_t* host = (sm_host_impl_t*)_user_data;
    int32_t index = 0;
    if(!host){
        return -1;
    }
    for(index = 0; index < _len; index++){
        host->m_buffer[host->m_buffer_index++] = _data[index];
        if(host->m_buffer_index >= SM_HOST_MAX_BUFFER){
            host->m_buffer_index = 0;
        }
    }
    return 0;
}