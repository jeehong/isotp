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



/* N_PCI type values in bits 7-4 of N_PCI bytes */
enum n_pci_type_e
{
	N_PCI_SF = 0x00,  /* single frame */
	N_PCI_FF = 0x10,  /* first frame */
	N_PCI_CF = 0x20,  /* consecutive frame */
	N_PCI_FC = 0x30,  /* flow control */
};

#define FC_CONTENT_SZ 	3 /* flow control content size in byte (FS/BS/STmin) */

#define FC_DEFAULT_BS	100UL

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

/* Timeout values */
#define TIMEOUT_SESSION		(500UL) /* Timeout between successfull send and receive */
#define TIMEOUT_FC			(250UL) /* Timeout between FF and FC or Block CF and FC */
#define TIMEOUT_CF			(250UL) /* Timeout between CFs                          */
#define MAX_FCWAIT_FRAME	(10UL)

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
	struct phy_msg_t phy;
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
	uint8_t BS;			/* block size, setting value */
	uint8_t STmin;		/* SeparationTime minimum */

	uint16_t rest;		/* mutilate frame remaining part */
	uint8_t  fc_wait_frames;
	uint32_t wait_fc;
	uint32_t wait_cf;
	uint32_t wait_session;
	uint8_t Buffer[ISOTP_FF_DL];	/* data pool */
	uint16_t buffer_index;			/* data_pool current index */
	struct isotp_msg_t isotp;	/* isotp data from the bus */
};

ERROR_CODE isotp_init(struct Message_t *msg, uint32_t sa, uint32_t ta, isotp_transfer send, isotp_transfer receive);
ERROR_CODE send(struct Message_t* msg);
ERROR_CODE receive(struct Message_t* msg);
void print_buffer(uint32_t id, uint8_t *buffer, uint16_t len);

#endif
