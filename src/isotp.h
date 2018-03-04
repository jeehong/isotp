#ifndef __ISOTP_H__
#define __ISOTP_H__

#include <comm_typedef.h>

//#define ISO_TP_DEBUG

typedef enum {
	ISOTP_IDLE = 0,
	ISOTP_SEND,
	ISOTP_SEND_FF,
	ISOTP_SEND_CF,
	ISOTP_WAIT_FIRST_FC,
	ISOTP_WAIT_FC,
	ISOTP_WAIT_DATA,
	ISOTP_FINISHED,
	ISOTP_ERROR
} isotp_states_t;

/*
 * ISO-15765-2-8.5.3.2
 * FirstFrame DataLength (FF_DL)
 * The encoding of the segmented message length results in a twelve bit length value (FF_DL) where the
 * least significant bit (LSB) is specified to be bit 0 of the N_PCI byte #2 and the most significant bit (MSB)
 * is bit 3 of the N_PCI byte #1. The maximum segmented message length supported is equal to 4 095
 * bytes of user data. It shall be assigned the value of the service parameter <Length>.
 */
#define ISOTP_FF_DL	(4095UL)

/* Flow Status given in FC frame */
enum ISOTP_FS_e
{
	ISOTP_FS_CTS = 0UL,		/* clear to send */
	ISOTP_FS_WAIT = 1UL,	/* wait */
	ISOTP_FS_OVFLW = 2UL,	/* overflow */
};

struct phy_msg_t
{
	uint8_t new_data;
	uint32_t id;
	uint8_t length;
	uint8_t data[8];
};

typedef ERROR_CODE (*isotp_transfer)(struct phy_msg_t *);

struct isotp_msg_t
{
	struct phy_msg_t phy_rx;
	struct phy_msg_t phy_tx;
	uint32_t N_TA;		/* network target address */
	uint32_t N_SA;		/* network source address */
	isotp_transfer phy_send;
	isotp_transfer phy_receive;
};

typedef struct Message_t
{
	uint16_t DL;		/* data length */
	isotp_states_t tp_state;
	
	uint16_t SN;		/* consecutive frame serial number */

	enum ISOTP_FS_e FS;	/* Flow control status */
	uint8_t BS;			/* setting block size, setting value */
	uint8_t BS_Counter;	/* block size counter, setting value */
	uint8_t STmin;		/* SeparationTime minimum */
	ERROR_CODE (*fs_set_cb)(struct Message_t* /*msg*/);
	uint16_t rest;		/* mutilate frame remaining part */
	uint8_t  fc_wait_frames;
	uint32_t wait_fc;
	uint32_t wait_cf;
	uint32_t wait_session;
	uint8_t Buffer[ISOTP_FF_DL];	/* data pool */
	uint16_t buffer_index;			/* data_pool current index */
	struct isotp_msg_t isotp;	/* isotp data from the bus */
};

ERROR_CODE isotp_init(struct Message_t *msg,
							uint32_t sa,
							uint32_t ta,
							ERROR_CODE *fs_set_cb(struct Message_t* /*msg*/),
							isotp_transfer send, 
							isotp_transfer receive);
ERROR_CODE send(struct Message_t* msg);
ERROR_CODE receive(struct Message_t* msg);
ERROR_CODE fc_set(struct Message_t *msg, enum ISOTP_FS_e FS, uint8_t BS, uint8_t STmin);

#endif
