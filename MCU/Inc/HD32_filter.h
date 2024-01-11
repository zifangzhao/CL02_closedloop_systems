#define ARM_MATH_CM4
#include "stm32f3xx_hal.h"
#include "arm_math.h"
#include "dataMGR.h"
#include <stdlib.h>

// define _SOS to enable second order filtering format
#define _SOS

#ifndef __CE32_FILTER
#define __CE32_FILTER
#define MAX_FIL_ORD 16
#define MAX_MA_ORD 512

#define DSP1_TRIG_TIM TIM6
#define DSP2_TRIG_TIM TIM7
//Use advance timer for stim
#define DSP1_STIM_TIM TIM1
#define DSP2_STIM_TIM TIM8


#define CE32_FILTER_DELTA 0
#define CE32_FILTER_THETA 1
#define CE32_FILTER_ALPHA 2
#define CE32_FILTER_BETA 3
#define CE32_FILTER_GAMMA 4
#define CE32_FILTER_EPSILON 5
#define CE32_FILTER_RIPPLE 6
#define CE32_FILTER_IED 7
#define CE32_FILTER_SPW_RIPPLE 8
#define CE32_FILTER_SPW 9
#define CE32_FILTER_CUSTOM 10
#define CE32_FILTER_CUSTOM1 11
typedef struct{
	int16_t state_x[MAX_FIL_ORD];
	int16_t state_y[MAX_FIL_ORD];
	int16_t* Num;
	int16_t* Den;
	long    Ptr;				//pointer for state buffer
	long    Ord;				//filter order
	long 		PtrMask;
}CE32_Filter_Int16;

#ifndef _SOS
typedef struct{
	float state_x[MAX_FIL_ORD];
	float state_y[MAX_FIL_ORD];
	float* Num;
	float* Den;
	long    Ptr;				//pointer for state buffer
	long    Ord;				//filter order
	long 		PtrMask;
}CE32_Filter;
typedef struct{
	uint16_t ord;
	float Num[MAX_FIL_ORD];
	float Den[MAX_FIL_ORD];
}HD32_filter_coeff;
#else
typedef struct{
	float state_x[MAX_FIL_ORD][3];
	float state_y[MAX_FIL_ORD][3];
	float Num[MAX_FIL_ORD][3];
	float Den[MAX_FIL_ORD][3];
	int    	Ptr[MAX_FIL_ORD];				//pointer for state buffer
	int*    SubOrd;				//filter order
	int 		Ord;
}CE32_Filter;

typedef struct{
	uint16_t ord;
	int NL[MAX_FIL_ORD];
	float Num[MAX_FIL_ORD][3];
	float Den[MAX_FIL_ORD][3];
}HD32_filter_coeff;
#endif

typedef struct{
	float 	state[MAX_MA_ORD];
	long    Ptr;				//pointer for state buffer
	long    Ord;				//filter order
	long 		PtrMask;
	float 	sum;
}CE32_MA_Filter;

//float DF_IIR_inputData(void* fil,float input);
//float DF_FIR_inputData(void* fil,float input);
void CE32_InitFilter(CE32_Filter* fil,CE32_MA_Filter* MA_fil, CE32_dspParam* dsp);
void DF_InitMAFilter(CE32_MA_Filter* fil,int ord);

#ifndef _SOS
void DF_InitFilter(CE32_Filter* fil,float* Num,float* Den,int ord);
void DF_InitFilter_Int16(CE32_Filter_Int16* fil,int16_t* Num,int16_t* Den,int ord);

__forceinline float DF_IIR_inputData(void* fil,float input){
	float fil_temp=((CE32_Filter*)fil)->Num[0]* input;
	int temp_ptr = ((CE32_Filter*)fil)->Ptr;
	for (int i = 1; i < ((CE32_Filter*)fil)->Ord; i++) {
		fil_temp += ((CE32_Filter*)fil)->Num[i] * ((CE32_Filter*)fil)->state_x[temp_ptr] - ((CE32_Filter*)fil)->Den[i] * ((CE32_Filter*)fil)->state_y[temp_ptr];
		temp_ptr--;
		if (temp_ptr < 0) {
			temp_ptr += ((CE32_Filter*)fil)->Ord;
		}
	}
	((CE32_Filter*)fil)->Ptr++;												//increase ptr to store new filter state
	if (((CE32_Filter*)fil)->Ptr >= ((CE32_Filter*)fil)->Ord) {
		((CE32_Filter*)fil)->Ptr = 0;
	}
	((CE32_Filter*)fil)->state_x[((CE32_Filter*)fil)->Ptr] = input;
	((CE32_Filter*)fil)->state_y[((CE32_Filter*)fil)->Ptr] = fil_temp;
	
	return fil_temp;
}

__forceinline int16_t DF_IIR_inputData_Int16(void* fil,int16_t input){
	int16_t fil_temp=((CE32_Filter_Int16*)fil)->Num[0]* input;
	int temp_ptr = ((CE32_Filter_Int16*)fil)->Ptr;
	for (int i = 1; i < ((CE32_Filter_Int16*)fil)->Ord; i++) {
		fil_temp += ((CE32_Filter_Int16*)fil)->Num[i] * ((CE32_Filter_Int16*)fil)->state_x[temp_ptr] - ((CE32_Filter_Int16*)fil)->Den[i] * ((CE32_Filter_Int16*)fil)->state_y[temp_ptr];
		temp_ptr--;
		if (temp_ptr < 0) {
			temp_ptr += ((CE32_Filter_Int16*)fil)->Ord;
		}
	}
	((CE32_Filter_Int16*)fil)->Ptr++;												//increase ptr to store new filter state
	if (((CE32_Filter_Int16*)fil)->Ptr >= ((CE32_Filter_Int16*)fil)->Ord) {
		((CE32_Filter_Int16*)fil)->Ptr = 0;
	}
	((CE32_Filter_Int16*)fil)->state_x[((CE32_Filter_Int16*)fil)->Ptr] = input;
	((CE32_Filter_Int16*)fil)->state_y[((CE32_Filter_Int16*)fil)->Ptr] = fil_temp;
	
	return fil_temp;
}

__forceinline float DF_FIR_inputData(void* fil,float input){
	float fil_temp=((CE32_Filter*)fil)->Num[0]* input;
	int temp_ptr = ((CE32_Filter*)fil)->Ptr;

	for (int i = 1; i < ((CE32_Filter*)fil)->Ord; i++) {
		fil_temp += ((CE32_Filter*)fil)->Num[i] * ((CE32_Filter*)fil)->state_x[temp_ptr];
		temp_ptr--;
		if (temp_ptr < 0) {
			temp_ptr += ((CE32_Filter*)fil)->Ord;
		}
	}
	((CE32_Filter*)fil)->Ptr++;												//increase ptr to store new filter state
	if (((CE32_Filter*)fil)->Ptr >= ((CE32_Filter*)fil)->Ord) {
		((CE32_Filter*)fil)->Ptr = 0;
	}
	((CE32_Filter*)fil)->state_x[((CE32_Filter*)fil)->Ptr] = input;
	
	return fil_temp;
}

__forceinline float DF_IIR_aligned_inputData(void* fil,float input){
	float fil_temp=((CE32_Filter*)fil)->Num[0]* input;
	int temp_ptr = ((CE32_Filter*)fil)->Ptr;
	for (int i = 1; i < ((CE32_Filter*)fil)->Ord; i++) {
		fil_temp += ((CE32_Filter*)fil)->Num[i] * ((CE32_Filter*)fil)->state_x[temp_ptr] - ((CE32_Filter*)fil)->Den[i] * ((CE32_Filter*)fil)->state_y[temp_ptr];
		temp_ptr--;
		temp_ptr&= ((CE32_Filter*)fil)->PtrMask;
	}
	((CE32_Filter*)fil)->Ptr++;												//increase ptr to store new filter state
	((CE32_Filter*)fil)->Ptr&= ((CE32_Filter*)fil)->PtrMask;
	((CE32_Filter*)fil)->state_x[((CE32_Filter*)fil)->Ptr] = input;
	((CE32_Filter*)fil)->state_y[((CE32_Filter*)fil)->Ptr] = fil_temp;
	
	return fil_temp;
}

__forceinline float DF_FIR_aligned_inputData(void* fil,float input){
	float fil_temp=((CE32_Filter*)fil)->Num[0]* input;
	int temp_ptr = ((CE32_Filter*)fil)->Ptr;
	for (int i = 1; i < ((CE32_Filter*)fil)->Ord; i++) {
		fil_temp += ((CE32_Filter*)fil)->Num[i] * ((CE32_Filter*)fil)->state_x[temp_ptr];
		temp_ptr--;
		temp_ptr&= ((CE32_Filter*)fil)->PtrMask;
	}
	((CE32_Filter*)fil)->Ptr++;												//increase ptr to store new filter state
	((CE32_Filter*)fil)->Ptr&= ((CE32_Filter*)fil)->PtrMask;
	((CE32_Filter*)fil)->state_x[((CE32_Filter*)fil)->Ptr] = input;
	return fil_temp;
}


__forceinline float DF_Pre_LFP(void* fil,float input){
	float fil_temp=input*(float)0.07295965403;
	fil_temp+=(float)0.07295965403*((CE32_Filter*)fil)->state_x[0] + (float)0.854080677* ((CE32_Filter*)fil)->state_y[0];
	((CE32_Filter*)fil)->state_x[0]=input;
	((CE32_Filter*)fil)->state_y[0]=fil_temp;
	return fil_temp;
}
#endif

#ifdef _SOS

void DF_InitFilter(CE32_Filter* fil,float Num[][3],float Den[][3],int Ord,int* SubOrd);

float DF_IIR_inputData(void* fil,float input);
#endif

__forceinline float DF_MA_aligned_inputData(CE32_MA_Filter* fil,float input){
	fil->sum-=fil->state[fil->Ptr];
	fil->sum+=input;
	fil->Ptr++;
	fil->Ptr&=fil->PtrMask;
	fil->state[fil->Ptr]=input;
	return fil->sum/fil->Ord;
}

__forceinline float DF_MA_inputData(CE32_MA_Filter* fil,float input){
	float temp=input/fil->Ord;
	fil->sum-=fil->state[fil->Ptr];
	fil->state[fil->Ptr]=temp;
	fil->sum+=temp;
	fil->Ptr++;
	if(fil->Ptr>=fil->Ord){
		fil->Ptr=0;
	}
	return fil->sum;
}

#endif
