#include "isotp.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "comm_typedef.h"

#define CLIENT_ADDRESS	0x766
#define SERVER_ADDRESS	0x706

static ERROR_CODE sender_test_send(struct phy_msg_t *msg);
static ERROR_CODE sender_test_receive(struct phy_msg_t *msg);
static ERROR_CODE receiver_test_send(struct phy_msg_t *msg);
static ERROR_CODE receiver_test_receive(struct phy_msg_t *msg);


static struct Message_t tx, rx;

void *rx_thread(void *arg)
{
	receive(&rx);
}

void main(void)
{
	uint16_t index;
	pthread_t rc_task;

	isotp_init(&tx, CLIENT_ADDRESS, SERVER_ADDRESS, sender_test_send, sender_test_receive);
	isotp_init(&rx, SERVER_ADDRESS, CLIENT_ADDRESS, receiver_test_send, receiver_test_receive);

	pthread_create(&rc_task, NULL, rx_thread, NULL);

	tx.DL = 256UL;
	for(index = 0; index < tx.DL; index ++)
	{
		tx.Buffer[index] = index;
	}

	send(&tx);
}

static ERROR_CODE sender_test_send(struct phy_msg_t *msg)
{
	static uint32_t seq = 0UL;

	seq ++;
	printf("Sender-Tx Seq:%d Len:%d Id:0x%04X Data:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
				seq,
				msg->length,
				msg->id,
				msg->data[0], msg->data[1], msg->data[2],
				msg->data[3], msg->data[4], msg->data[5],
				msg->data[6], msg->data[7]);
	memcpy(&rx.isotp.phy, &tx.isotp.phy, sizeof(tx.isotp.phy));
	rx.isotp.phy.new_data = TRUE;
	
	return STATUS_NORMAL;
}

static ERROR_CODE sender_test_receive(struct phy_msg_t *msg)
{
	static uint32_t seq = 0UL;
	ERROR_CODE err = ERR_EMPTY;

	if(tx.isotp.phy.new_data == TRUE)
	{
		tx.isotp.phy.new_data = FALSE;
		err = STATUS_NORMAL;
		seq ++;
		printf("Sender-Rx Seq:%d Len:%d Id:0x%04X Data:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
					seq,
					msg->length,
					msg->id,
					msg->data[0], msg->data[1], msg->data[2],
					msg->data[3], msg->data[4], msg->data[5],
					msg->data[6], msg->data[7]);
	}
	
	return err;
}

static ERROR_CODE receiver_test_send(struct phy_msg_t *msg)
{
	static uint32_t seq = 0UL;

	seq ++;
	
	printf("Receiver-Tx Seq:%d Len:%d Id:0x%04X Data:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
				seq,
				msg->length,
				msg->id,
				msg->data[0], msg->data[1], msg->data[2],
				msg->data[3], msg->data[4], msg->data[5],
				msg->data[6], msg->data[7]);
	memcpy(&tx.isotp.phy, &rx.isotp.phy, sizeof(rx.isotp.phy));
	tx.isotp.phy.new_data = TRUE;

	return STATUS_NORMAL;
}

static ERROR_CODE receiver_test_receive(struct phy_msg_t *msg)
{
	static uint32_t seq = 0UL;
	ERROR_CODE err = ERR_EMPTY;

	if(rx.isotp.phy.new_data == TRUE)
	{
		rx.isotp.phy.new_data = FALSE;
		err = STATUS_NORMAL;
		seq ++;
		printf("Receiver-Rx Seq:%d Len:%d Id:0x%04X Data:%02X %02X %02X %02X %02X %02X %02X %02X\r\n",
					seq,
					msg->length,
					msg->id,
					msg->data[0], msg->data[1], msg->data[2],
					msg->data[3], msg->data[4], msg->data[5],
					msg->data[6], msg->data[7]);
	}
	return err;
}

