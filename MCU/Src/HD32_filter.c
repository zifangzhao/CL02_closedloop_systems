#include "HD32_filter.h"

#ifndef _SOS
#include "fdacoefs_BPF_D_1_4@1250_ord2.h"
#include "fdacoefs_BPF_T_4_8@1250_ord2.h"
#include "fdacoefs_BPF_A_8_12@1250_ord2.h"
#include "fdacoefs_BPF_B_12_30@1250_ord2.h"
#include "fdacoefs_BPF_G_30_80@1250_ord2.h"
#include "fdacoefs_BPF_E_80_120@1250_ord2.h"
#include "fdacoefs_BPF_R_110_250@1250_ord2.h"
#include "fdacoefs_BPF_I_60_80Hz@1000_ord2.h"
#endif

#ifdef _SOS
#include "fdacoefs_BPF_D_1_4Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_T_4_8Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_A_8_12Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_B_13_30Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_G_30_80Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_E_80_120Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_R_110_250Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_I_60_80Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_R1_100_200Hz@1000_ord4_SOS.h"
#include "fdacoefs_BPF_SPW_8_40Hz@1000_ord4_SOS.h"

#include "fdacoefs_LPF_D_2Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_T_4Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_A_6Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_B_15Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_G_30Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_E_60Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_R_110Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_I_40Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_R1_100Hz@1000_ord4_SOS.h"
#include "fdacoefs_LPF_SPW_20Hz@1000_ord4_SOS.h"
HD32_filter_coeff filter_custom_coeff[2];
#endif
#include "fdacoefs_dispLPF_500Hz@20K_ord1.h"

void DF_InitFilter_Int16(CE32_Filter_Int16* fil,int16_t* Num,int16_t* Den,int ord){
	fil->Den=Den;
	fil->Num=Num;
	for(int i=0;i<ord;i++){
		fil->state_x[i]=0;
		fil->state_y[i]=0;
	}
	fil->Ord=ord;
	fil->Ptr=0;
}

#ifndef _SOS
void DF_InitFilter(CE32_Filter* fil,float* Num,float* Den,int ord){
	fil->Den=Den;
	fil->Num=Num;
	for(int i=0;i<ord;i++){
		fil->state_x[i]=0;
		fil->state_y[i]=0;
	}
	fil->Ord=ord;
	fil->Ptr=0;
}

void CE32_InitFilter(CE32_Filter* fil,CE32_MA_Filter* MA_fil, CE32_dspParam* dsp){
	switch(dsp->func1){
		case 0:
			DF_InitFilter(fil,(float*)NUM_D,(float*)DEN_D,NL_D);
			break;
		case 1:
			DF_InitFilter(fil,(float*)NUM_T,(float*)DEN_T,NL_T);
			break;
		case 2:
			DF_InitFilter(fil,(float*)NUM_A,(float*)DEN_A,NL_A);
			break;
		case 3:
			DF_InitFilter(fil,(float*)NUM_B,(float*)DEN_B,NL_B);
			break;
		case 4:
			DF_InitFilter(fil,(float*)NUM_G,(float*)DEN_G,NL_G);
			break;
		case 5:
			DF_InitFilter(fil,(float*)NUM_E,(float*)DEN_E,NL_E);
			break;
		case 6:
			DF_InitFilter(fil,(float*)NUM_R,(float*)DEN_R,NL_R);
			break;
		case 7:
			DF_InitFilter(fil,(float*)NUM_I,(float*)DEN_I,NL_I);
			break;
	}
	DF_InitMAFilter(MA_fil,dsp->MAOrd);
}


#endif

#ifdef _SOS
void DF_InitFilter(CE32_Filter* fil,float Num[][3],float Den[][3],int ord, int* Subord){

	for(int i=0;i<ord;i++){
		for(int j=0;j<3;j++){
			fil->state_x[i][j]=0;
			fil->state_y[i][j]=0;
			fil->Ptr[i]=0;
			fil->Den[i][j]=Den[i][j];
			fil->Num[i][j]=Num[i][j];
		}
		
	}
	fil->SubOrd=Subord;
	fil->Ord=ord;
}

float DF_IIR_inputData(void* fil,float input){
	float fil_temp=input;
	float fil_temp_old;
	for(int i=0;i<((CE32_Filter*)fil)->Ord;i++){
		fil_temp_old=fil_temp;
		fil_temp=((CE32_Filter*)fil)->Num[i][0]* fil_temp;
		int temp_ptr = ((CE32_Filter*)fil)->Ptr[i];
		for (int j = 1; j < ((CE32_Filter*)fil)->SubOrd[i]; j++) {
			fil_temp += ((CE32_Filter*)fil)->Num[i][j] * ((CE32_Filter*)fil)->state_x[i][temp_ptr] - ((CE32_Filter*)fil)->Den[i][j] * ((CE32_Filter*)fil)->state_y[i][temp_ptr];
			temp_ptr--;
			if (temp_ptr < 0) {
				temp_ptr += ((CE32_Filter*)fil)->SubOrd[i];
			}
		}
		((CE32_Filter*)fil)->Ptr[i]++;												//increase ptr to store new filter state
		if (((CE32_Filter*)fil)->Ptr[i] >= ((CE32_Filter*)fil)->SubOrd[i]) {
			((CE32_Filter*)fil)->Ptr[i] = 0;
		}	
		((CE32_Filter*)fil)->state_x[i][((CE32_Filter*)fil)->Ptr[i]] = fil_temp_old;
		((CE32_Filter*)fil)->state_y[i][((CE32_Filter*)fil)->Ptr[i]] = fil_temp;
	}
	return fil_temp;
}

void CE32_InitFilter(CE32_Filter* fil,CE32_Filter* MA_fil, CE32_dspParam* dsp){
	switch(dsp->func1){
		case CE32_FILTER_DELTA:
			DF_InitFilter(fil,(float(*)[3])NUM_D,(float(*)[3])DEN_D,MWSPT_NSEC,(int*)NL_D);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_D,(float(*)[3])DEN_LPF_D,MWSPT_NSEC,(int*)NL_LPF_D);
			break;
		case CE32_FILTER_THETA:
			DF_InitFilter(fil,(float(*)[3])NUM_T,(float(*)[3])DEN_T,MWSPT_NSEC,(int*)NL_T);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_T,(float(*)[3])DEN_LPF_T,MWSPT_NSEC,(int*)NL_LPF_T);
			break;
		case CE32_FILTER_ALPHA:
			DF_InitFilter(fil,(float(*)[3])NUM_A,(float(*)[3])DEN_A,MWSPT_NSEC,(int*)NL_A);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_A,(float(*)[3])DEN_LPF_A,MWSPT_NSEC,(int*)NL_LPF_A);
			break;
		case CE32_FILTER_BETA:
			DF_InitFilter(fil,(float(*)[3])NUM_B,(float(*)[3])DEN_B,MWSPT_NSEC,(int*)NL_B);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_B,(float(*)[3])DEN_LPF_B,MWSPT_NSEC,(int*)NL_LPF_B);
			break;
		case CE32_FILTER_GAMMA:
			DF_InitFilter(fil,(float(*)[3])NUM_G,(float(*)[3])DEN_G,MWSPT_NSEC,(int*)NL_G);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_G,(float(*)[3])DEN_LPF_G,MWSPT_NSEC,(int*)NL_LPF_G);
			break;
		case CE32_FILTER_EPSILON:
			DF_InitFilter(fil,(float(*)[3])NUM_E,(float(*)[3])DEN_E,MWSPT_NSEC,(int*)NL_E);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_E,(float(*)[3])DEN_LPF_E,MWSPT_NSEC,(int*)NL_LPF_E);
			break;
		case CE32_FILTER_RIPPLE:
			DF_InitFilter(fil,(float(*)[3])NUM_R,(float(*)[3])DEN_R,MWSPT_NSEC,(int*)NL_R);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_R,(float(*)[3])DEN_LPF_R,MWSPT_NSEC,(int*)NL_LPF_R);
			break;
		case CE32_FILTER_IED:
			DF_InitFilter(fil,(float(*)[3])NUM_I,(float(*)[3])DEN_I,MWSPT_NSEC,(int*)NL_I);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_I,(float(*)[3])DEN_LPF_I,MWSPT_NSEC,(int*)NL_LPF_I);
			break;
		case CE32_FILTER_SPW_RIPPLE:
			DF_InitFilter(fil,(float(*)[3])NUM_R1,(float(*)[3])DEN_R1,MWSPT_NSEC,(int*)NL_R1);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_R1,(float(*)[3])DEN_LPF_R1,MWSPT_NSEC,(int*)NL_LPF_R1);
			break;
		case CE32_FILTER_SPW:
			DF_InitFilter(fil,(float(*)[3])NUM_SPW,(float(*)[3])DEN_SPW,MWSPT_NSEC,(int*)NL_SPW);
			DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_SPW,(float(*)[3])DEN_LPF_SPW,MWSPT_NSEC,(int*)NL_LPF_SPW);
			break;
		case CE32_FILTER_CUSTOM:
			{
				uint8_t fil_id=0;
				if(filter_custom_coeff[fil_id].ord!=0)//Check if initialized
				{
					DF_InitFilter(fil,filter_custom_coeff[fil_id].Num,filter_custom_coeff[fil_id].Den,filter_custom_coeff[fil_id].ord,filter_custom_coeff[fil_id].NL);
				}
				DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_SPW,(float(*)[3])DEN_LPF_SPW,MWSPT_NSEC,(int*)NL_LPF_SPW);
				break;
			}
		case CE32_FILTER_CUSTOM1:
			{
				uint8_t fil_id=1;
				if(filter_custom_coeff[fil_id].ord!=0)//Check if initialized
				{
					DF_InitFilter(fil,filter_custom_coeff[fil_id].Num,filter_custom_coeff[fil_id].Den,filter_custom_coeff[fil_id].ord,filter_custom_coeff[fil_id].NL);
				}
				DF_InitFilter(MA_fil,(float(*)[3])NUM_LPF_SPW,(float(*)[3])DEN_LPF_SPW,MWSPT_NSEC,(int*)NL_LPF_SPW);
				break;
			}
	}
}
#endif

void DF_InitMAFilter(CE32_MA_Filter* fil,int ord){
	for(int i=0;i<ord;i++){
		fil->state[i]=0;
	}
	fil->Ord=ord;
	fil->Ptr=0;
	fil->sum=0;
}
