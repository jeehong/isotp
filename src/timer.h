#ifndef __TIMER_H__
#define __TIMER_H__

#include "comm_typedef.h"

/* s32k lpit timer count in down mode */
/* win32 system timer count in up mode */
#define TIMER_COUNT_UP      (0u)
#define TIMER_COUNT_DOWN    (1u)


struct timer_t
{
    Bool enable;
    Bool timeout;
    U32  markTime;
};

/*
 * @Function: initialize timer module
 * @Parameter: 
 *  tickUs: tick function, millisecond(0x0-0xFFFFFFFF)
 *  should modify TIMER_FACTOR_MS to match microsecond.
 *  e.g. tickUs outputs 1 tick = 1/8ms, so TIMER_FACTOR_MS = 8u
 * @Return: ERROR_CODE
 *      STATUS_NORMAL tickUs is valid
 *      ERR_POINTER_0 tickUs is not found
 */
ERROR_CODE timer_init(U32 (*tickUs)(void), U8 countType, U8 tickFactor);

/*
 * @Function: enable a timer and record current system tick
 * @Parameter: 
 *	timer: timer object
 * @Return: NULL
 */
void timer_add(struct timer_t *timer);

/*
 * @Function: disenable a timer
 * @Parameter: 
 *	timer: timer object
 * @Return: NULL
 */
void timer_xdelete(struct timer_t *timer);

/*
 * @Function: check if the timer is out of time
 * @Parameter: 
 *	timer: timer object
 * @Return: 
 *	TRUE: the timer is out of time
 *	FLASE: the time is not out of time
 */
Bool timer_overflow(struct timer_t * timer, U32 period_ms);

/*
 * @Function: check if the timer is enabled
 * @Parameter: 
 *	timer: timer object
 * @Return: 
 *	TRUE: the timer is enabled
 *	FLASE: the time is not enabled
 */
Bool timer_is_added(struct timer_t  *timer);

/*
 * @Function: refresh timer
 * @Parameter: 
 *  timer: timer object
 * @Return: NULL
 */
void timer_refresh(struct timer_t * timer);

/*
 * @Function: get interval for timer
 * @Parameter: 
 *  timer: timer object
 * @Return: interval, unit:MS
 */
U32 timer_interval(struct timer_t * timer);


#endif

