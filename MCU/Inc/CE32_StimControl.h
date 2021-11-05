#define ARM_MATH_CM4
#include "stm32f3xx_hal.h"
#include "arm_math.h"
#include "dataMGR.h"
#include <stdlib.h>

#ifndef __CE32_STIMCTRL
#define __CE32_STIMCTRL


#define SC_STATE_TRIG 			0x00000001U
#define SC_STATE_STIM			 	0x00000002U
#define SC_STATE_RNDELAY 		0x00000010U
#define SC_STATE_TRIGREADY 	0x00000020U
#define SC_STATE_ON 				0x00000800U
#define SC_STATE_TRAINING 	0x00001000U
#define SC_STATE_TRAINED  	0x00002000U


typedef struct{
	long Trig_delay_this;
	long Trig_delay;
	long Trig_RndDelay;
	long Trig_duration;
	long Trig_interval;
	float Trig_level;
	float Trig_gain;
	float Trig_mean;
	long Train_sps;
	long Train_start_sps;
	long Trig_state;
	long Trig_elapsed;
	long STIM_CNT;
	long count;
}CE32_StimControl;

void DF_StimControl_init(CE32_StimControl* sc,unsigned int fs, long train_start, long train_period, long trig_duration, long trig_interval, long trig_delay, long trig_rndDelay, float trig_gain);
void DF_StimControl_Setting(CE32_StimControl* sc,unsigned int fs, long train_start, long train_period, long trig_duration, long trig_interval, long trig_delay, long trig_rndDelay, float trig_gain);
void DF_StimControl_Start(CE32_StimControl* sc);
void DF_StimControl_Train(CE32_StimControl* sc,float data);
void DF_StimControl_Trig_IT(CE32_StimControl* sc,TIM_TypeDef* tim,uint16_t cnt);
void DF_StimControl_Stim_IT(CE32_StimControl* sc,TIM_TypeDef* tim,uint16_t cnt,uint16_t pwm,uint16_t repeat);
void DF_StimControl_TrigEnd(CE32_StimControl* sc);
void DF_StimControl_StimEnd(CE32_StimControl* sc);
void DF_StimControl_ReTrain(CE32_StimControl* sc);

int DF_StimControl_inputdata(CE32_StimControl* sc, float data);

__forceinline int DF_StimControl_inputdata_gated(CE32_StimControl* sc, float data,uint32_t gateSig){
	int resp=0;
	if((sc->Trig_state&SC_STATE_ON)!=0){
		if((sc->Trig_state&SC_STATE_TRAINING)!=0){	//wait for training start
			resp=1;
			if((sc->count>=sc->Train_start_sps)){	
				if((sc->count<sc->Train_start_sps+sc->Train_sps)){	//start training
					sc->Trig_mean+=data/sc->Train_sps;
					resp=2;
				}
				else{																								//EOF training, start triggering
					sc->Trig_level=sc->Trig_mean*sc->Trig_gain;
					sc->Trig_state&=~SC_STATE_TRAINING;
					sc->Trig_state|=SC_STATE_TRIG|SC_STATE_TRAINED;
					sc->Trig_elapsed=0;
				}
			}
			sc->count++;
		}
		else{
			if((sc->Trig_state&SC_STATE_TRIG)!=0){								
				if((sc->Trig_state&SC_STATE_TRIGREADY)!=0){					//Check if trigger is ready
					if((data > sc->Trig_level)&&(gateSig!=0)){													//Trig if input exceed threshold
						sc->Trig_state&=~SC_STATE_TRIGREADY;						
						sc->Trig_elapsed=0;
						sc->Trig_delay_this=sc->Trig_delay+sc->Trig_RndDelay/100.0*rand();
					}
				}
				if((sc->Trig_state&SC_STATE_TRIGREADY)==0){					//Check if triggered
					if((sc->Trig_elapsed >= sc->Trig_delay_this)&&(sc->Trig_elapsed < sc->Trig_duration + sc->Trig_delay_this)){
						resp=3;
					}
					else{
						if(sc->Trig_elapsed>=sc->Trig_interval){
							sc->Trig_state|=SC_STATE_TRIGREADY;
						}
						resp=4;
					}
					sc->Trig_elapsed++;
				}
			}
		}
	}
	return resp;
}

#endif
