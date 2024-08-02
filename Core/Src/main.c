
#include "board.h"
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
	dev_info_t m_devInfo;
	sm_host_t* m_host;

	uint8_t m_rxBuffer;
}app_t;
app_t app;

bool getLightStt(uint8_t _light){
    return app.m_devInfo.m_light[_light].m_stt;
}

void setLightBrightness(uint8_t _light, uint8_t _brightness){
	 app.m_devInfo.m_light[_light].m_brightness = _brightness;

	 if(!app.m_devInfo.m_light[_light].m_stt){
		 return;
	 }
	    switch (_light) {
			case BED_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4, _brightness);
				break;
			case KITCHEN_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3, _brightness);
				break;
			case LIVING_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, _brightness);
				break;
			case HALLWAY_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1, _brightness);
				break;
			default:
				break;
		}

}

void controlLight(uint8_t _light, uint8_t _stt){
    if(getLightStt(_light) == _stt)
        return;

    uint8_t value = _stt?app.m_devInfo.m_light[_light].m_brightness:0;

    switch (_light) {
		case BED_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4, value);
			break;
		case KITCHEN_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3, value);
			break;
		case LIVING_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2, value);
			break;
		case HALLWAY_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1, value);
			break;
		default:
			break;
	}


    LOG_INF(TAG, "Now %s light %d at %dh - %dm", _stt?"ON":"OFF", _light, app.m_devInfo.m_devTime.m_hour, app.m_devInfo.m_devTime.m_min);
    app.m_devInfo.m_light[_light].m_stt = _stt;
    sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
}



int32_t serialHostSendIfDefault(const uint8_t* _data, int32_t _len, int32_t _timeout, void* _arg){
    return HAL_UART_Transmit(&huart3, (uint8_t*)_data, _len, _timeout);
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
            HAL_GPIO_WritePin(OUT_DOOR_RL_GPIO_Port, OUT_DOOR_RL_Pin, _data[0]);
            app.m_devInfo.m_doorStt = _data[0];
            break;
        case CMD_HOLDING_CONTROL_LIGHT_BED:
            LOG_INF(TAG, "Now %s bed room light", _data[0] ? "Open" : "Lock");
            app.m_devInfo.m_light[BED_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_BED_BRIGHTNESS:
            LOG_INF(TAG, "Now set bed light brightness to %d", _data[0]);
            setLightBrightness(BED_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_FAN_BED:
            LOG_INF(TAG, "Now %s bed room fan", _data[0] ? "Open" : "Lock");
            controlLight(OUT_FAN_RL_Pin, _data[0]);
            HAL_GPIO_WritePin(OUT_FAN_RL_GPIO_Port, OUT_FAN_RL_Pin, _data[0]);
            app.m_devInfo.m_bedFan = _data[0];
            break;
        case CMD_HOLDING_CONTROL_LIGHT_LIVING:
            LOG_INF(TAG, "Now %s living room light", _data[0] ? "Open" : "Lock");
            controlLight(LIVING_LIGHT, _data[0]);
            app.m_devInfo.m_light[LIVING_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_LIVING_BRIGHTNESS:
            LOG_INF(TAG, "Now set living light brightness to %d", _data[0]);
            setLightBrightness(LIVING_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_LIGHT_KITCHEN:
            LOG_INF(TAG, "Now %s kitchen light", _data[0] ? "Open" : "Lock");
            controlLight(KITCHEN_LIGHT, _data[0]);
            app.m_devInfo.m_light[KITCHEN_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_KITCHEN_BRIGHTNESS:
            LOG_INF(TAG, "Now set kitchen light brightness to %d", _data[0]);
            setLightBrightness(KITCHEN_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_LIGHT_HALLWAY:
            LOG_INF(TAG, "Now %s hallway light", _data[0] ? "Open" : "Lock");
            controlLight(HALLWAY_LIGHT, _data[0]);
            app.m_devInfo.m_light[HALLWAY_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_HALLWAY_BRIGHTNESS:
            LOG_INF(TAG, "Now set hallway light brightness to %d", _data[0]);
            setLightBrightness(HALLWAY_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_FIRE_BUZZER:
            LOG_INF(TAG, "Now %s fire buzzer", _data[0] ? "Open" : "Lock");
            HAL_GPIO_WritePin(OUT_BUZZ_GPIO_Port, OUT_BUZZ_Pin, _data[0]);
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
            uint8_t hourStop = _data[4];
            uint8_t minStop = _data[5];
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
	HAL_UART_Receive_IT(&huart3, &app.m_rxBuffer, 1);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    app.m_host = sm_host_create_default(SM_HOST_ASYNC_MODE, 0x99, serialHostSendIfDefault, NULL, &app);
    sm_host_reg_handle(app.m_host, host_receive_cmd_callback, &app);

    LOG_INF(TAG, "Start application");

  while (1)
  {
	  sm_host_process(app.m_host);
  }
}

void USART3_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart3);
	HAL_UART_Receive_IT(&huart3, &app.m_rxBuffer, 1);
	sm_host_asyn_feed(&app.m_rxBuffer, 1, app.m_host);
	LOG_INF(TAG, "feed a byte %d", app.m_rxBuffer);
}



