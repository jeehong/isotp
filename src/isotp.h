#ifndef __ISOTP_H__
#define __ISOTP_H__

#include "comm_typedef.h"
#include "timer.h"


typedef enum 
{
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

/**
 * ISO-15765-2-8.5.3.2
 * FirstFrame DataLength (FF_DL)
 * The encoding of the segmented message length results in a twelve bit length value (FF_DL) where the
 * least significant bit (LSB) is specified to be bit 0 of the N_PCI byte #2 and the most significant bit (MSB)
 * is bit 3 of the N_PCI byte #1. The maximum segmented message length supported is equal to 4 095
 * bytes of user data. It shall be assigned the value of the service parameter <Length>.
 */
#define ISOTP_FF_DL     (4095UL)

#define FRAME_DATA_LEN  (8UL)

/* Flow Status given in FC frame */
enum ISOTP_FS_e
{
    ISOTP_FS_CTS   = 0UL,   /* clear to isotp_send */
    ISOTP_FS_WAIT  = 1UL,   /* wait */
    ISOTP_FS_OVFLW = 2UL,   /* overflow */
};

/**
 * Description:
 * This parameter contains the status relating to the outcome of a service execution. 
 * If two or more errors are discovered at the same time,
 * then the network layer entity shall use the parameter value found first
 * in this list when indicating the error to the higher layers.
 */
enum N_Result
{
    N_OK = 0,
    N_TIMEOUT_Ax,
    N_TIMEOUT_Bx,
    N_TIMEOUT_Cx,
    N_WRONG_SN,
    N_INVALID_FS,
    N_UNEXP_PDU,
    N_WFT_OVRN,
    N_BUFFER_OVFLW,
    N_ERROR
};

struct phy_msg_t
{
    U8  new_data;
    U32 id;
    U32 length;
    U8  data[FRAME_DATA_LEN];
};

typedef ERROR_CODE (*isotp_transfer)(struct phy_msg_t *);

struct isotp_msg_t
{
    struct phy_msg_t phy_rx;
    struct phy_msg_t phy_tx;
    U32              N_TA;       /* network target address */
    U32              N_SA;       /* network source address */
    isotp_transfer   phy_send;
    isotp_transfer   phy_receive;
};

struct isotp_t
{
    U16             DL; /* data length */
    isotp_states_t  tp_state;
    U16             SN; /* consecutive frame serial number */
    enum ISOTP_FS_e FS; /* Flow control status */
    U8 BS;              /* setting block size, setting value */
    U8 BS_Counter;      /* block size counter, setting value */
    U8 STmin;           /* SeparationTime minimum */
    ERROR_CODE (*fs_set_cb)(struct isotp_t* /*msg*/);
    U16 rest;           /* mutilate frame remaining part */
    struct timer_t N_Ax;/* x: s/r */
    struct timer_t N_Bx;/* x: s/r */
    struct timer_t N_Cx;/* x: s/r */
    enum N_Result  reply;
    U8   Buffer[ISOTP_FF_DL];   /* data pool */
    U16  buffer_index;          /* data_pool current index */
    struct isotp_msg_t isotp;   /* isotp data from the bus */
};

ERROR_CODE isotp_init(struct isotp_t *msg,
                            U32 sa,
                            U32 ta,
                            ERROR_CODE (*fs_set_cb)(struct isotp_t* /*msg*/),
                            isotp_transfer isotp_send,
                            isotp_transfer isotp_receive);
enum N_Result isotp_send(struct isotp_t* msg);
enum N_Result isotp_receive(struct isotp_t* msg, U32 tmoutUs);
ERROR_CODE fc_set(struct isotp_t *msg, enum ISOTP_FS_e FS, U8 BS, U8 STmin);

#endif

