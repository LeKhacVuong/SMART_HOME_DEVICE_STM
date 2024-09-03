#include "board.h"
#include "sm_logger.h"
#include "sm_host.h"
#include "stm32l1xx_hal_flash_ex.h"
#include "string.h"
#include "rc522.h"

#define TAG "main"

char KEY_1[5] = {211, 180, 5, 45, 79};
char KEY_2[5] = {195, 63, 153, 41, 76};

#define CONFIG_FLASH_ADDR ((uint32_t)0x0801FC00)
#define MAGIC_VALUE 0x5A5A5A5A
#define FIRE_TIMEOUT 3000

void controlLight(uint8_t _light, uint8_t _stt);


void log_put(const char* _log) {
    printf("%s\n", _log);
}

int64_t get_tick_count(){
	return HAL_GetTick();
}


int32_t serialHostSendIfDefault(const uint8_t* _data, int32_t _len, int32_t _timeout, void* _arg){
    return HAL_UART_Transmit(&huart2, (uint8_t*)_data, _len, _timeout);
}

typedef struct app_t{
	dev_info_t m_devInfo;
	sm_host_t* m_host;
	uint8_t m_rxBuffer;
	elapsed_timer_t m_hallwayLightTimer;
	elapsed_timer_t m_offLockTimer;
	elapsed_timer_t m_updateEviTimer;
	elapsed_timer_t m_fireWarningTimer;
	uint8_t m_force_light[4];
	uint8_t m_focre_fan;
	uint32_t m_minCount;
}app_t;
app_t app;

void saveConfig(){
	HAL_FLASH_Unlock();
    FLASH_EraseInitTypeDef EraseInitStruct;
    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = CONFIG_FLASH_ADDR;
    EraseInitStruct.NbPages     = 1;
    uint32_t PAGEError;

    if(HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)    {
    	PAGEError = HAL_FLASH_GetError();
    }

    uint32_t configData[256] = {0,};

    configData[0] = MAGIC_VALUE;

    memcpy(&configData[1], &app.m_devInfo, sizeof(dev_info_t));

    for(int i = 0; i < 256; i++){
    	 if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, CONFIG_FLASH_ADDR + i*4, configData[i]) != HAL_OK){
    		 PAGEError = HAL_FLASH_GetError();
    		 LOG_INF(TAG, "Save config FAILED");
    	 }
    }


    LOG_INF(TAG, "Save config SUCCEED");
	HAL_FLASH_Lock();
}

void loadConfig(){
	uint8_t* storage_buff = (uint8_t*)CONFIG_FLASH_ADDR;
	uint32_t configData[256] = {0,};

	memcpy(configData, storage_buff, sizeof(configData));

	if(configData[0] == MAGIC_VALUE){
		memcpy(&app.m_devInfo, &configData[1], sizeof(dev_info_t));
		if(app.m_devInfo.m_hallwayDetectHuman)
			app.m_devInfo.m_light[HALLWAY_LIGHT].m_stt = false;

		app.m_devInfo.m_doorStt = false;
		app.m_devInfo.m_fireBuzzer = false;
	}

	char emptyArray[8] = {0,}; // Tạo mảng chứa toàn số 0

	if(memcmp(app.m_devInfo.m_doorKey, emptyArray, 8) == 0){
		memset(app.m_devInfo.m_doorKey, '8', 8);
	}

	LOG_INF(TAG, "Door key is %s", app.m_devInfo.m_doorKey);

	for(int id = 0; id < LIGHT_NUMBER; id++){
		controlLight(id, app.m_devInfo.m_light[id].m_stt);
	}

	app.m_devInfo.m_humi = 61;
	app.m_devInfo.m_temp = 22;

	HAL_GPIO_WritePin(OUT_DOOR_GPIO_Port, OUT_DOOR_Pin, app.m_devInfo.m_doorStt);
	HAL_GPIO_WritePin(OUT_FAN_GPIO_Port, OUT_FAN_Pin, app.m_devInfo.m_bedFan);
	HAL_GPIO_WritePin(OUT_BUZZER_GPIO_Port, OUT_BUZZER_Pin, app.m_devInfo.m_fireBuzzer);
}


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
				 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4, _brightness);
				break;
			case KITCHEN_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3, _brightness);
				break;
			case LIVING_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2, _brightness);
				break;
			case HALLWAY_LIGHT:
				 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, _brightness);
				break;
			default:
				break;
		}

}

void controlLight(uint8_t _light, uint8_t _stt){
    uint8_t value = _stt?app.m_devInfo.m_light[_light].m_brightness:0;

    switch (_light) {
		case BED_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4, value);
			break;
		case KITCHEN_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3, value);
			break;
		case LIVING_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2, value);
			break;
		case HALLWAY_LIGHT:
			 __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_1, value);
			break;
		default:
			break;
	}

//    saveConfig();
    LOG_INF(TAG, "Now %s light %d at %dh - %dm", _stt?"ON":"OFF", _light, app.m_devInfo.m_devTime.m_hour, app.m_devInfo.m_devTime.m_min);
    app.m_devInfo.m_light[_light].m_stt = _stt;
}


int32_t host_receive_cmd_callback(int32_t _cmd, const uint8_t* _data, int32_t _len, void* _arg ) {
    switch (_cmd) {
        case CMD_HOLDING_UPDATE_TIME:{
            uint8_t hour = _data[1];
            uint8_t min  = _data[0];
//            LOG_INF(TAG, "Set local time to %2dh-%2dm", hour, min);
            app.m_minCount = 0;
            app.m_devInfo.m_devTime.m_hour = hour;
            app.m_devInfo.m_devTime.m_min = min;
            sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
            return 0;
            break;
        }
        case CMD_HOLDING_OPEN_DOOR:
            LOG_INF(TAG, "Now %s door lock", _data[0] ? "Open" : "Lock");
            HAL_GPIO_WritePin(OUT_DOOR_GPIO_Port, OUT_DOOR_Pin, _data[0]);
            app.m_devInfo.m_doorStt = _data[0];
            elapsed_timer_reset(&app.m_offLockTimer);
            break;
        case CMD_HOLDING_CONTROL_LIGHT_BED:
            LOG_INF(TAG, "Now %s bed room light", _data[0] ? "Open" : "Lock");
            controlLight(BED_LIGHT, _data[0]);
            app.m_force_light[BED_LIGHT] = true;
            app.m_devInfo.m_light[BED_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_BED_BRIGHTNESS:
            LOG_INF(TAG, "Now set bed light brightness to %d", _data[0]);
            setLightBrightness(BED_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_FAN_BED:
            LOG_INF(TAG, "Now %s bed room fan", _data[0] ? "Open" : "Lock");
            HAL_GPIO_WritePin(OUT_FAN_GPIO_Port, OUT_FAN_Pin, _data[0]);
            app.m_devInfo.m_bedFan = _data[0];
            app.m_focre_fan = true;
            break;
        case CMD_HOLDING_CONTROL_LIGHT_LIVING:
            LOG_INF(TAG, "Now %s living room light", _data[0] ? "Open" : "Lock");
            controlLight(LIVING_LIGHT, _data[0]);
            app.m_force_light[LIVING_LIGHT] = true;
            app.m_devInfo.m_light[LIVING_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_LIVING_BRIGHTNESS:
            LOG_INF(TAG, "Now set living light brightness to %d", _data[0]);
            setLightBrightness(LIVING_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_LIGHT_KITCHEN:
            LOG_INF(TAG, "Now %s kitchen light", _data[0] ? "Open" : "Lock");
            controlLight(KITCHEN_LIGHT, _data[0]);
            app.m_force_light[KITCHEN_LIGHT] = true;
            app.m_devInfo.m_light[KITCHEN_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_KITCHEN_BRIGHTNESS:
            LOG_INF(TAG, "Now set kitchen light brightness to %d", _data[0]);
            setLightBrightness(KITCHEN_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_LIGHT_HALLWAY:
            LOG_INF(TAG, "Now %s hallway light", _data[0] ? "Open" : "Lock");
            controlLight(HALLWAY_LIGHT, _data[0]);
            app.m_force_light[HALLWAY_LIGHT] = true;
            app.m_devInfo.m_light[HALLWAY_LIGHT].m_stt = _data[0];
            break;
        case CMD_HOLDING_SET_HALLWAY_BRIGHTNESS:
            LOG_INF(TAG, "Now set hallway light brightness to %d", _data[0]);
            setLightBrightness(HALLWAY_LIGHT, _data[0]);
            break;
        case CMD_HOLDING_CONTROL_FIRE_BUZZER:
            LOG_INF(TAG, "Now %s fire buzzer", _data[0] ? "Open" : "Lock");
            HAL_GPIO_WritePin(OUT_BUZZER_GPIO_Port, OUT_BUZZER_Pin, _data[0]);
            app.m_devInfo.m_fireBuzzer = _data[0];
            elapsed_timer_resetz(&app.m_fireWarningTimer, FIRE_TIMEOUT);
            break;
        case CMD_HOLDING_SET_AUTO_LIGHT:{
            LOG_INF(TAG, "Set auto light %d to %s", _data[0], _data[1] ? "ON" : "OFF");

            uint8_t light = _data[0];
            app.m_devInfo.m_light[light].m_auto = _data[1];
            if(!app.m_devInfo.m_light[light].m_auto){
            	if(app.m_devInfo.m_light[light].m_stt){
            		controlLight(light, false);
            	}
            }
            break;
        }
        case CMD_HOLDING_SET_AUTO_TIME:{
            uint8_t light = _data[0];
            uint8_t cfId = _data[1];
            app.m_devInfo.m_light[light].m_autoConfigStart[cfId].m_hour = _data[2];
            app.m_devInfo.m_light[light].m_autoConfigStart[cfId].m_min = _data[3];
            app.m_devInfo.m_light[light].m_autoConfigStop[cfId].m_hour = _data[4];
            app.m_devInfo.m_light[light].m_autoConfigStop[cfId].m_min = _data[5];
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
            if(app.m_devInfo.m_light[HALLWAY_LIGHT].m_stt){
            	controlLight(HALLWAY_LIGHT, 0);
            }
            break;
        case CMD_HOLDING_SET_AUTO_FAN:
            app.m_devInfo.m_autoFan = _data[0];
            if(app.m_devInfo.m_autoFan){
                uint8_t hourStart = _data[1];
                uint8_t minStart = _data[2];
                uint8_t hourStop = _data[3];
                uint8_t minStop = _data[4];
                app.m_devInfo.m_autoFanStart.m_hour = hourStart;
                app.m_devInfo.m_autoFanStart.m_min = minStart;
                app.m_devInfo.m_autoFanStop.m_hour = hourStop;
                app.m_devInfo.m_autoFanStop.m_min = minStop;
            }else{
            	if(app.m_devInfo.m_bedFan){
            		app.m_devInfo.m_bedFan = false;
            		HAL_GPIO_WritePin(OUT_FAN_GPIO_Port, OUT_FAN_Pin, false);
            	}
                app.m_devInfo.m_autoFanStart.m_hour = 0;
                app.m_devInfo.m_autoFanStart.m_min = 0;
                app.m_devInfo.m_autoFanStop.m_hour = 0;
                app.m_devInfo.m_autoFanStop.m_min = 0;
            }


        	break;
        case CMD_HOLDING_BLINK_LED_TEST:
//            LOG_INF(TAG, "Blink led test...");
            break;
        case CMD_HOLDING_SET_DOOR_KEY:

        	memcpy(app.m_devInfo.m_doorKey, _data, 8);
        	LOG_INF(TAG, "Change pass to %s", _data);
            saveConfig();
        	break;
        default:
            break;
    }
    sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
    return 0;
}

void processAutoLight(){
    for(int id = 0; id < LIGHT_NUMBER; id++){
    	if(id == HALLWAY_LIGHT && app.m_devInfo.m_hallwayDetectHuman)
    		continue;

        if(app.m_devInfo.m_light[id].m_auto){
        	int inTime = 0;
            for(int i = 0; i < 3; i++){
                if(isInTime(app.m_devInfo.m_devTime, app.m_devInfo.m_light[id].m_autoConfigStart[i], app.m_devInfo.m_light[id].m_autoConfigStop[i])){
                  if(!app.m_devInfo.m_light[id].m_stt){
                  	controlLight(id, true);
                    app.m_force_light[id] = false;
            		sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
                  }
                  inTime = true;
                  break;
                }
            }
            if(app.m_devInfo.m_light[id].m_stt && !app.m_force_light[id] && !inTime){
            	controlLight(id, false);
        		sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
            }
        }
    }
}

void processAutoFan(){
    if(app.m_devInfo.m_autoFan){
        if(isInTime(app.m_devInfo.m_devTime, app.m_devInfo.m_autoFanStart, app.m_devInfo.m_autoFanStop)){
            if(!app.m_devInfo.m_bedFan){
                HAL_GPIO_WritePin(OUT_FAN_GPIO_Port, OUT_FAN_Pin, true);
            	app.m_devInfo.m_bedFan = true;
            	app.m_focre_fan = false;
                sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
            }
            return;
        }else{
            if(app.m_devInfo.m_bedFan && !app.m_focre_fan){
                HAL_GPIO_WritePin(OUT_FAN_GPIO_Port, OUT_FAN_Pin, false);
                app.m_devInfo.m_bedFan = false;
                sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t *) &app.m_devInfo, sizeof(dev_info_t));
            }
        }
    }
}


int main(void)
{
	board_init();
	sm_logger_init(log_put, LOG_LEVEL_DEBUG);

	HAL_UART_Receive_IT(&huart2, &app.m_rxBuffer, 1);

	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

    app.m_host = sm_host_create_default(SM_HOST_ASYNC_MODE, 0x99, serialHostSendIfDefault, NULL, &app);
    sm_host_reg_handle(app.m_host, host_receive_cmd_callback, &app);
    elapsed_timer_resetz(&app.m_offLockTimer, 3000);
    elapsed_timer_resetz(&app.m_updateEviTimer, 500);
    loadConfig();

  while (1)
  {
	  while (1) {
	  		sm_host_process(app.m_host);
	  		if (!elapsed_timer_get_remain(&app.m_updateEviTimer)) {
	  			elapsed_timer_reset(&app.m_updateEviTimer);

	  			if (app.m_devInfo.m_hallwayDetectHuman) {
	  				if (HAL_GPIO_ReadPin(IN_HUMAN_DECT_GPIO_Port, IN_HUMAN_DECT_Pin)
	  						&& !elapsed_timer_get_remain(&app.m_hallwayLightTimer)) {
	  					elapsed_timer_resetz(&app.m_hallwayLightTimer, 5000);
	  					controlLight(HALLWAY_LIGHT, true);
	  				}
	  				if (!elapsed_timer_get_remain(&app.m_hallwayLightTimer) && app.m_devInfo.m_light[HALLWAY_LIGHT].m_stt) {
	  					controlLight(HALLWAY_LIGHT, false);
	  				}
	  			}
	  			if (app.m_devInfo.m_doorStt && !elapsed_timer_get_remain(&app.m_offLockTimer)) {
	  				app.m_devInfo.m_doorStt = false;
	  				HAL_GPIO_WritePin(OUT_DOOR_GPIO_Port, OUT_DOOR_Pin, app.m_devInfo.m_doorStt);
	  				sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t*) &app.m_devInfo, sizeof(dev_info_t));
	  			}
	  			processAutoLight();
	  			processAutoFan();


	  			if(!HAL_GPIO_ReadPin(IN_FIRE_SS_GPIO_Port, IN_FIRE_SS_Pin) && !elapsed_timer_get_remain(&app.m_fireWarningTimer)){
	  				app.m_devInfo.m_fireBuzzer = app.m_devInfo.m_fireSensor = true;
	  	     		HAL_GPIO_WritePin(OUT_BUZZER_GPIO_Port, OUT_BUZZER_Pin, true);
	  				sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t*) &app.m_devInfo, sizeof(dev_info_t));
	  				LOG_INF(TAG, "Fire!!!");
	  				elapsed_timer_resetz(&app.m_fireWarningTimer, FIRE_TIMEOUT);
	  			}
	  			if(!elapsed_timer_get_remain(&app.m_fireWarningTimer) && app.m_devInfo.m_fireBuzzer){
	  				app.m_devInfo.m_fireBuzzer = app.m_devInfo.m_fireSensor = false;
	  	     		HAL_GPIO_WritePin(OUT_BUZZER_GPIO_Port, OUT_BUZZER_Pin, false);
	  				sm_host_send_cmd(app.m_host, CMD_HOLDING_DEV_SEND_SYNC_DATA, (const uint8_t*) &app.m_devInfo, sizeof(dev_info_t));
	  			}
	  		}

	  	}
  }
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
	HAL_UART_Receive_IT(&huart2, &app.m_rxBuffer, 1);
	sm_host_asyn_feed(&app.m_rxBuffer, 1, app.m_host);
//	LOG_INF(TAG, "feed a byte %d", app.m_rxBuffer);
}

void SysTick_Handler(void)
{
	app.m_minCount++;
	if(app.m_minCount >= 60000){
		app.m_minCount = 0;
		increaseTimeMin(&app.m_devInfo.m_devTime);

		if(app.m_devInfo.m_temp == 22){
			app.m_devInfo.m_temp = 21;
		}else if(app.m_devInfo.m_temp == 21){
			app.m_devInfo.m_temp = 23;
		}else{
			app.m_devInfo.m_temp = 22;
		}

		if(app.m_devInfo.m_humi == 61){
			app.m_devInfo.m_humi = 59;
		}else if(app.m_devInfo.m_humi == 59){
			app.m_devInfo.m_humi = 62;
		}else{
			app.m_devInfo.m_humi = 61;
		}
	}

  HAL_IncTick();

}



