#include "CE32_UART_INTERCOM.h"
#include <string.h>

#define PUSH_RX_CMD handle->cmd_RX_idx_rec=ADD_CHKBDR(handle->cmd_RX_idx_rec,1,INTERCOM_CMD_SEQ);\
										handle->cmd_RX_pending++;\
										handle->state&=~CMD_STATE_RX_BUSY;
										
#define ENABLE_TX(__handle__) (__handle__)->UART->CR1|=USART_CR1_TXEIE|USART_CR1_UE|USART_CR1_TE;


void CE32_INTERCOM_Init(CE32_INTERCOM_Handle *handle,UART_HandleTypeDef *huart)
{
	CE32_INTERCOM_UART_Init(huart);
	handle->huart=huart;
	handle->UART=huart->Instance;
}

void CE32_INTERCOM_UART_Init(UART_HandleTypeDef *huart)
{
	
//  huart->Init.BaudRate = 115200;
//  huart->Init.WordLength = UART_WORDLENGTH_8B;
//  huart->Init.StopBits = UART_STOPBITS_1;
//  huart->Init.Parity = UART_PARITY_NONE;
//  huart->Init.Mode = UART_MODE_TX_RX;
//  huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
//  huart->Init.OverSampling = UART_OVERSAMPLING_16;
//  if (HAL_UART_Init(huart) != HAL_OK)
//  {
//    Error_Handler();
//  }
}

void CE32_INTERCOM_StartRX(CE32_INTERCOM_Handle *handle)
{
	handle->huart->Instance->CR1|=USART_CR1_RXNEIE; //Enable RXIE
	__HAL_UART_ENABLE(handle->huart);//Enable Device
}
#ifdef __BLE_HOST
//Return incoming data load length
int CE32_INTERCOM_Incoming_CMD_Len(int index)
{
	int resp=-1;
	switch(index) //Check if received data length fulfilled standard
	{
		case 0x30:
			resp=1;
			break;
		case 0x31:
			resp=1;
			break;
		case 0x40:
			resp=1;
			break;
		case 0x41:
			resp=1;
			break;
		case 0x42:
			resp=2;
			break;
		case 0x90:   //Send system parameter
			resp=513;
			break;
		case 0x91:	//Send DSP1
			resp=513;
			break;
		case 0x92:	//Send DSP2
			resp=513;
			break;
		case 0xAD:	//Incoming data
			resp=9+128;
			break;
		
		case 0xB0:	//Interface: report version
			resp=3;
			break;
		
		case 0xB1:	//Interface: report version
			resp=3;
			break;
		
		case 0xC0:	//Control Port function
			resp=2;
			break;
		
		case 0xF0: //LPF test
			resp=513;
			break;
		case 0xF1: //mainFil1 test
			resp=513;
			break;
		case 0xF2: //mainFIl2 test
			resp=513;
			break;
		case 0xF3: //maFIL1 test
			resp=513;
			break;
		case 0xF4: //maFil2 test
			resp=513;
			break;
	}
	return resp;
}
#else
int CE32_INTERCOM_Incoming_CMD_Len(int index)
{
	int resp=0;
	switch(index){ //Check if received data length fulfilled standard
	case 0x01:
		resp=513;
		break;
	case 0x02:
		resp=513;
		break;
	case 0x03:
		resp=513;								
		break;
	case 0x04:
		resp=1;
		break;
	case 0x05:
		resp=1;
		break;		
	case 0x11:
		resp=2;
		break;									
	case 0x12:
		resp=5;
		break;
	case 0x13:
		resp=4;
		break;
	case 0x14:
		resp=6;
		break;
	case 0x20:
		resp=21;
		break;
	case 0x21:
		resp=21;
		break;
	case 0x22:
		resp=25;
		break;
	case 0x23:
		resp=25;
		break;
	case 0x30: //start logging
		resp=1;
		break;
	case 0x31: //Stop logging
		resp=1;
		break;
	case 0x40:
		resp=1;
		break;
	case 0x41:
		resp=1;
		break;
	case 0x42:
		resp=2;
		break;
	case 0x50:
		resp=1;
		break;
	case 0x51:
		resp=1;
		break;
	case 0x80:
		resp=1;
		break;
	case 0x83:
		resp=2;
		break;
	case 0x90:   //Send system parameter
		resp=1;
		break;
	case 0x91:	//Send DSP1
		resp=1;
		break;
	case 0x92:	//Send DSP2
		resp=1;
		break;
	case 0x93:  //Send Sc
		resp=2;
		break;
	case 0xAB:	//Software reset
		resp=2;
		break;
	case 0xAC:	//DFU mode
		resp=2;
		break;
	case 0xAD:	//Incoming datapackage
		resp=137;
		break;
	
	case 0xB0:	//Interface: report version
		resp=3;
		break;
	
	case 0xB1:	//Interface: report Device info
		resp=25;
		break;
	
	case 0xF0: //LPF test
		resp=513;
		break;
	case 0xF1: //mainFil1 test
		resp=513;
		break;
	case 0xF2: //mainFIl2 test
		resp=513;
		break;
	case 0xF3: //maFIL1 test
		resp=513;
		break;
	case 0xF4: //maFil2 test
		resp=513;
		break;
	default:	//For error received codes
		resp=1;
		break;
	}
	return resp;
}
#endif
//Enqueue data to buffer
//Push to command buffer
int CE32_INTERCOM_RX_Enqueue_Byte(CE32_INTERCOM_Handle *handle_IC,unsigned char data)
{
	return CE32_COMMAND_Enqueue_Byte(&handle_IC->CMD_RX,data);
}


int CE32_INTERCOM_RX_EnqueueCmd(CE32_INTERCOM_Handle *handle_IC)
{
	return CE32_COMMAND_EnqueueCmd_buffered(&handle_IC->CMD_RX);
}

int CE32_INTERCOM_RX_DequeueCmd(CE32_INTERCOM_Handle *handle_IC,uint8_t** data_ptr,uint32_t* cmd_len)
{
	return CE32_COMMAND_DequeueCmd(&handle_IC->CMD_RX,data_ptr,cmd_len);
}

int CE32_INTERCOM_TX_EnqueueCmd(CE32_INTERCOM_Handle *handle_IC,unsigned char* data,int cmd_len)
{
	uint8_t* addr=CE32_COMMAND_Enqueue_Allocate(&handle_IC->CMD_TX,cmd_len);
	memcpy(addr,data,cmd_len);							//Copy the data to TX buffer
	CE32_COMMAND_EnqueueCmd_buffered(&handle_IC->CMD_TX);
	ENABLE_TX(handle_IC);//Enable UART transfer
	return 0;
}

int CE32_INTERCOM_TX_DequeueCmd(CE32_INTERCOM_Handle *handle_IC)
{
	CE32_COMMAND_DequeueCmd(&handle_IC->CMD_TX,NULL,NULL);
	return 0;
}

int CE32_INTERCOM_TX_Dequeue_Byte(CE32_INTERCOM_Handle *handle_IC,uint8_t* data)
{
	return CE32_COMMAND_Dequeue_Byte(&handle_IC->CMD_TX,data);
}

int CE32_INTERCOM_RX_ISR(CE32_INTERCOM_Handle *handle_IC)
{
	int resp=-1;
	CE32_command* handle=&handle_IC->CMD_RX;
	if(INTERCOM_UART_CHECK_RXNEIE(handle_IC))
	{
		if(INTERCOM_CHECK_RXNE(handle_IC))
		{
			handle_IC->UART->SR&=~USART_SR_RXNE;   //Clear RXNE flag
			uint8_t data_temp=handle_IC->UART->DR; //Read data into buffer first
			if(INTERCOM_RX_IS_ON(handle_IC)) //check if already in cmd receiveing mode
			{
				if(handle->stream_in.logging==0)
				{
					if(CE32_INTERCOM_Incoming_CMD_Len(data_temp)>0)
					{
						CE32_COMMAND_Enqueue_Allocate(handle,CE32_INTERCOM_Incoming_CMD_Len(data_temp)); //Preallocate space
					}
					else
					{
						CE32_COMMAND_Enqueue_Byte_Abort(handle);
					}
				}
				if(CE32_COMMAND_Enqueue_BytesLeft(handle)>0)
				{
					if(CE32_INTERCOM_RX_Enqueue_Byte(handle_IC,data_temp)==-1) //Enqueue data to current RX buffer
					{
						//Buffer is full
						CE32_COMMAND_Enqueue_Byte_Abort(handle);
						INTERCOM_STATE_END_RX_RECEIVE(handle_IC); //Reset flags to no cmd detected mode
						resp=0;
					}
				}
				else
				{
					if(data_temp==INTERCOM_PKG_TAIL) //If end package detectected at desinated length
					{
						CE32_INTERCOM_RX_EnqueueCmd(handle_IC);
						INTERCOM_STATE_END_RX_RECEIVE(handle_IC); //Reset flags to no cmd detected mode
						resp=0;
					}
					else
					{
						CE32_COMMAND_Enqueue_Byte_Abort(handle);
						INTERCOM_STATE_END_RX_RECEIVE(handle_IC); //Reset flags to no cmd detected mode
						resp=-1;
					}
				}
			}
			else //If recording is not started
			{
				if(data_temp==INTERCOM_PKG_HEAD)
				{
					INTERCOM_STATE_START_RX_RECEIVE(handle_IC);
					resp=0;
				}
			}
		}
	}
	
	return resp;
}

int CE32_INTERCOM_TX_ISR(CE32_INTERCOM_Handle *handle_IC)
{
	int resp=-1;
	CE32_command* handle=&handle_IC->CMD_TX;
	if(INTERCOM_UART_CHECK_TXEIE(handle_IC))
	{
		if(INTERCOM_CHECK_TXE(handle_IC))
		{
			if(CE32_COMMAND_GetPendingCounts(handle)>0)
			{
				uint8_t data;
				if(CE32_INTERCOM_TX_Dequeue_Byte(handle_IC,&data)==0)
				{
					INTERCOM_TX_SEND(handle_IC,data);
					resp=0;
				}
				else
				{
					//Disable TX transmission
					INTERCOM_UART_TXEIE_DISABLE(handle_IC);
					resp=0;
				}
			}
			else
			{
				//Disable TX transmission
				INTERCOM_UART_TXEIE_DISABLE(handle_IC);
				resp=0;
			}
		}
	}
	return resp;
}

int CE32_INTERCOM_TX_WaitTillSent(CE32_INTERCOM_Handle *handle_IC)
{
	CE32_command *handle=&handle_IC->CMD_TX;
	while(handle->cmd_pending>0);
	return 0;
}
