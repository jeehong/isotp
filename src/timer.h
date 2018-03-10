#ifndef __TIMER_H__
#define __TIMER_H__

#include "comm_typedef.h"

typedef struct timer_t
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

void timer_add(struct timer_t *timer);
void timer_delete(struct timer_t *timer);
Bool timer_overflow(struct timer_t * timer, uint32_t period_ms);
Bool timer_is_added(struct timer_t  *timer);
void timer_refresh(struct timer_t * timer);


#endif

