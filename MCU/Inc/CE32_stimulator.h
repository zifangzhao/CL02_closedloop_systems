#include "stm32f3xx_hal.h"
#include "dataMGR.h"

#ifndef __CE32_STIMULATOR
#define __CE32_STIMULATOR

//#define __USE_PWM

#define CE32_STIM_STATE_ON 0x0001
#define CE32_STIM_STATE_DELAY 0x0002
#define CE32_STIM_STATE_TRIG 0x0004
#define CE32_STIM_STATE_STIM 0x0008
#define CE32_STIM_STATE_REFRACT 0x0010
#define CE32_STIM_STATE_ERR 0x0020

#define CE32_STIM_TIMEBASECLK 10000
//Unit is in 0.1ms;
typedef struct{
	TIM_HandleTypeDef *htim;
	uint16_t CC_channel;
	uint16_t CC_channel_IT;
	uint16_t state;
	uint16_t delay;
	uint16_t random_delay;
	uint32_t delay_this;
	uint16_t interval;//interval between 2 stimulations
	uint16_t duration;
	uint16_t PWM;
	uint16_t cycle;
	uint16_t cycle_left;
	uint16_t level;
	uint16_t id;
}CE32_stimulator;

void CE32_STIM_Init(CE32_stimulator* handle,TIM_HandleTypeDef* htim,uint16_t cc_channel,uint16_t unit_id);
void CE32_STIM_Setup(CE32_stimulator* handle,uint16_t interval,uint16_t delay,uint16_t duration, uint16_t cycle, uint16_t PWM);
void CE32_STIM_ENABLE(CE32_stimulator* handle);
void CE32_STIM_DISABLE(CE32_stimulator* handle);
void CE32_STIM_Delay(CE32_stimulator* handle);
void CE32_STIM_Trig(CE32_stimulator* handle);
void CE32_STIM_Stim(CE32_stimulator* handle);
void CE32_STIM_StimOff(CE32_stimulator* handle); //routine to turn off stimulation under non-pwm mode
void CE32_STIM_Refract(CE32_stimulator* handle);
void CE32_STIM_Stop(CE32_stimulator* handle);
void CE32_STIM_Abort(CE32_stimulator* handle);
void CE32_STIM_SetLevel(CE32_stimulator* handle, uint16_t level);

void CE32_STIM_IT(CE32_stimulator* handle);
void CE32_STIM_TrigAct(CE32_stimulator* handle);//Trigger Onset action, Define this action in IT
void CE32_STIM_TrigStopAct(CE32_stimulator* handle);
void CE32_STIM_StimAct(CE32_stimulator* handle);//Stimulation Onset action, Define this action in IT
void CE32_STIM_StimOffAct(CE32_stimulator* handle);//Stimulation close for each cycle(when PWM mode is not used), Define this action in IT
void CE32_STIM_StimStopAct(CE32_stimulator* handle);//Stimulation Abort action, Define this action in IT
void CE32_STIM_StimAbortAct(CE32_stimulator* handle);//Stimulation Abort action, Define this action in IT
void CE32_STIM_SetLevelAct(CE32_stimulator* handle);
#endif
