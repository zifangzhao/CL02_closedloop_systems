#include "CE32_macro.h"
#include "CE32_COMMAND.h"
#include "stm32f3xx_hal.h"
#include "usbd_cdc_if.h"
#include "usbd_cdc.h"

#ifndef __CE32_USB_INTERCOM

#define __CE32_USB_INTERCOM
//#define __BLE_HOST
//#define INTERCOM_CMD_SEQ 8
//#define INTERCOM_CMD_BUFSIZE 1024

/*
INTERCOM class for handling data communication between Ephys boards

1. Handling incoming data stream
	(a) Save incoming data to buffer
	(b) Decode incoming stream to commands
	(c) Calling process command function to receive data to outside(external)
2. Handling outgoing data stream
	(a) Calling function to push data into outgoing buffer
	(b) Export data by byte(interrupt service)
	(*c) Export data by DMA 
		i.  return current outgoing buffer location
		ii. calling function to mark the end of one command transmission

Need command length to be known
		
Possible states:
RX
	Cmd receiving
	Pending
TX
	
*/

//Marco
#define INTERCOM_USB_RX_IS_ON(__IC_handle__) (((__IC_handle__)->state&INTERCOM_STATE_RECIVING)!=0)
#define INTERCOM_USB_STATE_START_RX_RECEIVE(__IC_handle__) ((__IC_handle__)->state|=(INTERCOM_STATE_RECIVING))
#define INTERCOM_USB_STATE_END_RX_RECEIVE(__IC_handle__) ((__IC_handle__)->state&=~(INTERCOM_STATE_RECIVING))

#define INTERCOM_PKG_HEAD							0x3c
#define INTERCOM_PKG_TAIL							0x3e

#define INTERCOM_STATE_IDLE  					0x0001
#define INTERCOM_STATE_TXON  					0x0002
#define INTERCOM_STATE_SLEEP 					0x0004
#define INTERCOM_STATE_CONFIG 				0x0008
#define INTERCOM_STATE_CONNECTED 			0x0010
#define INTERCOM_STATE_ERROR  				0x0020
#define INTERCOM_STATE_RXON						0x0040
#define INTERCOM_STATE_MASTER					0x0080
#define INTERCOM_STATE_STOP 					0x0100
#define INTERCOM_STATE_RECIVING				0x0200
#define INTERCOM_STATE_CMD_RESP_PEND	0x0400
#define INTERCOM_STATE_WAITRESP  			0x0800
#define INTERCOM_STATE_CE32_CMD  			0x1000
#define INTERCOM_STATE_CE32_CMD_PEND  0x2000



typedef struct 
{
	CE32_command CMD_TX,CMD_RX;					//command object
	uint32_t state;
	int id;
	int type;
	int version;
}CE32_USB_INTERCOM_Handle;	

#define CE32_DEVICE_STATE_IDLE				0x0001
#define CE32_DEVICE_STATE_REC					0x0002
#define CE32_DEVICE_STATE_PREV				0x0004

void CE32_USB_INTERCOM_Init(CE32_USB_INTERCOM_Handle *handle);		//Initialize instance
void CE32_USB_INTERCOM_StartRX(CE32_USB_INTERCOM_Handle *handle);
int CE32_USB_INTERCOM_Incoming_CMD_Len(int index);																						//Returning correct command length
int CE32_USB_INTERCOM_RX_EnqueueCmd(CE32_USB_INTERCOM_Handle *handle_IC);
int CE32_USB_INTERCOM_RX_Enqueue_Byte(CE32_USB_INTERCOM_Handle *handle,unsigned char data); 				//Handling incoming data
int CE32_USB_INTERCOM_RX_DequeueCmd(CE32_USB_INTERCOM_Handle *handle,uint8_t** data_ptr,uint32_t* cmd_len);	//Pass the buffer and length of the command to be processed, advance the command ptr to next
int CE32_USB_INTERCOM_TX_EnqueueCmd(CE32_USB_INTERCOM_Handle *handle,unsigned char* data,int cmd_len);
int CE32_USB_INTERCOM_TX_DequeueCmd(CE32_USB_INTERCOM_Handle *handle);
int CE32_USB_INTERCOM_TX_Dequeue_Byte(CE32_USB_INTERCOM_Handle *handle,uint8_t* data);
int CE32_USB_INTERCOM_RX_ISR(CE32_USB_INTERCOM_Handle *handle,uint8_t data);
int CE32_USB_INTERCOM_TX_ISR(CE32_USB_INTERCOM_Handle *handle);
int CE32_USB_INTERCOM_TX_WaitTillSent(CE32_USB_INTERCOM_Handle *handle);
#endif
