#ifndef _CL02_SVR
#define _CL02_SVR

#define ARM_MATH_CM4
#include "stm32f3xx_hal.h"
#include "arm_math.h"
#include "dataMGR.h"
#include "HD32_filter.h"
#include "CE32_ClosedLoop.h"
#include <stdlib.h>


int CL02_CmdSvr(uint8_t *data_ptr,uint32_t cmd_len,dataMGR *MGR,CE32_systemParam *sysParam,CE32_dspParam *sysDSP,CE32_Filter *mainFil,CE32_MA_Filter *maFil,CE32_StimControl *sc,CE32_CL* cl);
#endif
