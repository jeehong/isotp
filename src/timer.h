#ifndef __TIMER_H__
#define __TIMER_H__

#include "comm_typedef.h"

struct timer_t
{
    Bool enable;
    Bool timeout;
    uint32_t curtime;
};

/* 
 * function: delay 1ms
 * This functionality needs to be reimplemented on your platform.
 */
void delay_1ms(uint16_t ms1);

/* 
 * function: delay 100us
 * This functionality needs to be reimplemented on your platform.
 */
void delay_100us(uint16_t us100);

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
void xtimer_delete(struct timer_t *timer);

/*
 * @Function: check if the timer is out of time
 * @Parameter: 
 *	timer: timer object
 * @Return: 
 *	TRUE: the timer is out of time
 *	FLASE: the time is not out of time
 */
Bool timer_overflow(struct timer_t * timer, uint32_t period_ms);

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


#endif

