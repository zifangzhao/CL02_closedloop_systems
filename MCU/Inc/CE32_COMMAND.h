#include "CE32_macro.h"
#include "stm32f3xx_hal.h"

#ifndef __CE32_COMMAND

#define __CE32_COMMAND

#define CMD_SEQ 8
#define CMD_MAXSIZE 514
#define CMD_BUFSIZE CMD_MAXSIZE*CMD_SEQ
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

typedef struct 
{
	uint8_t* addr;
	uint32_t idx;
	uint32_t len;
	uint8_t logging;
}CE32_MEMORY_PTR;

typedef struct 
{
	uint16_t state;
	uint8_t  buffer[CMD_BUFSIZE]; //internal 
	uint8_t* cmd_buf[CMD_SEQ];	// command buffer
	uint8_t* cmd_ptr[CMD_SEQ];			// command ptr
	uint32_t cmd_len[CMD_SEQ];		  // command length
	
	CE32_MEMORY_PTR stream_in,stream_out;
	
	uint32_t cmd_idx_in;								//Incoming command idx
	uint32_t cmd_ptr_in;								//Incoming command ptr in Current page of Buffer
	uint32_t cmd_idx_out;								//outgoing command idx
	
	uint32_t cmd_pending;								//Count for pending commands to be processed
}CE32_command;


int CE32_COMMAND_EnqueueCmd(CE32_command *handle,unsigned char* data,int cmd_len);
int CE32_COMMAND_EnqueueCmd_MEMCopy(CE32_command *handle,unsigned char* data,int cmd_len);
uint8_t* CE32_COMMAND_Enqueue_Allocate(CE32_command *handle,int cmd_len); 										//Prelocate buffer for incoming command stream
int CE32_COMMAND_Enqueue_Byte(CE32_command *handle,unsigned char data); 											//Handling incoming data
int CE32_COMMAND_Enqueue_BytesLeft(CE32_command *handle); 											//
int CE32_COMMAND_Enqueue_Byte_Abort(CE32_command *handle); 								//Abort current data enqueue byte
int CE32_COMMAND_EnqueueCmd_buffered(CE32_command *handle); 					//Push memory in receiving buffer to command
int CE32_COMMAND_DequeueCmd(CE32_command *handle,uint8_t** data_ptr,uint32_t* cmd_len);	//Pass the buffer and length of the command to be processed, advance the command ptr to next
int CE32_COMMAND_Dequeue_Byte(CE32_command *handle,uint8_t* data);
int CE32_COMMAND_Dequeue_BytesLeft(CE32_command *handle); 											//
int CE32_COMMAND_Dequeue_Byte_Abort(CE32_command *handle);
int CE32_COMMAND_GetPendingCounts(CE32_command *handle);

#define CMD_ADD_CHK(A,B,BDR) ((A)+(B))<(BDR)?((A)+(B)):((A)+(B)-(BDR))
#endif
