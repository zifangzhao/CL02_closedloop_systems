/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
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

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

DAC_HandleTypeDef hdac1;

I2C_HandleTypeDef hi2c1;

SDADC_HandleTypeDef hsdadc1;
SDADC_HandleTypeDef hsdadc2;
SDADC_HandleTypeDef hsdadc3;
DMA_HandleTypeDef hdma_sdadc1;
DMA_HandleTypeDef hdma_sdadc2;
DMA_HandleTypeDef hdma_sdadc3;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;
TIM_HandleTypeDef htim18;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
int16_t adc_buf[2];
int16_t data_buf[BUF_SIZE/2];
dataMGR MGR;
CE32_systemParam 	sysParam;

CE32_dspParam    sysDSP[2];
CE32_Filter			 mainFil[2];
CE32_Filter   	 maFil[2];
CE32_StimControl sc[2];
CE32_CL					 cl;
const int trans_size=256;
uint16_t misc_DigiSig;
uint16_t test;
float dsp_gain=10;
extern HD32_filter_coeff filter_custom_coeff[2];

uint8_t TX_buf[8]; //TX buffer, with extra bytes as reserve
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
static void MX_TIM2_Init(void);
static void MX_TIM18_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SDADC3_Init(void);
static void MX_TIM5_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
int CL02_CmdSvr(uint8_t *data_ptr,uint32_t cmd_len);
int CL02_sendDataPackage();
int CL02_purgeData();
int CL02_setClParam();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_TIM2_Init();
  MX_TIM18_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_USART2_UART_Init();
  MX_SDADC3_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
	//HAL_TIM_Base_Start(&htim2);
	//HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);
	
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
	CE32_STIM_Setup(&STIM_handle[1],sysParam.stim_intensity[0],sysParam.stim_delay[0]+rand()*sysParam.stim_RndDelay[0],sysParam.stim_interval[0],sysParam.pulse_cnt[0],sysParam.pulse_width[0]);
	//CE32_STIM_Setup(&STIM_handle[1],sysParam.stim_intensity[1],sysParam.stim_delay[1]+rand()*sysParam.stim_RndDelay[1],sysParam.stim_interval[1],sysParam.pulse_cnt[1],sysParam.pulse_width[1]);

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_ADC1
                              |RCC_PERIPHCLK_SDADC;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  PeriphClkInit.SdadcClockSelection = RCC_SDADCSYSCLK_DIV12;
  PeriphClkInit.Adc1ClockSelection = RCC_ADC1PCLK2_DIV2;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG1);
  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG2);
  HAL_PWREx_EnableSDADC(PWR_SDADC_ANALOG3);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
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
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief DAC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC1_Init(void)
{

  /* USER CODE BEGIN DAC1_Init 0 */

  /* USER CODE END DAC1_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC1_Init 1 */

  /* USER CODE END DAC1_Init 1 */

  /** DAC Initialization
  */
  hdac1.Instance = DAC1;
  if (HAL_DAC_Init(&hdac1) != HAL_OK)
  {
    Error_Handler();
  }

  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac1, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC1_Init 2 */

  /* USER CODE END DAC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
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
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SDADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDADC1_Init(void)
{

  /* USER CODE BEGIN SDADC1_Init 0 */

  /* USER CODE END SDADC1_Init 0 */

  SDADC_ConfParamTypeDef ConfParamStruct = {0};

  /* USER CODE BEGIN SDADC1_Init 1 */

  /* USER CODE END SDADC1_Init 1 */

  /** Configure the SDADC low power mode, fast conversion mode,
  slow clock mode and SDADC1 reference voltage
  */
  hsdadc1.Instance = SDADC1;
  hsdadc1.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc1.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc1.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc1.Init.ReferenceVoltage = SDADC_VREF_EXT;
  if (HAL_SDADC_Init(&hsdadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure The Regular Mode
  */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc1, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    Error_Handler();
  }

  /** Set parameters for SDADC configuration 0 Register
  */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_SE_ZERO_REFERENCE;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc1, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDADC1_Init 2 */

  /* USER CODE END SDADC1_Init 2 */

}

/**
  * @brief SDADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDADC2_Init(void)
{

  /* USER CODE BEGIN SDADC2_Init 0 */

  /* USER CODE END SDADC2_Init 0 */

  SDADC_ConfParamTypeDef ConfParamStruct = {0};

  /* USER CODE BEGIN SDADC2_Init 1 */

  /* USER CODE END SDADC2_Init 1 */

  /** Configure the SDADC low power mode, fast conversion mode,
  slow clock mode and SDADC1 reference voltage
  */
  hsdadc2.Instance = SDADC2;
  hsdadc2.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc2.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc2.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc2.Init.ReferenceVoltage = SDADC_VREF_EXT;
  if (HAL_SDADC_Init(&hsdadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure The Regular Mode
  */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc2, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    Error_Handler();
  }

  /** Set parameters for SDADC configuration 0 Register
  */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_SE_ZERO_REFERENCE;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc2, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDADC2_Init 2 */

  /* USER CODE END SDADC2_Init 2 */

}

/**
  * @brief SDADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDADC3_Init(void)
{

  /* USER CODE BEGIN SDADC3_Init 0 */

  /* USER CODE END SDADC3_Init 0 */

  SDADC_ConfParamTypeDef ConfParamStruct = {0};

  /* USER CODE BEGIN SDADC3_Init 1 */

  /* USER CODE END SDADC3_Init 1 */

  /** Configure the SDADC low power mode, fast conversion mode,
  slow clock mode and SDADC1 reference voltage
  */
  hsdadc3.Instance = SDADC3;
  hsdadc3.Init.IdleLowPowerMode = SDADC_LOWPOWER_NONE;
  hsdadc3.Init.FastConversionMode = SDADC_FAST_CONV_DISABLE;
  hsdadc3.Init.SlowClockMode = SDADC_SLOW_CLOCK_DISABLE;
  hsdadc3.Init.ReferenceVoltage = SDADC_VREF_EXT;
  if (HAL_SDADC_Init(&hsdadc3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure The Regular Mode
  */
  if (HAL_SDADC_SelectRegularTrigger(&hsdadc3, SDADC_SOFTWARE_TRIGGER) != HAL_OK)
  {
    Error_Handler();
  }

  /** Set parameters for SDADC configuration 0 Register
  */
  ConfParamStruct.InputMode = SDADC_INPUT_MODE_SE_ZERO_REFERENCE;
  ConfParamStruct.Gain = SDADC_GAIN_1;
  ConfParamStruct.CommonMode = SDADC_COMMON_MODE_VSSA;
  ConfParamStruct.Offset = 0;
  if (HAL_SDADC_PrepareChannelConfig(&hsdadc3, SDADC_CONF_INDEX_0, &ConfParamStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Regular Channel
  */
  if (HAL_SDADC_AssociateChannelConfig(&hsdadc3, SDADC_CHANNEL_6, SDADC_CONF_INDEX_0) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_SDADC_ConfigChannel(&hsdadc3, SDADC_CHANNEL_6, SDADC_CONTINUOUS_CONV_ON) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDADC3_Init 2 */

  /* USER CODE END SDADC3_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 7199;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 9999;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 4999;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 7199;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 65535;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim16, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
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
    Error_Handler();
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
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 7199;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 65535;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OnePulse_Init(&htim17, TIM_OPMODE_SINGLE) != HAL_OK)
  {
    Error_Handler();
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
    Error_Handler();
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
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief TIM18 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM18_Init(void)
{

  /* USER CODE BEGIN TIM18_Init 0 */

  /* USER CODE END TIM18_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM18_Init 1 */

  /* USER CODE END TIM18_Init 1 */
  htim18.Instance = TIM18;
  htim18.Init.Prescaler = 71;
  htim18.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim18.Init.Period = 999;
  htim18.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim18) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim18, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM18_Init 2 */

  /* USER CODE END TIM18_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  /* DMA2_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Channel5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DOUT1_Pin|DOUT_Pin|LED1_Pin|LED0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DOUT1_Pin DOUT_Pin LED1_Pin LED0_Pin */
  GPIO_InitStruct.Pin = DOUT1_Pin|DOUT_Pin|LED1_Pin|LED0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : IN2_back_Pin */
  GPIO_InitStruct.Pin = IN2_back_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IN2_back_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 BUTTON0_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_14|BUTTON0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
			uint8_t id = *(uint8_t*)&data_ptr[1]; //index of custom filter
			uint8_t type=*(uint8_t*)&data_ptr[2]; //first data is uint8,type(01=DF1,02=SOS)
			uint16_t ord=*(uint16_t*)&data_ptr[3]; //2nd data is uint16
			filter_custom_coeff[id].ord=ord;
			int* ptr=(int*)&data_ptr[5];
			memcpy(&filter_custom_coeff[id].NL,ptr,ord*sizeof(int));
			float* fptr=(float*)&data_ptr[5+ord*sizeof(int)];
			memcpy(&filter_custom_coeff[id].Num,&fptr[0],3*ord*sizeof(float));
			memcpy(&filter_custom_coeff[id].Den,&fptr[3*ord],3*ord*sizeof(float));
//			sysDSP[id].func1=CE32_FILTER_CUSTOM;
//			CE32_CL_Init(&cl,&sysParam,sysDSP,mainFil,maFil,sc); //Initialize Closed-loop unit
//			CE32_CL_Start(&cl);
			break;
		}
		case 0x10://Gain setting realtime
		{
			float v;
			uint8_t *ptr;
			uint8_t id = data_ptr[1];
			ptr=(uint8_t*)&v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[i+2];
			}
			sc[id].Trig_gain=v;
			sc[id].Trig_level=sc[id].Trig_gain*sc[id].Trig_mean;
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
		case 0x12: //Trig mode
		{
			if(data_ptr[1]==0){
				cl.trig_mode = 0;
				STIM_handle[0].trig_mode = 0;
				STIM_handle[1].trig_mode = 0;
			}
			else
			{
				cl.trig_mode = 1;
				STIM_handle[0].trig_mode = 1;
				STIM_handle[1].trig_mode = 1;
			}
			break;
		}
		case 0x14:
		{
			float v;
			uint8_t *ptr;
			uint8_t id = data_ptr[1];
			ptr=(uint8_t*)&v;
			for(int i=0;i<4;i++){
				*(ptr+i)=data_ptr[i+2];
			}
			sc[id].Trig_level=v;
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
		htim5.Instance->ARR=9999;
		htim5.Instance->CCR4=4999;
		HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);
		misc_DigiSig|=MISC_SIG_DSP_WAIT;
		misc_DigiSig&=~(MISC_SIG_DSP_CAL|MISC_SIG_DSP_READY);
	}
}

void CE32_CL_TrainAct(int id) //Triggers when traning starting
{
	if(id==0)
	{
		htim5.Instance->ARR=4999;
		htim5.Instance->CCR4=2499;
		misc_DigiSig|=MISC_SIG_DSP_CAL;
		misc_DigiSig&=~(MISC_SIG_DSP_WAIT|MISC_SIG_DSP_READY);
	}
}

void CE32_CL_ReadyAct(int id) //Triggers when traning is done
{
	if(id==0)
	{
		htim5.Instance->ARR=4999;
		htim5.Instance->CCR4=4999;
		misc_DigiSig|=MISC_SIG_DSP_READY;
		misc_DigiSig&=~(MISC_SIG_DSP_CAL|MISC_SIG_DSP_WAIT);
	}
}

void CE32_CL_TrigInitAct(int id)
{
	MX_TIM16_Init();
	MX_TIM17_Init();
	CE32_STIM_StimOffAct(&STIM_handle[0]);
	CE32_STIM_StimOffAct(&STIM_handle[1]);
	CE32_STIM_Init(&STIM_handle[0],&htim16,0,0);
	CE32_STIM_Init(&STIM_handle[1],&htim17,0,1);
	CE32_STIM_Setup(&STIM_handle[0],sysParam.stim_intensity[0],sysParam.stim_delay[0]+rand()*sysParam.stim_RndDelay[0],sysParam.stim_interval[0],sysParam.pulse_cnt[0],sysParam.pulse_width[0]);
	CE32_STIM_Setup(&STIM_handle[1],sysParam.stim_intensity[1],sysParam.stim_delay[1]+rand()*sysParam.stim_RndDelay[1],sysParam.stim_interval[1],sysParam.pulse_cnt[1],sysParam.pulse_width[1]);

}

void CE32_STIM_TrigAct(CE32_stimulator* handle)//Trigger Onset action, Define this action in IT
{
	PIN_SET(LED1);
	PIN_SET(DOUT1);
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
	if(sysParam.stim_mode ==1)
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
}

void CE32_STIM_TrigStopAct(CE32_stimulator* handle)
{
	PIN_RESET(LED1);
	PIN_RESET(DOUT1);
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
		
			float temp=cl.sc[0]->Trig_level;
			temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
			temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
			*((int16_t*)&TX_buf[2])=temp;
			temp=cl.sc[1]->Trig_level;
			temp=temp>INT16_MAX?INT16_MAX:temp;			//Make sure temp is not saturated 
			temp=temp<INT16_MIN?INT16_MIN:temp;			//Make sure temp is not saturated 
			*((int16_t*)&TX_buf[4])=temp;
			if(CDC_Transmit_FS(TX_buf,6)==USBD_OK){
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
			uint8_t TX_tail = 0x3e;
			if(CDC_Transmit_FS(&TX_tail,1)==USBD_OK){
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
  * @retval None
  */
void Error_Handler(void)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
