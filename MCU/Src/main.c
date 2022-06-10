/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2021 STMicroelectronics International N.V.
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f3xx_hal.h"
#include "usb_device.h"

/* USER CODE BEGIN Includes */
#include "dataMGR.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"
#include "HD32_filter.h"
#include "CE32_USB_INTERCOM.h"
#include "CE32_Stimulator.h"
#include "CE32_ClosedLoop.h"
#define kFS 1000
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DAC_HandleTypeDef hdac1;

I2C_HandleTypeDef hi2c1;

SDADC_HandleTypeDef hsdadc1;
SDADC_HandleTypeDef hsdadc2;
DMA_HandleTypeDef hdma_sdadc1;
DMA_HandleTypeDef hdma_sdadc2;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;
TIM_HandleTypeDef htim18;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int16_t adc_buf[2];
int16_t data_buf[BUF_SIZE/2];
dataMGR MGR;
CE32_systemParam 	sysParam;

CE32_dspParam    sysDSP[2];
CE32_Filter			 mainFil[2];
CE32_MA_Filter   maFil[2];
CE32_StimControl sc[2];
CE32_CL					 cl;
const int trans_size=256;
uint16_t misc_DigiSig;
uint16_t test;
float dsp_gain=10;
extern HD32_filter_coeff filter_custom_coeff;

uint8_t TX_buf[6]; //TX buffer, with extra bytes as reserve
uint8_t TX_buf_state=0;

void (*arbitarCal_CH1)(uchar* ch_ord,short* data,float* output);
void (*arbitarCal_CH2)(uchar* ch_ord,short* data,float* output);

CE32_USB_INTERCOM_Handle IC_USB_handle;

CE32_stimulator STIM_handle[2];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DAC1_Init(void);
static void MX_SDADC1_Init(void);
static void MX_SDADC2_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM18_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
int CL02_CmdSvr(uint8_t *data_ptr,uint32_t cmd_len);
int CL02_sendDataPackage();
int CL02_purgeData();
int CL02_setClParam();
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)

{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	//Validate_ChipId();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_DAC1_Init();
  MX_SDADC1_Init();
  MX_SDADC2_Init();
  MX_USB_DEVICE_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_SPI1_Init();
  MX_TIM2_Init();
  MX_TIM18_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	
	dataMGR_init(&MGR,(char*) data_buf,sizeof(data_buf));					//FIFO setup 
	sysParam.fs = kFS;
	CE32_CL_Init(&cl,&sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
	CE32_CL_Start(&cl);
	
	HAL_SDADC_CalibrationStart(&hsdadc1,SDADC_CALIBRATION_SEQ_3);
	HAL_SDADC_CalibrationStart(&hsdadc2,SDADC_CALIBRATION_SEQ_3);
	HAL_SDADC_ConfigChannel(&hsdadc1,SDADC_CHANNEL_6,SDADC_CONTINUOUS_CONV_ON);
	HAL_SDADC_ConfigChannel(&hsdadc2,SDADC_CHANNEL_6,SDADC_CONTINUOUS_CONV_ON);
	//HAL_SDADC_Start_DMA(&hsdadc1,(uint32_t*)adc_buf,1);
	
	hsdadc1.Instance->CR1 |= SDADC_CR1_RDMAEN;
	HAL_DMA_Start_IT(hsdadc1.hdma, (uint32_t)&hsdadc1.Instance->RDATAR, (uint32_t) adc_buf, 1);
	hsdadc1.Instance->CR2 |= SDADC_CR2_RSWSTART;
	hsdadc2.Instance->CR1 |= SDADC_CR1_RDMAEN;
	HAL_DMA_Start_IT(hsdadc2.hdma, (uint32_t)&hsdadc2.Instance->RDATAR, (uint32_t) (adc_buf+1), 1);
	hsdadc2.Instance->CR2 |= SDADC_CR2_RSWSTART;
	
	//Stimulator initializing
	CE32_STIM_Init(&STIM_handle[0],&htim16,0,0);
	CE32_STIM_Init(&STIM_handle[1],&htim17,0,1);
	CE32_STIM_Setup(&STIM_handle[0],sysParam.stim_intensity[0],sysParam.stim_delay[0]+rand()*sysParam.stim_RndDelay[0],sysParam.stim_interval[0],sysParam.pulse_cnt[0],sysParam.pulse_width[0]);
	CE32_STIM_Setup(&STIM_handle[1],sysParam.stim_intensity[1],sysParam.stim_delay[1]+rand()*sysParam.stim_RndDelay[1],sysParam.stim_interval[1],sysParam.pulse_cnt[1],sysParam.pulse_width[1]);
	
	HAL_DAC_Start(&hdac1,DAC_CHANNEL_1);
	//start ticking timer
	HAL_TIM_Base_Start_IT(&htim18);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
		
		if(MGR.bufferUsed[0]>trans_size){
			if(MGR.sysParam&MGR_SYS_PREVIEW){
				CL02_sendDataPackage();
			}
			else{
				CL02_purgeData();
			}
		}
		
		if(MGR.bufferUsed[0]>BUF_SIZE){
			CL02_purgeData();
		}
		
		uint8_t *data_ptr;
		uint32_t cmd_len;
		
		if(CE32_USB_INTERCOM_RX_DequeueCmd(&IC_USB_handle,&data_ptr,&cmd_len)==0)
		{
			CL02_CmdSvr(data_ptr,cmd_len);
		}
  }
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_ADC1|RCC_PERIPHCLK_SDADC;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  PeriphClkInit.SdadcClockSelection = RCC_SDADCSYSCLK_DIV12;
  PeriphClkInit.Adc1ClockSelection = RCC_ADC1PCLK2_DIV2;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG1);

  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG2);

    /**Configure the Systick interrupt time
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Common config
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Regular Channel
    */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* DAC1 init function */
static void MX_DAC1_Init(void)
{

  DAC_ChannelConfTypeDef sConfig;

    /**DAC Initialization
    */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**DAC channel OUT1 config
    */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SDADC1 init function */
static void MX_SDADC1_Init(void)
{

  SDADC_ConfParamTypeDef ConfParamStruct;

    /**Configure the SDADC low power mode, fast conversion mode,
    slow clock mode and SDADC1 reference voltage
    */
  hsdadc1.Instance = SDADC1;
  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_EXT;
  if (HAL_SDADC_Init(&hsdadc1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure The Regular Mode
    */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Set parameters for SDADC configuration 0 Register
    */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_SE_ZERO_REFERENCE;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SDADC2 init function */
static void MX_SDADC2_Init(void)
{

  SDADC_ConfParamTypeDef ConfParamStruct;

    /**Configure the SDADC low power mode, fast conversion mode,
    slow clock mode and SDADC1 reference voltage
    */
  hsdadc2.Instance = SDADC2;
  hsdadc2.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc2.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc2.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc2.Init.ReferenceVoltage = SDADC_VREF_EXT;
  if (HAL_SDADC_Init(&hsdadc2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure The Regular Mode
    */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc2, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Set parameters for SDADC configuration 0 Register
    */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_SE_ZERO_REFERENCE;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* SPI1 init function */
static void MX_SPI1_Init(void)
{

  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM2 init function */
static void MX_TIM2_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;
  TIM_OC_InitTypeDef sConfigOC;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 4999;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim2);

}

/* TIM16 init function */
static void MX_TIM16_Init(void)
{

  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 7199;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 0;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OC_Init(&htim16) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OnePulse_Init(&htim16, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_OC_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM17 init function */
static void MX_TIM17_Init(void)
{

  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 7199;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 0;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_OnePulse_Init(&htim17, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM18 init function */
static void MX_TIM18_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim18.Instance = TIM18;
  htim18.Init.Prescaler = 71;
  htim18.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim18.Init.Period = 999;
  htim18.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim18) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim18, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);
  /* DMA2_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel4_IRQn);

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PD8   ------> SDADC3_AIN6P
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DOUT_Pin|LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DOUT_Pin LED0_Pin LED1_Pin */
  GPIO_InitStruct.Pin = DOUT_Pin|LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IN2_back_Pin */
  GPIO_InitStruct.Pin = IN2_back_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN2_back_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BUTTON1_Pin BUTTON0_Pin */
  GPIO_InitStruct.Pin = BUTTON1_Pin|BUTTON0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PD8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

int CL02_CmdSvr(uint8_t *data_ptr,uint32_t cmd_len)
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
			memcpy(((uint8_t*)&sysParam),&data_ptr[1],CE32_SYSPARAM_WR_LEN);

			CE32_CL_Init(&cl,&sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
			CE32_STIM_Setup(&STIM_handle[0],sysParam.stim_intensity[0],sysParam.stim_delay[0]+rand()*sysParam.stim_RndDelay[0],sysParam.stim_interval[0],sysParam.pulse_cnt[0],sysParam.pulse_width[0]);
			
			//DF_StimControl_Setting(&sc[0],sysParam.fs,sysParam.trigger_trainStart,sysParam.trigger_trainDuration,sysParam.pulse_width[0],sysParam.stim_interval[0],sysParam.stim_delay[0],sysParam.stim_RndDelay[0],sysParam.trigger_gain[0]);
			//DF_StimControl_Setting(&sc[1],sysParam.fs,sysParam.trigger_trainStart,sysParam.trigger_trainDuration,sysParam.pulse_width[1],sysParam.stim_interval[1],sysParam.stim_delay[1],sysParam.stim_RndDelay[1],sysParam.trigger_gain[1]);
			CE32_CL_Start(&cl);
			CL02_setClParam();
			break;
		case 0x02:
			memcpy(&sysDSP[0],&data_ptr[1],512);
			//CE32_arbitar_Init(&arbitarCal_CH1,&sysDSP[0]);			//Init pointer for signal arbitar
			CE32_CL_Init(&cl,&sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
			CE32_CL_Start(&cl);
			break;
		case 0x06: //Load custom filter
		{
			uint8_t type=*(uint8_t*)&data_ptr[1]; //first data is uint8,type(01=DF1,02=SOS)
			uint16_t ord=*(uint16_t*)&data_ptr[2]; //2nd data is uint16
			filter_custom_coeff.ord=ord;
			int* ptr=(int*)&data_ptr[4];
			memcpy(&filter_custom_coeff.NL,ptr,ord*sizeof(int));
			float* fptr=(float*)&data_ptr[4+ord*sizeof(int)];
			memcpy(&filter_custom_coeff.Num,&fptr[0],3*ord*sizeof(float));
			memcpy(&filter_custom_coeff.Den,&fptr[3*ord],3*ord*sizeof(float));
			sysDSP[0].func1=CE32_FILTER_CUSTOM;
			CE32_CL_Init(&cl,&sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
			CE32_CL_Start(&cl);
			break;
		}
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
				sysParam.stim_mode=0;
				CE32_STIM_DISABLE(&STIM_handle[0]);
			}
			else{
				//sc->Trig_state|=SC_STATE_ON;
				sysParam.stim_mode=1;
				CE32_STIM_ENABLE(&STIM_handle[0]);
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
		case 0x20:
		{
			uint32_t* ptr=(uint32_t*)(&data_ptr[2]);
			uint8_t id=data_ptr[1];
			STIM_handle[id].delay=ptr[0];
			STIM_handle[id].random_delay=ptr[1];
			STIM_handle[id].duration=ptr[2];
			STIM_handle[id].interval=ptr[3];
			STIM_handle[id].cycle=ptr[4];
			break;
		}
		case 0x21:
		{
			uint32_t * ptr=(uint32_t*)(&data_ptr[2]);
			uint8_t id=data_ptr[1];
			sysDSP[id].MAOrd=ptr[0];
			sysDSP[id].func1=ptr[1];
			sysDSP[id].formula=ptr[2];
			CE32_InitFilter(&mainFil[id],&maFil[id],&sysDSP[id]);
			break;
		}
		case 0x40:
		{
			MGR.sysParam|=MGR_SYS_PREVIEW;
			break;
		}
		case 0x41:
		{
			MGR.sysParam=~MGR_SYS_PREVIEW;
			break;
		}
		case 0x60:
		{
			uint32_t* ptr=(uint32_t*)(&data_ptr[1]);
			dsp_gain=ptr[0]/10;
			break;
		}
		case 0x83:
		{
			CE32_STIM_Trig(&STIM_handle[0]);
			break;
		}
	}
	return 0;
}

void CE32_CL_TrigAct(int id) //Triggers when Closed-loop detect a event
{
	CE32_STIM_Trig(&STIM_handle[id]);
}

void CE32_CL_WaitAct(int id)
{
	if(id==0)
	{
		htim2.Instance->ARR=9999;
		htim2.Instance->CCR4=4999;
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
		misc_DigiSig|=MISC_SIG_DSP_WAIT;
		misc_DigiSig&=~(MISC_SIG_DSP_CAL|MISC_SIG_DSP_READY);
	}
}

void CE32_CL_TrainAct(int id) //Triggers when traning starting
{
	if(id==0)
	{
		htim2.Instance->ARR=4999;
		htim2.Instance->CCR4=2499;
		misc_DigiSig|=MISC_SIG_DSP_CAL;
		misc_DigiSig&=~(MISC_SIG_DSP_WAIT|MISC_SIG_DSP_READY);
	}
}

void CE32_CL_ReadyAct(int id) //Triggers when traning is done
{
	if(id==0)
	{
		htim2.Instance->ARR=4999;
		htim2.Instance->CCR4=4999;
		misc_DigiSig|=MISC_SIG_DSP_READY;
		misc_DigiSig&=~(MISC_SIG_DSP_CAL|MISC_SIG_DSP_WAIT);
	}
}


void CE32_STIM_TrigAct(CE32_stimulator* handle)//Trigger Onset action, Define this action in IT
{
	PIN_SET(LED1);
	switch(handle->id){
		case 0:
			misc_DigiSig|=MISC_SIG_TRIG1;
			break;
		case 1:
			misc_DigiSig|=MISC_SIG_TRIG2;
			break;
	}
}
void CE32_STIM_StimAct(CE32_stimulator* handle)//Stimulation Onset action, Define this action in IT
{
	PIN_SET(LED0);
	switch(handle->id){
		case 0:
			misc_DigiSig|=MISC_SIG_STIM1;
			PIN_SET(DOUT);
			break;
		case 1:
			misc_DigiSig|=MISC_SIG_STIM2;
			PIN_SET(DOUT);
			break;
	}
}

void CE32_STIM_TrigStopAct(CE32_stimulator* handle)
{
	PIN_RESET(LED1);
	switch(handle->id){
		case 0:
			misc_DigiSig&=~(MISC_SIG_TRIG1);
			break;
		case 1:
			misc_DigiSig&=~(MISC_SIG_TRIG2);
			break;
	}
}

void CE32_STIM_StimOffAct(CE32_stimulator* handle)
{
	PIN_RESET(LED0);
	switch(handle->id){
		case 0:
			PIN_RESET(DOUT);
			misc_DigiSig&=~MISC_SIG_STIM1;
			break;
		case 1:
			PIN_RESET(DOUT);
			misc_DigiSig&=~MISC_SIG_STIM2;
			break;
	}
}

void CE32_STIM_StimStopAct(CE32_stimulator* handle)
{
	PIN_RESET(LED0);
	switch(handle->id){
		case 0:
			PIN_RESET(DOUT);
			misc_DigiSig&=~MISC_SIG_STIM1;
			break;
		case 1:
			PIN_RESET(DOUT);
			misc_DigiSig&=~MISC_SIG_STIM2;
			break;
	}
}

void CE32_STIM_StimAbortAct(CE32_stimulator* handle)
{
	PIN_RESET(LED0);
	PIN_RESET(LED1);
	misc_DigiSig&=~(MISC_SIG_TRIG1|MISC_SIG_STIM1|MISC_SIG_TRIG2|MISC_SIG_STIM2);
}


int CL02_sendDataPackage(){
	switch(TX_buf_state){
		case 0:	{ //Send header
			TX_buf[0]=0x3c;
			TX_buf[1]=0xAD;
			TX_buf[4]=0x3e;
			float temp=cl.sc[0]->Trig_level;
			temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
			temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
			*((int16_t*)&TX_buf[2])=temp;
			if(CDC_Transmit_FS(TX_buf,4)==USBD_OK){
				TX_buf_state=1;
			}
			break;
		}
		case 1:{ //Send package
			if(CDC_Transmit_FS(((uint8_t*)data_buf)+MGR.outPTR[0],trans_size)==USBD_OK){
				TX_buf_state=2;
				dataMGR_deQueue(&MGR,trans_size,0);
			}
			break;
		}
		case 2:{//Send tail
			if(CDC_Transmit_FS(&TX_buf[4],1)==USBD_OK){
				TX_buf_state=0;
			}
			break;
		}
	}
}

int CL02_purgeData(){
	dataMGR_deQueue(&MGR,trans_size,0);	
	TX_buf_state=0;
}

int CL02_setClParam()
{
	if(sysParam.stim_mode==0){
		CE32_STIM_DISABLE(&STIM_handle[0]);
		CE32_STIM_DISABLE(&STIM_handle[1]);
	}
	else{
		CE32_STIM_ENABLE(&STIM_handle[0]);
		CE32_STIM_ENABLE(&STIM_handle[1]);
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
