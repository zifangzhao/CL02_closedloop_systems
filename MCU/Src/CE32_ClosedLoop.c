#include "CE32_ClosedLoop.h"

#define CE32_CL_FUNC_DISABLED 0
#define CE32_CL_FUNC_SINGLE 1
#define CE32_CL_FUNC_DOUBLE 2
#define CE32_CL_FUNC_CASCADE 3
#define CE32_CL_FUNC_GATED 4
#define CE32_CL_FUNC_RND 5

void CE32_CL_Disabled(void* cl, float input1,float input2);
void CE32_CL_Single(void* cl, float input1,float input2);
void CE32_CL_Double(void* cl, float input1,float input2);
void CE32_CL_Cascade(void* cl, float input1,float input2);
void CE32_CL_Gated(void* cl, float input1,float input2);
void CE32_CL_Random(void* cl, float input1,float input2);

void CE32_CL_Random_Update(void* cl);


void CE32_CL_Init(CE32_CL* cl, CE32_systemParam* sysParam,CE32_dspParam* sysDSP, CE32_Filter* main_Fil, CE32_MA_Filter* MA_fil, CE32_StimControl* sc)
{
	for(int i=0;i<CE32_CL_UNIT_NUM;i++)
	{
		cl->main_fil[i]=&main_Fil[i];
		cl->MA_fil[i]=&MA_fil[i];
		cl->sc[i]=&sc[i];
		CE32_InitFilter(&main_Fil[i],&MA_fil[i],&sysDSP[i]);
		DF_StimControl_init(&sc[i],sysParam->fs,
			sysParam->trigger_trainStart,sysParam->trigger_trainDuration,
			sysParam->pulse_width[i],sysParam->stim_interval[i],
			sysParam->stim_delay[i],sysParam->stim_RndDelay[i],
			sysParam->trigger_gain[i]);
	}
	CE32_CL_Set_RndTrig(cl,sysParam->randtrig_min,sysParam->randtrig_max);
	cl->sysParam=sysParam;
	cl->mode=cl->sysParam->cl_mode;
	switch(cl->mode)
	{
		case CE32_CL_FUNC_DISABLED:
			cl->CL_func=&CE32_CL_Disabled;
			break;
		case CE32_CL_FUNC_SINGLE:
			cl->CL_func=&CE32_CL_Single;
			break;
		case CE32_CL_FUNC_DOUBLE:
			cl->CL_func=&CE32_CL_Double;
			break;
		case CE32_CL_FUNC_CASCADE:
			cl->CL_func=&CE32_CL_Cascade;
			break;
		case CE32_CL_FUNC_GATED:
			cl->CL_func=&CE32_CL_Gated;
			break;
		case CE32_CL_FUNC_RND:
			cl->CL_func=&CE32_CL_Random;
			break;
	}	
	srand(RTC->SSR);	//Randomize seed
}

void CE32_CL_Start(CE32_CL* cl)
{
	switch(cl->mode)
	{
		case CE32_CL_FUNC_DISABLED:
			break;
		case CE32_CL_FUNC_SINGLE:
			DF_StimControl_Start(cl->sc[0]);
			break;
		case CE32_CL_FUNC_DOUBLE:
			DF_StimControl_Start(cl->sc[0]);
			DF_StimControl_Start(cl->sc[1]);
			break;
		case CE32_CL_FUNC_CASCADE:
			DF_StimControl_Start(cl->sc[0]);
			DF_StimControl_Start(cl->sc[1]);
			break;
		case CE32_CL_FUNC_GATED:
			DF_StimControl_Start(cl->sc[0]);
			DF_StimControl_Start(cl->sc[1]);
			break;
		case CE32_CL_FUNC_RND:
			DF_StimControl_Start(cl->sc[0]);
			break;
	}
}

void CE32_CL_Reset(CE32_CL* cl,int id);

void CE32_CL_Disabled(void* vcl, float input1,float input2)
{
	//Do nothing
}

void CE32_CL_Single(void* vcl, float input1,float input2)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	float* DSP_output=(float*)vcl;
	float DSP_temp1=DF_IIR_inputData(cl->main_fil[0],input1);	//Process main filter
	DSP_output[0]=DF_MA_inputData(cl->MA_fil[0],fabsf(DSP_temp1));	// Process main moving average filter
	cl->DSP_temp[0]=DSP_temp1;
	int rst=DF_StimControl_inputdata(cl->sc[0],DSP_output[0],0);
	
	if((cl->sc[0]->Trig_state&SC_STATE_TRIG)!=0)
	{
		CE32_CL_TrigAct(0x01);
	}
}
void CE32_CL_Double(void* vcl, float input1,float input2)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	float* DSP_output=(float*)vcl;
	float DSP_temp1=DF_IIR_inputData(cl->main_fil[0],input1);	//Process main filter
	DSP_output[0]=DF_MA_inputData(cl->MA_fil[0],fabsf(DSP_temp1));	// Process main moving average filter
	float DSP_temp2=DF_IIR_inputData(cl->main_fil[1],input2);	//Process main filter
	DSP_output[1]=DF_MA_inputData(cl->MA_fil[1],fabsf(DSP_temp2));	// Process main moving average filter
	cl->DSP_temp[0]=DSP_temp1;
	cl->DSP_temp[1]=DSP_temp2;
	DF_StimControl_inputdata(cl->sc[0],DSP_output[0],0);
	DF_StimControl_inputdata(cl->sc[1],DSP_output[1],1);
	
	if((cl->sc[0]->Trig_state&SC_STATE_TRIG)!=0)
	{
		CE32_CL_TrigAct(0x01);
	}
	if((cl->sc[1]->Trig_state&SC_STATE_TRIG)!=0)
	{
		CE32_CL_TrigAct(0x02);
	}
}
void CE32_CL_Cascade(void* vcl, float input1,float input2)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	float* DSP_output=(float*)vcl;
	float DSP_temp1=DF_IIR_inputData(cl->main_fil[0],input1);	//Process main filter
	DSP_output[0]=DSP_temp1;
	float DSP_temp2=DF_IIR_inputData(cl->main_fil[1],DSP_temp1);	//Process main filter
	DSP_output[1]=DF_MA_inputData(cl->MA_fil[1],fabsf(DSP_temp2));	// Process main moving average filter
	cl->DSP_temp[0]=DSP_temp1;
	cl->DSP_temp[1]=DSP_temp2;
	int rst=DF_StimControl_inputdata(cl->sc[1],DSP_output[1],1);
	if((cl->sc[1]->Trig_state&SC_STATE_TRIG)!=0)
	{
		CE32_CL_TrigAct(0x01);
	}
}
void CE32_CL_Gated(void* vcl, float input1,float input2)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	float* DSP_output=(float*)vcl;
	float DSP_temp1=DF_IIR_inputData(cl->main_fil[0],input1);	//Process main filter
	DSP_output[0]=DF_MA_inputData(cl->MA_fil[0],fabsf(DSP_temp1));	// Process main moving average filter
	float DSP_temp2=DF_IIR_inputData(cl->main_fil[1],input2);	//Process main filter
	DSP_output[1]=DF_MA_inputData(cl->MA_fil[1],fabsf(DSP_temp2));	// Process main moving average filter
	cl->DSP_temp[0]=DSP_temp1;
	cl->DSP_temp[1]=DSP_temp2;
	DF_StimControl_inputdata(cl->sc[0],DSP_output[0],0);
	DF_StimControl_inputdata(cl->sc[1],DSP_output[1],1);
	
	if(((cl->sc[0]->Trig_state&SC_STATE_TRIG)!=0)&&(cl->sc[1]->Trig_state&SC_STATE_TRIG)!=0)
	{
		CE32_CL_TrigAct(0x01);
	}
}

void CE32_CL_Random(void* vcl, float input1,float input2)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	CE32_CL_RndTrig* rndtrig=&cl->rndTrig;
	if(rndtrig->delay_cnt--==0)
	{
		CE32_CL_Random_Update(vcl); //Update next trig time
		CE32_CL_TrigAct(0x01);
	}
}


void CE32_CL_Random_Update(void* vcl)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	uint16_t fs=cl->sysParam->fs;
	CE32_CL_RndTrig* rndtrig=&cl->rndTrig;
	uint64_t temp;
		temp= (rndtrig->max_delay-rndtrig->min_delay)*((float)(rand())/RAND_MAX);
		temp+= rndtrig->min_delay; //this unit is in ms
		rndtrig->delay_cnt = temp*(fs/1000.0); //Set next rand trig time
}

void CE32_CL_Set_TrigLvl(void* vcl,int id,float lvl)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	cl->sc[id]->Trig_level=lvl;
}

void CE32_CL_Set_RndTrig(CE32_CL* cl,uint32_t min_delay,uint32_t max_delay)
{
	cl->rndTrig.max_delay=max_delay;
	cl->rndTrig.min_delay=min_delay;
	CE32_CL_Random_Update((void*) cl);
}

void CE32_CL_TrigStart(void* vcl,int id)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	cl->trig_state[id]|=SC_STATE_TRIG;
	CE32_CL_TrigAct(id);
}

void CE32_CL_TrigStop(void* vcl,int id)
{
	CE32_CL* cl=(CE32_CL*) vcl;
	cl->trig_state[id]&=~SC_STATE_TRIG;
	CE32_CL_TrigStopAct(id);
}

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

int DF_StimControl_inputdata(CE32_StimControl* sc, float data,int id){
	if(((sc->Trig_state&SC_STATE_ON)!=0)&&((sc->Trig_state&SC_STATE_EXTCTRL)==0)){
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
						CE32_CL_ReadyAct(id);
					}
				}
			}
			else{
				//Waiting for training start
				if((sc->count>=sc->Train_start_sps)){	//start training
					sc->Trig_mean=0;//Recalculate mean value
					sc->Trig_state|=SC_STATE_TRAINING;
					sc->Trig_state&=~SC_STATE_WAITING;
					CE32_CL_TrainAct(id);
				}
				else{
					if((sc->Trig_state&SC_STATE_WAITING)==0)
					{
						sc->Trig_state|=SC_STATE_WAITING;
						CE32_CL_WaitAct(id);
					}
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

__weak void CE32_CL_WaitAct(int id)
{
	__nop();
}

__weak void CE32_CL_TrainAct(int id)
{
	__nop();
}

__weak void CE32_CL_ReadyAct(int id)
{
	__nop();
}


__weak void CE32_CL_TrigAct(int id)
{
	__nop();
}

__weak void CE32_CL_TrigStopAct(int id)
{
	__nop();
}

