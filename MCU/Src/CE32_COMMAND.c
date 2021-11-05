#include "CE32_COMMAND.h"
#include <string.h>

//Encode external buffer to command list
int CE32_COMMAND_EnqueueCmd(CE32_command *handle,unsigned char* data,int cmd_len)
{
	handle->cmd_buf[handle->cmd_idx_in]=data;
	handle->cmd_len[handle->cmd_idx_in]=cmd_len;
	handle->cmd_idx_in=CMD_ADD_CHK(handle->cmd_idx_in,1,CMD_SEQ); //Advance command pointer
	handle->cmd_pending++;
	return 0;
}

int CE32_COMMAND_EnqueueCmd_MEMCopy(CE32_command *handle,unsigned char* data,int cmd_len)
{
	uint8_t* ptr=CE32_COMMAND_Enqueue_Allocate(handle,cmd_len);//allocate memory in stack
	memcpy(ptr,data,cmd_len);
	CE32_COMMAND_EnqueueCmd_buffered(handle);//Push into cmd list

	return 0;
}

uint8_t* CE32_COMMAND_Enqueue_Allocate(CE32_command *handle,int cmd_len) 										//Prelocate buffer for incoming command stream
{
	if(handle->cmd_ptr_in+cmd_len>=CMD_BUFSIZE)
	{
		handle->cmd_ptr_in=0; 						//If the memory left in the page is not enough for the incoming command, start from begining.
	}
	handle->stream_in.addr=&handle->buffer[handle->cmd_ptr_in];	//allocate buffer start location to Streamer
	handle->stream_in.len=cmd_len; //Set target length
	handle->stream_in.idx=0; //Reset incoming pointer
	handle->stream_in.logging=1; //Set logging state
	handle->cmd_ptr_in=CMD_ADD_CHK(handle->cmd_ptr_in,cmd_len,CMD_BUFSIZE);//Advance incoming pointer to next location
	return handle->stream_in.addr;
}

int CE32_COMMAND_Enqueue_Byte(CE32_command *handle,unsigned char data)								//Handling incoming data
{
	if(handle->stream_in.idx<handle->stream_in.len)
	{
		handle->stream_in.addr[handle->stream_in.idx++]=data;	//push data into buffer
//		if(handle->stream_in.idx==handle->stream_in.len)
//		{
//			CE32_COMMAND_EnqueueCmd_buffered(handle);
//		}
	}
	else
	{
		handle->stream_in.idx++; //Still count evenif it is exceeded
		return -1; //For length error,Buffer OVERRUN
	}
 return 0;
}

int CE32_COMMAND_Enqueue_BytesLeft(CE32_command *handle)
{
	return handle->stream_in.len-handle->stream_in.idx;
}

int CE32_COMMAND_Enqueue_Byte_Abort(CE32_command *handle) 								//Abort current data enqueue byte
{
		handle->stream_in.logging=0;
		handle->stream_in.idx=0; //Reset incoming pointer
	return 0;
}

int CE32_COMMAND_EnqueueCmd_buffered(CE32_command *handle) 					//Push memory in receiving buffer to command
{
	handle->stream_in.logging=0; //Reset logging state
	CE32_COMMAND_EnqueueCmd(handle,handle->stream_in.addr,handle->stream_in.len);
	return 0;
}

int CE32_COMMAND_DequeueCmd(CE32_command *handle,uint8_t** data_ptr,uint32_t* cmd_len)	//Pass the buffer and length of the command to be processed, advance the command ptr to next
{
	if(handle->cmd_pending>0)
	{
		*data_ptr=handle->cmd_buf[handle->cmd_idx_out];  //return current output buffer location
		*cmd_len=handle->cmd_len[handle->cmd_idx_out];
		handle->cmd_idx_out=CMD_ADD_CHK(handle->cmd_idx_out,1,CMD_SEQ);//Advance outgoing pointer to next location
		handle->cmd_pending--;
		return 0;
	}
	else
	{
		return -1;
	}
}

int CE32_COMMAND_Dequeue_Byte(CE32_command *handle,uint8_t* data)	//Continuous off load until empty
{
	if(handle->cmd_pending>0)	//if there still command in the buffer
	{
		if(handle->stream_out.logging==0) //if outgoing stream has not started
		{
			handle->stream_out.addr=handle->cmd_buf[handle->cmd_idx_out];	//assign nex command start address to out going streamer
			handle->stream_out.len=handle->cmd_len[handle->cmd_idx_out];	 	//Set target length
			handle->stream_out.idx=0;
			handle->stream_out.logging=1;
		}
		*data=handle->stream_out.addr[handle->stream_out.idx++];
		if(handle->stream_out.idx>=handle->stream_out.len)
		{
			handle->stream_out.logging=0;
			CE32_COMMAND_DequeueCmd(handle,NULL,NULL);
		}
		return 0;
	}
	else
	{
		return -1; //If no command to send
	}
	
}

int CE32_COMMAND_Dequeue_BytesLeft(CE32_command *handle)
{
	return handle->stream_out.len-handle->stream_out.idx;
}


int CE32_COMMAND_Dequeue_Byte_Abort(CE32_command *handle)
{
	handle->stream_out.idx=0;
	handle->stream_out.logging=0;
	return 0;
}

int CE32_COMMAND_GetPendingCounts(CE32_command *handle)
{
	return handle->cmd_pending;
}
