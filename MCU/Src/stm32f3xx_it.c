/**
  ******************************************************************************
  * @file    stm32f3xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2021 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"

/* USER CODE BEGIN 0 */
#include "dataMGR.h"
#include "HD32_filter.h"
#include "CE32_ClosedLoop.h"
#include "CE32_Stimulator.h"

extern DAC_HandleTypeDef hdac1;

extern int16_t adc_buf[2];
extern int16_t data_buf[BUF_SIZE/2];
extern dataMGR MGR;
extern CE32_systemParam 	sysParam;

extern CE32_dspParam    sysDSP[2];
extern CE32_Filter			 LPF[2];
extern CE32_Filter			 mainFil[2];
extern CE32_MA_Filter   maFil[2];
extern CE32_StimControl sc[2];
extern CE32_CL					 cl;
extern CE32_stimulator STIM_handle[2];
extern uint16_t misc_DigiSig;
extern uint16_t test;
extern float dsp_gain;
\
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_FS;
extern DMA_HandleTypeDef hdma_sdadc1;
extern DMA_HandleTypeDef hdma_sdadc2;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern TIM_HandleTypeDef htim18;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F3xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f3xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles TIM16 global interrupt.
*/
void TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM16_IRQn 0 */
	__HAL_TIM_MOE_DISABLE(STIM_handle[0].htim); //Shutdown PWM immediately to avoid giving extra pulse
	CE32_STIM_IT(&STIM_handle[0]);
  /* USER CODE END TIM16_IRQn 0 */
  //HAL_TIM_IRQHandler(&htim16);
  /* USER CODE BEGIN TIM16_IRQn 1 */

  /* USER CODE END TIM16_IRQn 1 */
}

/**
* @brief This function handles TIM17 global interrupt.
*/
void TIM17_IRQHandler(void)
{
  /* USER CODE BEGIN TIM17_IRQn 0 */
	__HAL_TIM_MOE_DISABLE(STIM_handle[1].htim); //Shutdown PWM immediately to avoid giving extra pulse
	CE32_STIM_IT(&STIM_handle[1]);
  /* USER CODE END TIM17_IRQn 0 */
  //HAL_TIM_IRQHandler(&htim17);
  /* USER CODE BEGIN TIM17_IRQn 1 */

  /* USER CODE END TIM17_IRQn 1 */
}

/**
* @brief This function handles TIM18 global interrupt and DAC2 underrun error interrupt.
*/
void TIM18_DAC2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM18_DAC2_IRQn 0 */
	__HAL_TIM_CLEAR_IT(&htim18, TIM_IT_UPDATE);	
  /* USER CODE END TIM18_DAC2_IRQn 0 */
  //HAL_TIM_IRQHandler(&htim18);
  /* USER CODE BEGIN TIM18_DAC2_IRQn 1 */
	float ch1,ch2;
	switch(sysDSP[0].formula)
	{
		case 0:
			ch1 = adc_buf[0];
			break;
		case 1:
			ch1 = adc_buf[1];
			break;
		case 2:
			ch1 = adc_buf[0]-adc_buf[1];
			break;
	}
	switch(sysDSP[1].formula)
	{
		case 0:
			ch2 = adc_buf[0];
			break;
		case 1:
			ch2 = adc_buf[1];
			break;
		case 2:
			ch2 = adc_buf[0]-adc_buf[1];
			break;
	}

	cl.CL_func((void*)&cl,ch1,ch2);
	
//	misc_DigiSig|=(sc[0].Trig_state&((SC_STATE_TRIG|SC_STATE_STIM|SC_STATE_ON|SC_STATE_TRAINING|SC_STATE_TRAINED)))|\
//								((sc[1].Trig_state&(SC_STATE_TRIG|SC_STATE_STIM)<<2));
	//Enqueue data to preview buffer
	//1. ADC of channel 1
	dataMGR_enQueue_halfword_aligned(&MGR,adc_buf[0]);   //Sampling from ADC buffer(ADC update buffer constantly by DMA)
	
	//2. ADC of channel 2
	dataMGR_enQueue_halfword_aligned(&MGR,adc_buf[1]);
	
	//3. ADC of channel 3
	
	float temp=(float32_t) *(((float*)&cl)+CE32_CL_UNIT_NUM);
	temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
	temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
	dataMGR_enQueue_halfword_aligned(&MGR,(int16_t)temp);
	
	//4. ADC of channel 4
	temp=(float32_t) *(((float*)&cl)+CE32_CL_UNIT_NUM+1);
	temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
	temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
	dataMGR_enQueue_halfword_aligned(&MGR,(int16_t)temp);
	
	//5. Processed data of channel1
	temp=(float32_t) *((float*)&cl);
	temp*=dsp_gain;
	temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
	temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
	dataMGR_enQueue_halfword_aligned(&MGR,(int16_t)temp);
	HAL_DAC_SetValue(&hdac1,DAC_CHANNEL_1,DAC_ALIGN_12B_L,(uint16_t) temp);    //Set DAC value to Calculated Envelop
	//6. Processed data of channel2
	temp=(float32_t) *(((float*)&cl)+1);
	temp*=dsp_gain;
	temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
	temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
	dataMGR_enQueue_halfword_aligned(&MGR,(int16_t)temp);

	//7. Digital state signal
	dataMGR_enQueue_halfword_aligned(&MGR,misc_DigiSig);
	
	//8.test signal
	dataMGR_enQueue_halfword_aligned(&MGR,0xabcd);
	

  /* USER CODE END TIM18_DAC2_IRQn 1 */
}

/**
* @brief This function handles DMA2 channel3 global interrupt.
*/
void DMA2_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel3_IRQn 0 */

  /* USER CODE END DMA2_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdadc1);
  /* USER CODE BEGIN DMA2_Channel3_IRQn 1 */
	
  /* USER CODE END DMA2_Channel3_IRQn 1 */
}

/**
* @brief This function handles DMA2 channel4 global interrupt.
*/
void DMA2_Channel4_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel4_IRQn 0 */

  /* USER CODE END DMA2_Channel4_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_sdadc2);
  /* USER CODE BEGIN DMA2_Channel4_IRQn 1 */

  /* USER CODE END DMA2_Channel4_IRQn 1 */
}

/**
* @brief This function handles USB low priority global interrupt.
*/
void USB_LP_IRQHandler(void)
{
  /* USER CODE BEGIN USB_LP_IRQn 0 */

  /* USER CODE END USB_LP_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
  /* USER CODE BEGIN USB_LP_IRQn 1 */

  /* USER CODE END USB_LP_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
