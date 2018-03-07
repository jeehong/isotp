#include "isotp.h"
#include <string.h>

#define time_after(a,b)  \
((long)(b) - (long)(a) < 0)

#define time_before(a,b) time_after(b,a)

#define time_interval(now,pre) ((long)(now) - (long)(pre)) 

/* N_PCI type values in bits 7-4 of N_PCI bytes */
enum n_pci_type_e
{
	N_PCI_SF = 0x00,  /* single frame */
	N_PCI_FF = 0x10,  /* first frame */
	N_PCI_CF = 0x20,  /* consecutive frame */
	N_PCI_FC = 0x30,  /* flow control */
};

/*
 * ISO-15765-2-8.5.4.2
 * that the SN shall start with zero for all segmented messages; 
 * the FirstFrame (FF) shall be assigned the value zero;
 * it does not include an explicit SequenceNumber in the N_PCI field 
 * but shall be treated as the segment number zero;
 */
#define ISOTP_DEFAULT_SN	1UL

/*
 * ISO-15765-2-8.5.5.6
 * If an FC N_PDU message is received with a reserved STmin parameter value,
 * then the sending network entit shall use the longest STmin value specified
 * by this part of ISO 15765 (0x7F = 127 ms) instead of the value
 * received from the receiving network entity 
 * for the duration of the on-going segmented message transmission.
 */
#define ISOTP_DEFAULT_STmin	(0x7F)

/*
 * 0x00 BlockSize (BS)
 * The BS parameter value 0 shall be used to indicate to the sender that no more FC frames shall be sent
 * during the transmission of the segmented message. The sending network layer entity shall send all
 * remaining ConsecutiveFrames without any stop for further FC frames from the receiving network layer
 * entity.
 * 0x01-0xFF BlockSize (BS)
 * This range of BS parameter values shall be used to indicate to the sender the maximum number of
 * ConsecutiveFrames that can be received without an intermediate FC frame from the receiving network
 * entity.
 */
#define FC_DEFAULT_BS	0UL

/* Timeout values */
#define TIMEOUT_SESSION		(500UL) /* Timeout between successfull send and receive */
#define TIMEOUT_FC			(250UL) /* Timeout between FF and FC or Block CF and FC */
#define TIMEOUT_CF			(250UL) /* Timeout between CFs                          */
#define MAX_FCWAIT_FRAME	(10UL)

void delay_1ms(uint16_t ms1);
void delay_100us(uint16_t us100);
static uint32_t systickms(void);
static ERROR_CODE send_fc(struct Message_t* msg);
static ERROR_CODE send_sf(struct Message_t* msg);
static ERROR_CODE send_ff(struct Message_t* msg);
static ERROR_CODE send_cf(struct Message_t* msg);
static ERROR_CODE rcv_sf(struct Message_t* msg);
static ERROR_CODE rcv_ff(struct Message_t* msg);
static ERROR_CODE rcv_cf(struct Message_t* msg);
static ERROR_CODE rcv_fc(struct Message_t* msg);
static void fc_delay(uint8_t STmin);
static ERROR_CODE isotp_send(struct isotp_msg_t *msg);
static ERROR_CODE isotp_receive(struct isotp_msg_t *msg);

/* 
 * function: delay 1ms
 * This functionality needs to be reimplemented on your platform.
 */
void delay_1ms(uint16_t ms1)
{
	uint16_t i, j;
	for(i = 0; i < 1000; i ++)
	{
		for(j = 0; j < 1000; j ++)
		{
			;
		}
	}
}

/* 
 * function: delay 100us
 * This functionality needs to be reimplemented on your platform.
 */
void delay_100us(uint16_t us100)
{
	uint16_t i, j;
	
	for(i = 0; i < 100; i ++)
	{
		for(j = 0; j < 1000; j ++)
		{
			;
		}
	}
}

/* 
 * function: system tick
 * This functionality needs to be reimplemented on your platform.
 */
uint32_t systickms(void)
{
	return 0;
}

ERROR_CODE isotp_init(struct Message_t *msg,
							uint32_t sa,
							uint32_t ta,
							ERROR_CODE *fs_set_cb(struct Message_t* /*msg*/),
							isotp_transfer send, 
							isotp_transfer receive)
{
	ERROR_CODE err = STATUS_NORMAL;

	if(msg == NULL || send == NULL || receive == NULL)
	{
		err = ERR_POINTER_0;
	}
	else
	{
		msg->DL = 0UL;			/* data length */
		msg->tp_state = ISOTP_IDLE;
		msg->SN = ISOTP_DEFAULT_SN;		/* consecutive frame serial number */
		msg->FS = ISOTP_FS_CTS;	/* Flow control status */
		msg->BS = FC_DEFAULT_BS;		/* block size, setting value */
		msg->BS_Counter = FC_DEFAULT_BS;	/* block size, setting value */
		msg->STmin = 0UL;
		msg->rest = 0UL;		/* mutilate frame remaining part */
		msg->fc_wait_frames = 0UL;
		msg->N_Bs = systickms();
		msg->N_Cr = systickms();
		msg->wait_session = 0UL;
		msg->buffer_index = 0UL;
		msg->reply = N_OK;
		msg->isotp.N_TA = ta;
		msg->isotp.N_SA = sa;
		msg->isotp.phy_rx.new_data = FALSE;
		msg->isotp.phy_send = send;
		msg->isotp.phy_receive = receive;
		msg->fs_set_cb = fs_set_cb;
	}

	return err;
}

ERROR_CODE fc_set(struct Message_t *msg, enum ISOTP_FS_e FS, uint8_t BS, uint8_t STmin)
{
	ERROR_CODE err = STATUS_NORMAL;
	
	if(msg == NULL)
	{
		err = ERR_POINTER_0;
	}
	else
	{
		msg->FS = FS;	/* Flow control status */
		msg->BS = BS;		/* block size, setting value */
		msg->BS_Counter = BS;
		msg->STmin = STmin;
	}

	return err;
}

#if 0
void print_buffer(uint32_t id, uint8_t *buffer, uint16_t len)
{
	uint16_t i = 0;

	Serial.print(F("Buffer: "));
	Serial.print(id, HEX); 
	Serial.print(F(" ["));
	Serial.print(len); 
	Serial.print(F("] "));
	for(i = 0; i < len; i++)
	{
		if(buffer[i] < 0x10)
		{
			Serial.print(F("0"));
		}
		Serial.print(buffer[i], HEX);
		Serial.print(F(" "));
	}
	Serial.println();
}
#endif

static ERROR_CODE isotp_send(struct isotp_msg_t *msg)
{
	msg->phy_tx.new_data = TRUE;
	msg->phy_tx.id = msg->N_TA;
	msg->phy_tx.length = 8UL;
	return msg->phy_send(&msg->phy_tx);
}

static ERROR_CODE isotp_receive(struct isotp_msg_t *msg)
{
	ERROR_CODE err = ERR_EMPTY;

	for(;;)
	{
		if(msg->phy_receive(&msg->phy_rx) != STATUS_NORMAL)
		{
			break;
		}
		if(msg->phy_rx.id != msg->N_SA)
		{
			break;
		}
		if(msg->phy_rx.length == 0)
		{
			break;
		}
		if(msg->phy_rx.length > 8UL)
		{
			msg->phy_rx.length = 8UL;
		}
		err = STATUS_NORMAL;
		break;
	}
	
	return err;
}

/*
 * Send a Flow Control Frame
 */
static ERROR_CODE send_fc(struct Message_t *msg)
{
	uint8_t *data = msg->isotp.phy_tx.data;

	memset(data, 0UL, 8UL);
	/* FC message high nibble = 0x3 , low nibble = FC Status */
	data[0] = (N_PCI_FC | msg->FS);
	data[1] = msg->BS;
	/* fix wrong separation time values according spec */
	if ((msg->STmin > 0x7F) && 
		((msg->STmin < 0xF1) || (msg->STmin > 0xF9))) 
	{
		msg->STmin = ISOTP_DEFAULT_STmin;
	}
	data[2] = msg->STmin;

	return isotp_send(&msg->isotp);
}

static ERROR_CODE send_sf(struct Message_t *msg) //Send SF Message
{
	uint8_t *data = msg->isotp.phy_tx.data;

	memset(data, 0UL, 8UL);
	/* SF message high nibble = 0x0 , low nibble = Length */
	data[0] = (N_PCI_SF | msg->DL);
	memcpy(data + 1UL, msg->Buffer + msg->buffer_index, msg->DL);

	return isotp_send(&msg->isotp);
}

/*
 * Send a First Frame
 */
static ERROR_CODE send_ff(struct Message_t *msg) 
{
	uint8_t *data = msg->isotp.phy_tx.data;

	memset(data, 0UL, 8UL);
	msg->buffer_index = 0UL;
	msg->SN = ISOTP_DEFAULT_SN;
	data[0] = N_PCI_FF | ((msg->DL >> 8UL) & 0x0F);
	data[1] = (msg->DL & 0xFF);
	/* Skip 2 Bytes PCI */
	memcpy(data + 2UL, msg->Buffer + msg->buffer_index, 6UL);
	
	/* First Frame has full length */
	return isotp_send(&msg->isotp);
}

/*
 * Send a Consecutive Frame
 */
static ERROR_CODE send_cf(struct Message_t *msg)
{
	uint8_t *data = msg->isotp.phy_tx.data;
	uint16_t len = 7UL;

	memset(data, 0UL, 8UL);
	data[0] = (N_PCI_CF | (msg->SN & 0x0F));
	if(msg->DL > 7UL) 
	{
		len = 7UL;
	}
	else
	{
		len = msg->DL;
	}
	/* Skip 1 Byte PCI */
	memcpy(data + 1, msg->Buffer + msg->buffer_index, len);

	return isotp_send(&msg->isotp);
}

static void fc_delay(uint8_t STmin)
{
	/* SeparationTime minimum (STmin) range: 0 ms ��C 127 ms */
	if(STmin <= 0x7F)
	{
		delay_1ms(STmin);
	}
	else if(STmin <= 0xF0)
	{
		/* This range of values is reserved by this part of ISO 15765 */
	}
	else if(STmin <= 0xF9)
	{
		/* SeparationTime minimum (STmin) range: 100 |��s ��C 900 |��s */
		delay_100us(STmin - 0xF0);
	}
	else
	{
		/* This range of values is reserved by this part of ISO 15765. */
	}
}

/*
 * Receive a Single Frame
 */
static ERROR_CODE rcv_sf(struct Message_t* msg)
{
	/* get the SF_DL from the N_PCI byte */
	msg->DL = msg->isotp.phy_rx.data[0] & 0x0F;
	msg->buffer_index = 0UL;
	/* copy the received data bytes */
	/* Skip PCI, SF uses len bytes */
	memcpy(msg->Buffer + msg->buffer_index, msg->isotp.phy_rx.data + 1UL, msg->DL);
	msg->tp_state = ISOTP_FINISHED;

	return STATUS_NORMAL;
}

/*
 * Receive a First Frame
 */
static ERROR_CODE rcv_ff(struct Message_t* msg)
{
	ERROR_CODE err = STATUS_NORMAL;
	uint8_t *data = msg->isotp.phy_rx.data;

	/* get the FF_DL */
	msg->DL = (data[0] & 0x0F) << 8;
	msg->DL += data[1];
	if(msg->DL <= 6UL)
	{
		err = ERR_PARAMETER;
	}
	else
	{
		msg->SN = ISOTP_DEFAULT_SN;
		msg->rest = msg->DL;
		msg->buffer_index = 0UL;
		/* 
		 * copy the first received data bytes
		 * Skip 2 bytes PCI, FF must have 6 bytes!
		 */
		memcpy(msg->Buffer + msg->buffer_index, data + 2UL, 6UL);
		msg->buffer_index += 6UL;
		msg->rest -= 6UL; /* Rest length */
		msg->tp_state = ISOTP_WAIT_DATA;
		msg->FS = ISOTP_FS_CTS;	/* continue to send */
		msg->STmin = 10UL;		/* SeparationTime minimum (STmin) range: 0 ms �C 127 ms */
		err = send_fc(msg);
	}

	return err;
}

/*
 * Receive a Consecutive Frame
 */
static ERROR_CODE rcv_cf(struct Message_t* msg)
{
	ERROR_CODE err = STATUS_NORMAL;
	uint8_t *data = msg->isotp.phy_rx.data;

	msg->N_Bs = systickms();
	for(;;)
	{
		if(time_after(systickms(), msg->N_Bs + TIMEOUT_FC) && (msg->SN != ISOTP_DEFAULT_SN))
		{
			msg->tp_state = ISOTP_IDLE;
			msg->SN = ISOTP_DEFAULT_SN;
			msg->rest = 0UL;
			msg->buffer_index = 0UL;
			msg->reply = N_TIMEOUT_Bs;
			err = ERR_TIMEOUT;
			break;
		}
		if (msg->tp_state != ISOTP_WAIT_DATA) 
		{
			err = ERR_PARAMETER;
			break;
		}
		if ((data[0] & 0x0F) != (msg->SN & 0x0F))
		{
			msg->tp_state = ISOTP_IDLE;
			msg->SN = ISOTP_DEFAULT_SN;
			err = ERR_PARAMETER;
			break;
		}
		
		if(msg->rest < 7UL)
		{
			/* Last Frame */
			memcpy(msg->Buffer + msg->buffer_index, data + 1UL, msg->rest);	/* 6 Bytes in FF + 7 */
			msg->tp_state = ISOTP_FINISHED;	/* per CF skip PCI */
			msg->rest = 0UL;
		}
		else
		{
			memcpy(msg->Buffer + msg->buffer_index, data + 1UL, 7UL);	/* 6 Bytes in FF + 7 */
			msg->rest -= 7UL; /* Got another 7 Bytes of Data; */
			if(msg->BS != 0UL
				&& (--msg->BS_Counter) == 0UL)
			{
				msg->BS_Counter = msg->BS;
				if(msg->fs_set_cb != NULL)
				{
					msg->fs_set_cb(msg);
				}
				err = send_fc(msg);
			}
		}
		msg->buffer_index += 7UL;
		msg->SN ++;
		

		break;
	}
	
	return err;
}

/*
 * Receive a Flow Control Frame
 */
static ERROR_CODE rcv_fc(struct Message_t* msg)
{
	ERROR_CODE err = STATUS_NORMAL;
	uint8_t *data = msg->isotp.phy_rx.data;

	for(;;)
	{
		if (msg->tp_state != ISOTP_WAIT_FC 
			&& msg->tp_state != ISOTP_WAIT_FIRST_FC)
		{
			err = ERR_PARAMETER;
			break;
		}
		if(N_PCI_FC != data[0] & 0x0F)
		{
			err = ERR_PARAMETER;
			break;
		}
		/* get communication parameters only from the first FC frame */
		if (msg->tp_state == ISOTP_WAIT_FIRST_FC)
		{
			msg->FS = (enum ISOTP_FS_e)(data[0] & 0x0F);
			msg->BS = data[1];
			msg->BS_Counter = msg->BS;
			msg->STmin = data[2];
			/* fix wrong separation time values according spec */
			if ((msg->STmin > 0x7F) 
				&& ((msg->STmin < 0xF1) || (msg->STmin > 0xF9))) 
			{
				msg->STmin = ISOTP_DEFAULT_STmin;
			}
		}
		switch (msg->FS)
		{
			case ISOTP_FS_CTS:
				msg->tp_state = ISOTP_SEND_CF;
				break;
			case ISOTP_FS_WAIT:
				msg->N_Bs = systickms();
				msg->fc_wait_frames ++;
				if(msg->fc_wait_frames >= MAX_FCWAIT_FRAME)
				{
					msg->fc_wait_frames = 0UL;
					msg->tp_state = ISOTP_IDLE;
					err = ERR_TIMEOUT;
				}
				break;
			case ISOTP_FS_OVFLW:
				err = ERR_FULL;
				break;
			default:
				msg->tp_state = ISOTP_IDLE;
				err = ERR_PARAMETER;
				break;
		}
		break;
	}
	
	return err;
}

ERROR_CODE send(struct Message_t* msg)
{
	ERROR_CODE err = STATUS_NORMAL;

	msg->tp_state = ISOTP_SEND;
	while(msg->tp_state != ISOTP_IDLE && msg->tp_state != ISOTP_ERROR)
	{
		switch(msg->tp_state)
		{
			case ISOTP_IDLE:
				break;
			case ISOTP_SEND:
				if(msg->DL <= 7UL)
				{
					err = send_sf(msg);
					msg->tp_state = ISOTP_IDLE;
				}
				else
				{
					err = send_ff(msg);
					if(err == STATUS_NORMAL) // FF complete
					{
						msg->buffer_index += 6UL;
						msg->DL -= 6UL;
						msg->tp_state = ISOTP_WAIT_FIRST_FC;
						msg->fc_wait_frames = 0UL;
						msg->N_Bs = systickms();
					}
				}
				break;
			case ISOTP_WAIT_FIRST_FC:
				if(time_after(systickms(), msg->N_Bs + TIMEOUT_FC))
				{
					msg->tp_state = ISOTP_IDLE;
					err = ERR_TIMEOUT;
				}
				/* break; */
			case ISOTP_WAIT_FC:
				if(isotp_receive(&msg->isotp) == STATUS_NORMAL)
				{
					err = rcv_fc(msg);
				}			
				break;
			case ISOTP_SEND_CF:
				while(msg->tp_state == ISOTP_SEND_CF) 
				{
					fc_delay(msg->STmin);
					err = send_cf(msg);
					if(err == STATUS_NORMAL)
					{
						if(msg->BS > 0UL)
						{
							if((--msg->BS_Counter) == 0UL)
							{
								msg->BS_Counter = msg->BS;
								msg->tp_state = ISOTP_WAIT_FC;
							}
						}
						msg->SN ++;
						if(msg->DL > 7L)
						{
							msg->buffer_index += 7UL;
							msg->DL -= 7UL;
						}
						else
						{
							msg->buffer_index += msg->DL;
							msg->DL = 0UL;
							msg->tp_state = ISOTP_IDLE;
						}
					}
				}
				break;
			default:
				err = ERR_PARAMETER;
				break;
		}
	}

	return err;
}

ERROR_CODE receive(struct Message_t* msg)
{
	enum n_pci_type_e n_pci_type = N_PCI_SF;
	ERROR_CODE err = STATUS_NORMAL;

	msg->wait_session = systickms();
	msg->tp_state = ISOTP_IDLE;
	while(msg->tp_state != ISOTP_FINISHED && msg->tp_state != ISOTP_ERROR)
	{
		if(time_after(systickms(), msg->wait_session + TIMEOUT_SESSION))
		{
			err = ERR_TIMEOUT;
			break;
		}
		if(isotp_receive(&msg->isotp) == STATUS_NORMAL)
		{
			n_pci_type = (enum n_pci_type_e)(msg->isotp.phy_rx.data[0] & 0xF0);
			switch (n_pci_type)
			{
				case N_PCI_FC:
					rcv_fc(msg);/* tx path: fc frame */
					break;
				case N_PCI_SF:
					rcv_sf(msg);/* rx path: single frame */
					break;
				case N_PCI_FF:
					rcv_ff(msg);/* rx path: first frame */
					break;
				case N_PCI_CF:
					rcv_cf(msg);/* rx path: consecutive frame */
					break;
				default:
					err = ERR_PARAMETER;
					break;
			}
		}
	}

	return err;
}
