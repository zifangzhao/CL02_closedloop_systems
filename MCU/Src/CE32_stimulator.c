#include "CE32_stimulator.h"
#include <stdlib.h>

void CE32_STIM_Init(CE32_stimulator* handle,TIM_HandleTypeDef* htim,uint16_t CC_channel,uint16_t unit_id){
	handle->id=unit_id;
	handle->htim = htim;
	handle->CC_channel=CC_channel;
	handle->CC_channel_IT=0x01<<(CC_channel);
	handle->htim->Init.AutoReloadPreload=TIM_AUTORELOAD_PRELOAD_ENABLE;
	handle->htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	handle->htim->Init.Prescaler=HAL_RCC_GetSysClockFreq()/CE32_STIM_TIMEBASECLK-1;
	handle->trig_mode = 0;
}

#ifdef __USE_PWM
void CE32_STIM_Setup(CE32_stimulator* handle,uint16_t interval,uint16_t delay,uint16_t duration, uint16_t cycle, uint16_t PWM){
	handle->interval=interval;
	handle->cycle=cycle-1;
	handle->duration=duration;
	handle->delay=delay;
	int64_t temp;
	handle->level=65535;
	handle->PWM=PWM;
	__IO uint32_t	*CCR;
	CCR=(&handle->htim->Instance->CCR1)+handle->CC_channel*4;
	*CCR=PWM;
	handle->htim->Init.Period=delay;	//Use delay as initial trigger time
  handle->htim->Init.RepetitionCounter = 0;
}
#else
void CE32_STIM_Setup(CE32_stimulator* handle,uint16_t interval,uint16_t delay,uint16_t duration, uint16_t cycle, uint16_t PWM){
	handle->interval=interval;
	handle->cycle=cycle-1;
	handle->duration=duration;
	handle->delay=delay;
	int64_t temp;
	handle->level=65535;
	handle->PWM=PWM;
	__IO uint32_t	*CCR;
	CCR=(&handle->htim->Instance->CCR1)+handle->CC_channel*4;
	*CCR=PWM;
	handle->htim->Init.Period=delay;	//Use delay as initial trigger time
  handle->htim->Init.RepetitionCounter = 0;
}
#endif
void CE32_STIM_ENABLE(CE32_stimulator* handle){
	handle->state|=CE32_STIM_STATE_ON;
}
void CE32_STIM_DISABLE(CE32_stimulator* handle){
	handle->state&=~CE32_STIM_STATE_ON;
	CE32_STIM_StimAbortAct(handle);
}

void CE32_STIM_Trig(CE32_stimulator* handle){
	uint32_t flag;
	if(handle->trig_mode ==0)
	{
		flag = CE32_STIM_STATE_DELAY|CE32_STIM_STATE_STIM|CE32_STIM_STATE_REFRACT; //trig on first trigger
	}
	else
	{
		flag = CE32_STIM_STATE_STIM|CE32_STIM_STATE_REFRACT; //trig on last trigger
	}
	if((handle->state&flag)==0)
	{
		handle->state|=CE32_STIM_STATE_TRIG;
		CE32_STIM_TrigAct(handle);
		uint32_t rand_delay=handle->random_delay*(rand()%UINT16_MAX);
		rand_delay/=UINT16_MAX;
		handle->delay_this=handle->delay+rand_delay;
		handle->cycle_left=handle->cycle;
		if(handle->delay_this!=0){
			CE32_STIM_Delay(handle);
		}
		else{
//			if(handle->state&CE32_STIM_STATE_ON)
//			{
				CE32_STIM_Stim(handle);
//			}
		}
	}
}
void CE32_STIM_Delay(CE32_stimulator* handle){
	handle->state|=CE32_STIM_STATE_DELAY;
	uint32_t temp=handle->delay_this;
	temp=temp>UINT16_MAX?UINT16_MAX:temp;			//Make sure temp is not saturated 
	handle->htim->Init.Period=temp;
	handle->htim->Init.RepetitionCounter = 0;
	handle->htim->Instance->ARR=handle->delay;
	handle->htim->Instance->RCR=0;
	HAL_TIM_Base_Init(handle->htim);
	handle->htim->Instance->SR&=~(TIM_IT_UPDATE|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4);
	HAL_TIM_Base_Start_IT(handle->htim);
}

void CE32_STIM_StimOff(CE32_stimulator* handle){
	handle->state&=~(CE32_STIM_STATE_STIM);
	handle->state|=CE32_STIM_STATE_REFRACT;
	TIM_CCxChannelCmd(handle->htim->Instance, 4*handle->CC_channel, TIM_CCx_DISABLE);
	CE32_STIM_StimOffAct(handle);
}

void CE32_STIM_Refract(CE32_stimulator* handle)
{
	handle->state&=~(CE32_STIM_STATE_DELAY|CE32_STIM_STATE_STIM);
	handle->state|=CE32_STIM_STATE_REFRACT;
	HAL_TIM_Base_Stop_IT(handle->htim);
	//handle->htim->Instance->CNT=0;
	handle->htim->Init.Period=handle->interval;
	handle->htim->Init.RepetitionCounter =0;
	HAL_TIM_Base_Init(handle->htim);
	HAL_TIM_Base_Start_IT(handle->htim);
}

void CE32_STIM_Abort(CE32_stimulator* handle){
	HAL_TIM_Base_Stop_IT(handle->htim);
	__HAL_TIM_MOE_DISABLE(handle->htim);
	CE32_STIM_StimStopAct(handle);
	CE32_STIM_TrigStopAct(handle);
	handle->state=CE32_STIM_STATE_ERR;
}

void CE32_STIM_SetLevel(CE32_stimulator* handle, uint16_t level){
	handle->level=level;
	CE32_STIM_SetLevelAct(handle);
}


#ifndef __USE_PWM
//Interrupt service routine for timer event
void CE32_STIM_IT(CE32_stimulator* handle){
	//Three phases
	//1. (optional) delay phase #CE32_STIM_STATE_DELAY
	//2. Stim phase #CE32_STIM_STATE_STIM
	//3. (optional)Refractory period CE32_STIM_STATE_REFRACT
	
	//delay->[stim->stimOff(CC)->Refract]xN->stim_stop
	uint16_t state=handle->htim->Instance->SR;
	handle->htim->Instance->SR&=~(TIM_IT_UPDATE|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4);
	if((state&(TIM_IT_UPDATE))==0){ //capture event for stimulation off, CC_IT seems set automatically after UR
		CE32_STIM_StimOff(handle);
	}
	else{
		//Enters here with main UR interrupt
		if((handle->state&CE32_STIM_STATE_DELAY)!=0){ //If Delay is set,triggers after delay phase
			CE32_STIM_Stim(handle);
		}
		else{
			if((handle->state&CE32_STIM_STATE_TRIG)!=0){ //Enters here after one cycle of stimulation
				if(handle->cycle_left==0){
					CE32_STIM_Stop(handle);
				}
				else{
					CE32_STIM_Stim(handle);
				}
			}
		}
	}
}

void CE32_STIM_Stim(CE32_stimulator* handle){
	HAL_TIM_Base_Stop_IT(handle->htim);
	
	handle->state&=~CE32_STIM_STATE_DELAY;
	handle->state|=CE32_STIM_STATE_STIM;
	
	handle->htim->Init.Period=handle->interval;
	HAL_TIM_Base_Init(handle->htim);
	
	__IO uint32_t	*CCR;
	CCR=(&handle->htim->Instance->CCR1)+handle->CC_channel*4;
	*CCR=handle->duration;
	TIM_CCxChannelCmd(handle->htim->Instance, 4*handle->CC_channel, TIM_CCx_ENABLE);
	
	handle->htim->Instance->SR&=~(TIM_IT_UPDATE|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4);
	__HAL_TIM_ENABLE_IT(handle->htim,handle->CC_channel_IT<<1); //Enable CC1 triger
	
	HAL_TIM_Base_Start_IT(handle->htim);
	CE32_STIM_StimAct(handle);
	handle->cycle_left--;
}

void CE32_STIM_Stop(CE32_stimulator* handle){
	HAL_TIM_Base_Stop_IT(handle->htim);
	TIM_CCxChannelCmd(handle->htim->Instance, 4*handle->CC_channel, TIM_CCx_DISABLE);
	
	handle->state&=~(CE32_STIM_STATE_TRIG|CE32_STIM_STATE_STIM|CE32_STIM_STATE_REFRACT);
	CE32_STIM_StimOffAct(handle);
	CE32_STIM_TrigStopAct(handle);
}
#else
//Interrupt service routine for timer event with PWM mode
void CE32_STIM_IT(CE32_stimulator* handle){
	//Three phases
	//1. (optional) delay phase #CE32_STIM_STATE_DELAY
	//2. Stim phase #CE32_STIM_STATE_STIM
	//3. (optional)Refractory period CE32_STIM_STATE_REFRACT
	
	handle->htim->Instance->SR&=~(TIM_IT_UPDATE|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4);
	if((handle->state&CE32_STIM_STATE_DELAY)!=0){ //If Delay is set
		CE32_STIM_Stim(handle);
	}
	else{
		if((handle->state&CE32_STIM_STATE_STIM)!=0){
			if(handle->interval>0){
				CE32_STIM_Refract(handle);
			}
			else{	
				CE32_STIM_Stop(handle);
			}
		}
		else{
			CE32_STIM_Stop(handle);
		}
	}
}

void CE32_STIM_Stim(CE32_stimulator* handle){
	handle->state&=~CE32_STIM_STATE_DELAY;
	handle->state|=CE32_STIM_STATE_STIM;
	HAL_TIM_Base_Stop_IT(handle->htim);
	//handle->htim->Instance->CNT=0;
	
	handle->htim->Init.Period=handle->interval;
	int cyc=handle->cycle-1;
	cyc=cyc<0?0:cyc;
	handle->htim->Init.RepetitionCounter =cyc;
	__IO uint32_t	*CCR;
	CCR=(&handle->htim->Instance->CCR1)+handle->CC_channel*4;
	*CCR=handle->duration;
	HAL_TIM_Base_Init(handle->htim);
	handle->htim->Instance->SR&=~(TIM_IT_UPDATE|TIM_IT_CC1|TIM_IT_CC2|TIM_IT_CC3|TIM_IT_CC4);
	//handle->htim->Instance->ARR=handle->duration;
	//handle->htim->Instance->RCR=handle->cycle;
	TIM_CCxChannelCmd(handle->htim->Instance, 4*handle->CC_channel, TIM_CCx_ENABLE);
	__HAL_TIM_MOE_ENABLE(handle->htim);
	HAL_TIM_Base_Start_IT(handle->htim);
	CE32_STIM_StimAct(handle);
}

void CE32_STIM_Stop(CE32_stimulator* handle){
	HAL_TIM_Base_Stop_IT(handle->htim);
	__HAL_TIM_MOE_DISABLE(handle->htim);
	TIM_CCxChannelCmd(handle->htim->Instance, 4*handle->CC_channel, TIM_CCx_DISABLE);
	
	if((handle->state&CE32_STIM_STATE_REFRACT)!=0){
		CE32_STIM_StimStopAct(handle);
	}
	handle->state&=~(CE32_STIM_STATE_TRIG|CE32_STIM_STATE_STIM|CE32_STIM_STATE_REFRACT);
	CE32_STIM_TrigStopAct(handle);
}
#endif
__weak void CE32_STIM_TrigAct(CE32_stimulator* handle)//Trigger Onset action, Define this action in IT
{
	__nop();
}

__weak void CE32_STIM_TrigStopAct(CE32_stimulator* handle)//Trigger Onset action, Define this action in IT
{
	__nop();
}

__weak void CE32_STIM_StimAct(CE32_stimulator* handle)//Stimulation Onset action, Define this action in IT
{
	__nop();
}

__weak void CE32_STIM_StimOffAct(CE32_stimulator* handle)
{
	__nop();
}

__weak void CE32_STIM_StimStopAct(CE32_stimulator* handle)//Stimulation Abort action, Define this action in IT
{
	__nop();
}

__weak void CE32_STIM_StimAbortAct(CE32_stimulator* handle)
{
	__nop();
}
__weak void CE32_STIM_SetLevelAct(CE32_stimulator* handle)
{
	__nop();
}
