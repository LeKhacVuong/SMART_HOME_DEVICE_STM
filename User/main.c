#include "board.h"
#include "sm_uart.h"
#include "sm_logger.h"
#include "stdio.h"
#include "sm_host.h"

#define TAG "main"

int _write(int file, char *ptr, int len)
{
	 int DataIdx;
	 for (DataIdx = 0; DataIdx < len; DataIdx++)
	 {
		 ITM_SendChar(*ptr++);
	 }
	 return len;
}

void log_put(const char* _log) {
    printf("%s\n", _log);
}

int64_t get_tick_count(){
	return HAL_GetTick();
}

typedef struct app_t{
	sm_uart_t* m_debug_com;
	sm_uart_t* m_bluetooth_com;
	dev_info_t m_devInfo;
	sm_host_t* m_host;
}app_t;
app_t app;

bool getLightStt(uint8_t _light){
    return app.m_devInfo.m_light[_light].m_stt;
}

void controlLight(uint8_t _light, uint8_t _stt){
    if(getLightStt(_light) == _stt)
        return;

    LOG_INF(TAG, "Now %s light %d at %dh - %dm", _stt?"ON":"OFF", _light, app.m_devInfo.m_devTime.m_hour, app.m_devInfo.m_devTime.m_min);
    app.m_devInfo.m_light[_light].m_stt = _stt;
    sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
}



int32_t serialHostSendIfDefault(const uint8_t* _data, int32_t _len, int32_t _timeout, void* _arg){
    return sm_uart_send_bytes(app.m_bluetooth_com, (uint8_t*)_data, _len, _timeout);
}

int32_t serialHostRecvIfDefault(uint8_t *_buf, int32_t _len, int32_t _timeout, void* _arg) {
    return sm_uart_get_bytes(app.m_bluetooth_com, _buf, _len, _timeout);
}

int32_t host_receive_cmd_callback(int32_t _cmd, const uint8_t* _data, int32_t _len, void* _arg ) {
    switch (_cmd) {
        case CMD_HOLDING_UPDATE_TIME:{
            uint8_t hour = _data[1];
            uint8_t min  = _data[0];
            LOG_INF(TAG, "Set local time to %2dh-%2dm", hour, min);
            app.m_devInfo.m_devTime.m_hour = hour;
            app.m_devInfo.m_devTime.m_min = min;
            break;
        }
        case CMD_HOLDING_OPEN_DOOR:
            LOG_INF(TAG, "Now %s door lock", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_doorStt = _data[0];
            break;
        case CMD_HOLDING_CONTROL_LIGHT_BED:
            LOG_INF(TAG, "Now %s bed room light", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_light[BED_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_BED_BRIGHTNESS:
            LOG_INF(TAG, "Now set bed light brightness to %d", _data[0]);
            app.m_devInfo.m_light[BED_LIGHT].m_brightness = _data[0];
            break;
        case CMD_HOLDING_CONTROL_FAN_BED:
            LOG_INF(TAG, "Now %s bed room fan", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_bedFan = _data[0];
            break;
        case CMD_HOLDING_CONTROL_LIGHT_LIVING:
            LOG_INF(TAG, "Now %s living room light", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_light[LIVING_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_LIVING_BRIGHTNESS:
            LOG_INF(TAG, "Now set living light brightness to %d", _data[0]);
            app.m_devInfo.m_light[LIVING_LIGHT].m_brightness = _data[0];
            break;
        case CMD_HOLDING_CONTROL_LIGHT_KITCHEN:
            LOG_INF(TAG, "Now %s kitchen light", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_light[KITCHEN_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_KITCHEN_BRIGHTNESS:
            LOG_INF(TAG, "Now set kitchen light brightness to %d", _data[0]);
            app.m_devInfo.m_light[KITCHEN_LIGHT].m_brightness = _data[0];
            break;
        case CMD_HOLDING_CONTROL_LIGHT_HALLWAY:
            LOG_INF(TAG, "Now %s hallway light", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_light[HALLWAY_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_HALLWAY_BRIGHTNESS:
            LOG_INF(TAG, "Now set hallway light brightness to %d", _data[0]);
            app.m_devInfo.m_light[HALLWAY_LIGHT].m_brightness = _data[0];
            break;
        case CMD_HOLDING_CONTROL_FIRE_BUZZER:
            LOG_INF(TAG, "Now %s fire buzzer", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_fireBuzzer = _data[0];
            break;
        case CMD_HOLDING_SET_AUTO_LIGHT:{
            LOG_INF(TAG, "Set auto light %d to %s", _data[0], _data[1] ? "ON" : "OFF");

            uint8_t light = _data[0];
            app.m_devInfo.m_light[light].m_auto = _data[1];
            break;
        }
        case CMD_HOLDING_SET_AUTO_TIME:{
            uint8_t light = _data[0];
            uint8_t cfId = _data[1];
            uint8_t hourStart = _data[2];
            uint8_t minStart = _data[3];
            uint8_t hourStop = _data[2];
            uint8_t minStop = _data[3];
            app.m_devInfo.m_light[light].m_autoConfigStart[cfId].m_hour = hourStart;
            app.m_devInfo.m_light[light].m_autoConfigStart[cfId].m_min = minStart;
            app.m_devInfo.m_light[light].m_autoConfigStop[cfId].m_hour = hourStop;
            app.m_devInfo.m_light[light].m_autoConfigStop[cfId].m_min = minStop;
            break;
        }
        case CMD_HOLDING_DELETE_AUTO_TIME:{
            uint8_t light = _data[0];
            uint8_t cfId = _data[1];
            app.m_devInfo.m_light[light].m_autoConfigStart[cfId].m_hour = 0;
            app.m_devInfo.m_light[light].m_autoConfigStart[cfId].m_min = 0;
            app.m_devInfo.m_light[light].m_autoConfigStop[cfId].m_hour = 0;
            app.m_devInfo.m_light[light].m_autoConfigStop[cfId].m_min = 0;
            break;
        }
        case CMD_HOLDING_HALLWAY_HUMAN_DETECT:
            LOG_INF(TAG, "Now %s hallway human detect", _data[0] ? "Enable" : "Disable");
            app.m_devInfo.m_hallwayDetectHuman = _data[0];
            break;
        case CMD_HOLDING_BLINK_LED_TEST:
            LOG_INF(TAG, "Blink led test...");
            break;
        default:
            break;
    }
    sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
    return 0;
}


void processAutoLight(){
    for(int id = 0; id < LIGHT_NUMBER; id++){
        if(app.m_devInfo.m_light[id].m_auto){
            for(int i = 0; i < 3; i++){
                if(isInTime(app.m_devInfo.m_devTime, app.m_devInfo.m_light[id].m_autoConfigStart[i], app.m_devInfo.m_light[id].m_autoConfigStop[i])){
                    controlLight(id, true);
                    return;
                }
            }
            controlLight(id, false);
        }
    }
}

void processAutoFan(){
    if(app.m_devInfo.m_autoFan){
        if(isInTime(app.m_devInfo.m_devTime, app.m_devInfo.m_autoFanStart, app.m_devInfo.m_autoFanStop)){
            if(!app.m_devInfo.m_bedFan){
                ///open fan
            	app.m_devInfo.m_bedFan = true;
                LOG_INF(TAG, "ON fan at %dh - %dm", app.m_devInfo.m_devTime.m_hour, app.m_devInfo.m_devTime.m_min);
                sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
            }
            return;
        }
        if(app.m_devInfo.m_bedFan){
            ///Off fan
            app.m_devInfo.m_bedFan = false;
            LOG_INF(TAG, "OFF fan at %dh - %dm", app.m_devInfo.m_devTime.m_hour, app.m_devInfo.m_devTime.m_min);
            sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
        }
    }

}

int main(void)
{
	sm_logger_init(log_put, LOG_LEVEL_DEBUG);
	board_init();
	app.m_bluetooth_com = sm_uart_create(&huart2, 9600, 256);
	app.m_debug_com = sm_uart_create(&huart1, 9600, 32);

    app.m_host = sm_host_create(SM_HOST_ASYNC_MODE, 0x99, serialHostSendIfDefault, serialHostRecvIfDefault, &app);
    sm_host_reg_handle(app.m_host, host_receive_cmd_callback, &app);

    sm_uart_send_bytes(app.m_bluetooth_com, (uint8_t*)"vuong\n", 6, 50);


  while (1)
  {
      uint8_t buf;
      int len = sm_uart_get_bytes(app.m_bluetooth_com, &buf, 1, 0);
      if(len > 0){
            LOG_INF(TAG, "feed 1 bytes is %s", &buf);
          sm_host_asyn_feed(&buf, 1, app.m_host);
      }

	  sm_host_process(app.m_host);
  }
}


void SysTick_Handler(void)
{
  HAL_IncTick();
}

void USART1_IRQHandler(void)
{
	sm_uart_recv_handle(app.m_debug_com);
}

void USART2_IRQHandler(void)
{
	sm_uart_recv_handle(app.m_bluetooth_com);
}



