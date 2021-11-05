#include "CL02_Server.h"


int CL02_CmdSvr(uint8_t *data_ptr,uint32_t cmd_len,dataMGR *MGR,CE32_systemParam *sysParam,CE32_dspParam *sysDSP,CE32_Filter *mainFil,CE32_MA_Filter *maFil,CE32_StimControl *sc,CE32_CL* cl)
{
	/*CMD ID: 
		
		0x01.	update SYS 
		0x02.	update DSP1

		0x10. Threshold setting(std.)
		0x11. state control (stimulation/closed loop)
		0x14. Threshold setting(abs.)
	
		0x20. stimulation timing control
		0x21. DSP setting
	
		
		0x40. Start Preview
		0x41. Stop Preview

		*/
	switch(data_ptr[0])
	{
		case 0x01:
			memcpy(((uint8_t*)&sysParam),&data_ptr[1],512);
			if((sysParam->Flag&CE32_SYSPARAM_FLAG_NORESET)==0)
			{
				CE32_CL_Init(cl,sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
				CE32_CL_Start(cl);
			}
			else
			{
				DF_StimControl_Setting(&sc[0],sysParam->fs/RES_RATE,sysParam->trigger_trainStart,sysParam->trigger_trainDuration,sysParam->pulse_width[0],sysParam->stim_interval[0],sysParam->stim_delay[0],sysParam->stim_RndDelay[0],sysParam->trigger_gain[0]);
				DF_StimControl_Setting(&sc[1],sysParam->fs/RES_RATE,sysParam->trigger_trainStart,sysParam->trigger_trainDuration,sysParam->pulse_width[1],sysParam->stim_interval[1],sysParam->stim_delay[1],sysParam->stim_RndDelay[1],sysParam->trigger_gain[1]);
			}
			break;
		case 0x02:
			memcpy(&sysDSP[0],&data_ptr[1],512);
			//CE32_arbitar_Init(&arbitarCal_CH1,&sysDSP[0]);			//Init pointer for signal arbitar
			CE32_CL_Init(cl,sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
			CE32_CL_Start(cl);
			break;
		case 0x10://Gain setting realtime
		{
			float v;
			uint8_t *ptr;
			ptr=(uint8_t*)&v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[i+1];
			}
			sc->Trig_gain=v;
			sc->Trig_level=sc->Trig_gain*sc->Trig_mean;
			break;
		}
		case 0x11://STIM on/off
		{
			if(data_ptr[1]==0){
				//sc->Trig_state=~SC_STATE_ON;
				sysParam->stim_mode=0;
			}
			else{
				//sc->Trig_state|=SC_STATE_ON;
				sysParam->stim_mode=1;
			}
				break;
		}
		case 0x14:
		{
			float v;
			uint8_t *ptr;
			ptr=(uint8_t*)&v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[i+1];
			}
			sc->Trig_level=v;
			break;
		}
		case 0x17:
		{
			long v;
			uint8_t * ptr;
			uint8_t cnt=1;
			ptr=(uint8_t*)v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sc->Trig_delay=v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sc->Trig_RndDelay=v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sc->Trig_duration=v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sc->Trig_interval=v;
			break;
		}
		case 0x21:
		{
			ulong v;
			uint8_t * ptr;
			uint8_t cnt=1;
			ptr=(uint8_t*)v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sysDSP->MAOrd=v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sysDSP->func1=v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[cnt];
				cnt++;
			}
			sysDSP->formula=v;
			
			CE32_InitFilter(mainFil,maFil,sysDSP);
			break;
		}
		case 0x40:
		{
			MGR->sysParam|=MGR_SYS_PREVIEW;
			break;
		}
		case 0x41:
		{
			MGR->sysParam=~MGR_SYS_PREVIEW;
			break;
		}
	}
	return 0;
}
