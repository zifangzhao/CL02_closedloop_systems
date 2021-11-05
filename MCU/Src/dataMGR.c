#include "dataMGR.h"

void dataMGR_init(dataMGR* MGR,char* dataPtr,unsigned long dataSize)
{
	MGR->dataPtr=dataPtr;
	MGR->dataSize=dataSize;
	
	MGR->inPTR=0;
	for(int i=0;i<outPTR_num;i++){
		MGR->outPTR[i]=0;
		MGR->bufferUsed[i]=0;
		MGR->bufferMAX[i]=0;
	}
	MGR->logState=0;
	MGR->sysParam=0;
}

void dataMGR_init_DMA(dataMGR* MGR,char* dataPtr,unsigned long dataSize,__IO uint32_t*RX_NDTR,__IO uint32_t*TX_NDTR)
{
	MGR->dataPtr=dataPtr;
	MGR->dataSize=dataSize;
	
	MGR->inPTR=0;
	for(int i=0;i<outPTR_num;i++){
		MGR->outPTR[i]=0;
		MGR->bufferUsed[i]=0;
		MGR->bufferMAX[i]=0;
	}
	MGR->logState=0;
	MGR->sysParam=0;
	MGR->DMA_RX_NDTR=RX_NDTR;
	MGR->DMA_TX_NDTR=TX_NDTR;
}

void dataMGR_init_515B(dataMGR* MGR,char* dataPtr,unsigned long dataSize)
{
	unsigned long size;
	int cnt=dataSize/515;
	size=515*cnt;
	MGR->dataPtr=dataPtr;
	MGR->dataSize=size;
	MGR->inPTR=1;
	for(int i=0;i<outPTR_num;i++){
		MGR->outPTR[i]=0;
		MGR->bufferUsed[i]=0;
		MGR->bufferMAX[i]=0;
	}
	MGR->logState=0;
	MGR->sysParam=0;
	for(int i=0;i<cnt;i++){
		MGR->dataPtr[i*515]=0xFC; //SD_CMD25_TOKEN_START
	}
}

void dataMGR_IncNrec(CE32_systemLog* sysLog)
{
	if(sysLog->Nrec<127){
		sysLog->Nrec++;
	}
}

void syslog_init(CE32_systemLog* sysLog,unsigned int* filePTR)
{
	//if(sysLog->Nrec>126){
	//	sysLog->Nrec=0;
	//}
	for(int i=sysLog->Nrec;i>1;i--){
		if(sysLog->log[i-1]<LOG_ADDR+1){
			sysLog->Nrec--;
		}
	}
	dataMGR_IncNrec(sysLog);
	if(sysLog->Nrec==1){
		*filePTR=DATA_ADDR;//LOG_ADDR+1;
	}
	else{
		*filePTR=sysLog->log[sysLog->Nrec-2];
		if(*filePTR<LOG_ADDR+1){
			sysLog->Nrec=1;
			*filePTR=DATA_ADDR;//LOG_ADDR+1;
		}
	}
}

void CE32_init(CE32_systemParam* sys)
{
	sys->rec_ch=28;		//Do not edit, this is the initial state(3 states before 0);
	sys->cmd_ch=31;
}

void CE32_arbitarCal_formula1(uchar* chOrd, short* dataPacket, float* output)
{
	*output=dataPacket[chOrd[0]];
}

void CE32_arbitarCal_formula2(uchar* chOrd, short* dataPacket, float* output)
{
	*output=dataPacket[chOrd[0]]-dataPacket[chOrd[1]];
}

void CE32_arbitarCal_formula3(uchar* chOrd, short* dataPacket, float* output)
{
	*output=2*dataPacket[chOrd[0]]-dataPacket[chOrd[1]]-dataPacket[chOrd[2]];
}

void CE32_arbitar_Init(void (**arbitarCal_CH)(uchar* ch_ord,short* data,float* output),CE32_dspParam* sysDSP)
{
	switch(sysDSP->formula)
	{
		case 0:
			*arbitarCal_CH=CE32_arbitarCal_formula1;
			break;
		case 1:
			*arbitarCal_CH=CE32_arbitarCal_formula2;
			break;
		case 2:
			*arbitarCal_CH=CE32_arbitarCal_formula3;
			break;
	}
}
