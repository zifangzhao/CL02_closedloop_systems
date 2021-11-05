#include "stm32f3xx_hal.h"

#ifndef __CE32_macro

#define __CE32_macro
#define ADD_CHKBDR(A,B,BDR) ((A)+(B))<(BDR)?((A)+(B)):((A)+(B)-(BDR))
#define PIN_SET(__HANDLE__) (__HANDLE__##_GPIO_Port->BSRR|=__HANDLE__##_Pin)
#define PIN_RESET(__HANDLE__) (__HANDLE__##_GPIO_Port->BSRR|=__HANDLE__##_Pin<<16)

#define CE32_NOP(__x__) (for(int i=0;i<__x__;i++){__nop();})
typedef struct
{
  __IO uint32_t ISR;   /*!< DMA interrupt status register */
  __IO uint32_t Reserved0;
  __IO uint32_t IFCR;  /*!< DMA interrupt flag clear register */
} DMA_Base_Registers;


#define SET_STIM_DAC(__HANDLE__)  DAC1->DHR12L1=(65535-__HANDLE__);
#define STIM1_ON		//LD1_GPIO_Port->BSRR=LD1_Pin
#define STIM1_OFF		//LD1_GPIO_Port->BSRR=(LD1_Pin)<<16U
#define STIM2_ON		//LD2_GPIO_Port->BSRR=LD2_Pin
#define STIM2_OFF		//LD2_GPIO_Port->BSRR=(LD2_Pin)<<16U

//#define __PROFILE
//#define __BLE_DEBUG
//#define __PWRSAVE_SLEEP

typedef struct
{
	uint32_t rec_svr[3];	
	uint32_t prev_svr[3];
	uint32_t cmd_svr[3];
	uint32_t MAIN_HS[3];
	uint32_t MAIN_LS[3];
	uint32_t SD_Write[3];
	uint32_t BLE_Send[3];
} Profile_CNT;

#ifdef __PROFILE
	extern Profile_CNT profile;
	#endif
#endif

#ifdef __PROFILE
	#define Profile_ADDR_20K 0
	#define Profile_ADDR_1250 1
	#define Profile_ADDR_SD_START 2
	#define Profile_ADDR_SD_END 3
	#define Profile_ADDR_PREV_START 4
	#define Profile_ADDR_PREV_END 5
	#define Profile_ADDR_MAINLOOP_START 6
	#define Profile_ADDR_MAINLOOP_END 7
#endif
