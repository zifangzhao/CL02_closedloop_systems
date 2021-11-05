#include "CE32_StimControl.h"

void DF_StimControl_init(CE32_StimControl* sc,unsigned int fs, long train_start, long train_period, long trig_duration, long trig_interval, long trig_delay, long trig_rndDelay, float trig_gain)
{
	sc->Train_start_sps=fs*train_start;
	sc->Train_sps=fs*train_period;
	sc->Trig_duration=fs*trig_duration/1000;
	sc->Trig_interval=fs*trig_interval/1000;
	sc->Trig_delay=fs*trig_delay/1000;
	sc->Trig_RndDelay=fs*trig_rndDelay/1000;
	sc->Trig_delay_this=fs*trig_delay/1000;
	sc->Trig_elapsed=0;
	sc->Trig_gain=trig_gain;
	sc->count=0;
	sc->Trig_level=0;
	sc->Trig_state=0;
}

void DF_StimControl_Setting(CE32_StimControl* sc,unsigned int fs, long train_start, long train_period, long trig_duration, long trig_interval, long trig_delay, long trig_rndDelay, float trig_gain)
{
	sc->Train_start_sps=fs*train_start;
	sc->Train_sps=fs*train_period;
	sc->Trig_duration=fs*trig_duration/1000;
	sc->Trig_interval=fs*trig_interval/1000;
	sc->Trig_delay=fs*trig_delay/1000;
	sc->Trig_RndDelay=fs*trig_rndDelay/1000;
	sc->Trig_delay_this=fs*trig_delay/1000;
	sc->Trig_elapsed=0;
	sc->Trig_gain=trig_gain;
}

void DF_StimControl_Start(CE32_StimControl* sc)
{
	sc->Trig_state=SC_STATE_ON;
}

void DF_StimControl_ReTrain(CE32_StimControl* sc)
{
	sc->count=0;
	sc->Trig_state=SC_STATE_ON;
}

void DF_StimControl_Train(CE32_StimControl* sc,float data)
{
	if((sc->count>=sc->Train_start_sps)){	
		if((sc->count<sc->Train_start_sps+sc->Train_sps)){	//start training
			sc->Trig_mean+=data/sc->Train_sps;
			sc->Trig_state|=SC_STATE_TRAINING;
		}
		else{																								//EOF training, start triggering
			sc->Trig_level=sc->Trig_mean*sc->Trig_gain;
			sc->Trig_state&=~SC_STATE_TRAINING;
			sc->Trig_state|=SC_STATE_TRAINED|SC_STATE_TRIGREADY;
			sc->Trig_elapsed=0;
		}
	}

	sc->count++;
}



//Those should be weakly defined
void DF_StimControl_Trig_IT(CE32_StimControl* sc,TIM_TypeDef* tim,uint16_t cnt)
{
	sc->Trig_state|=SC_STATE_TRIG;
	tim->DIER|=TIM_DIER_UIE; //ENable Update interrupt;
	tim->CNT=0;
	tim->ARR=cnt;
	tim->CR1=TIM_CR1_CEN|TIM_CR1_OPM|TIM_CR1_URS; //Configure the timer in One time mode and start counting
}
void DF_StimControl_Stim_IT(CE32_StimControl* sc,TIM_TypeDef* tim,uint16_t cnt,uint16_t pwm,uint16_t repeat)
{
	sc->Trig_state|=SC_STATE_STIM;
	sc->Trig_state&=~SC_STATE_TRIGREADY;
	tim->DIER|=TIM_DIER_UIE|TIM_DIER_CC1IE; //ENable Update interrupt;
	tim->CNT=0;
	tim->ARR=cnt;
	tim->CCR1=pwm;
	sc->STIM_CNT=repeat;
	tim->CR1=TIM_CR1_CEN|TIM_CR1_URS; //Configure the timer in One time mode and start counting
}
void DF_StimControl_TrigEnd(CE32_StimControl* sc)
{
	sc->Trig_state&=~(SC_STATE_STIM);
	sc->Trig_state|=(SC_STATE_TRIGREADY);
}

void DF_StimControl_StimEnd(CE32_StimControl* sc)
{
	sc->Trig_state&=~(SC_STATE_STIM);
}


int DF_StimControl_inputdata(CE32_StimControl* sc, float data){
	if((sc->Trig_state&SC_STATE_ON)!=0){
		if((sc->Trig_state&SC_STATE_TRAINED)==0){	//If not trained
			if((sc->Trig_state&SC_STATE_TRAINING)!=0){	
				if((sc->count>=sc->Train_start_sps)){	
					if((sc->count<sc->Train_start_sps+sc->Train_sps)){	//start training
						sc->Trig_mean+=data/sc->Train_sps;
					}
					else{																								//EOF training, start triggering
						sc->Trig_level=sc->Trig_mean*sc->Trig_gain;
						sc->Trig_state&=~SC_STATE_TRAINING;
						sc->Trig_state|=SC_STATE_TRAINED;
						sc->Trig_elapsed=0;
					}
				}
			}
			else{
				//Waiting for training start
				if((sc->count>=sc->Train_start_sps)){	//start training
					sc->Trig_state|=SC_STATE_TRAINING;
				}
			}
			sc->count++;
		}
		else{							
			if((sc->Trig_state&SC_STATE_TRIG)==0){					//Check if trigger is ready
				if(data > sc->Trig_level){													//Trig if input exceed threshold
					sc->Trig_state|=SC_STATE_TRIG;						//Set Trigger flag, need to be reset by software
				}
			}
			else{
				if(data < sc->Trig_level){													//Trig if input exceed threshold
					sc->Trig_state&=~SC_STATE_TRIG;						//Reset trigger once the signal declined threshold
				}
			}
		}
	}
	return sc->Trig_state;
}
