//#include "stm32f4xx_hal.h"
#include "dataMGR.h"
#include "HD32_filter.h"


#ifndef __CE32_CL
#define __CE32_CL

#define SC_STATE_TRIG 			0x00000001U
#define SC_STATE_STIM			 	0x00000002U
#define SC_STATE_RNDELAY 		0x00000010U
#define SC_STATE_TRIGREADY 	0x00000020U
#define SC_STATE_ON 				0x00000800U
#define SC_STATE_TRAINING 	0x00001000U
#define SC_STATE_TRAINED  	0x00002000U
#define SC_STATE_WAITING  	0x00004000U
#define SC_STATE_EXTCTRL		0x00008000U
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

typedef struct{
	uint32_t min_delay; //in ms
	uint32_t max_delay; //in ms
	uint32_t delay_cnt;
	uint8_t state;
}CE32_CL_RndTrig;

void DF_StimControl_init(CE32_StimControl* sc,unsigned int fs, long train_start, long train_period, long trig_duration, long trig_interval, long trig_delay, long trig_rndDelay, float trig_gain);
void DF_StimControl_Setting(CE32_StimControl* sc,unsigned int fs, long train_start, long train_period, long trig_duration, long trig_interval, long trig_delay, long trig_rndDelay, float trig_gain);
void DF_StimControl_Start(CE32_StimControl* sc);
int DF_StimControl_inputdata(CE32_StimControl* sc, float data,int id);

typedef struct
{
	float DSP_output[CE32_CL_UNIT_NUM]; //Keep this fixed at beginning for (float*) pointer access
	float DSP_temp[CE32_CL_UNIT_NUM]; //Middle state
	uint16_t mode;
	uint16_t trig_state[CE32_CL_UNIT_NUM];
	CE32_systemParam* sysParam;
	void (*CL_func)(void* cl,float input1,float input2);
	CE32_Filter *main_fil[CE32_CL_UNIT_NUM];
	CE32_MA_Filter *MA_fil[CE32_CL_UNIT_NUM];
	CE32_StimControl *sc[CE32_CL_UNIT_NUM];
	CE32_dspParam*    sysDSP[CE32_CL_UNIT_NUM];
	CE32_CL_RndTrig rndTrig;
}CE32_CL;

void CE32_CL_Init(CE32_CL* cl, CE32_systemParam* sysParam, CE32_dspParam* sysDSP, CE32_Filter* main_Fil, CE32_MA_Filter* MA_fil, CE32_StimControl* sc); //Should be calling after the filter and stim control initialized
void CE32_CL_Start(CE32_CL* cl);
void CE32_CL_Reset(CE32_CL* cl,int id);

void CE32_CL_Set_TrigLvl(void* cl,int id,float lvl);
void CE32_CL_Set_RndTrig(CE32_CL* cl,uint32_t min_delay,uint32_t max_delay);
void CE32_CL_TrigStart(void* vcl,int id);
void CE32_CL_TrigStop(void* vcl,int id);

void CE32_CL_WaitAct(int id);
void CE32_CL_TrainAct(int id);
void CE32_CL_ReadyAct(int id);

void CE32_CL_TrigAct(int id);
void CE32_CL_TrigStopAct(int id);

#endif
