#include "timer.h"

#define time_after(a,b)  \
((long)(b) - (long)(a) < 0)

#define time_before(a,b) time_after(b,a)

#define time_interval(now,pre) ((long)(now) - (long)(pre)) 

/* 
 * function: system tick
 * This functionality needs to be reimplemented on your platform.
 */
static uint32_t systickms(void)
{
	return 0;
}


void delay_1ms(uint16_t ms1)
{
	/*uint16_t i, j;
	for(i = 0UL; i < 1000UL; i ++)
	{
		for(j = 0UL; j < 1000UL; j ++)
		{
			;
		}
	}*/
}

void delay_100us(uint16_t us100)
{
	/*uint16_t i, j;
	
	for(i = 0UL; i < 100UL; i ++)
	{
		for(j = 0UL; j < 1000UL; j ++)
		{
			;
		}
	}*/
}

void timer_add(struct timer_t *timer)
{
	timer->enable = TRUE;
	timer->timeout = FALSE;
	timer->curtime = systickms();
}


void timer_refresh(struct timer_t *timer)
{
	if(timer->enable == TRUE)
	{
		timer->curtime = systickms();
	}
}

void timer_delete(struct timer_t * timer)
{
    timer->enable = FALSE;
}

Bool timer_overflow(struct timer_t * timer, uint32_t period_ms)
{
    if(timer->enable)
    {
        if(time_after(systickms(), timer->curtime + period_ms))
        {
            timer->timeout = TRUE;
        }
        else
        {
            timer->timeout = FALSE;
        }
    }
    else
    {
        timer->timeout = FALSE;
    }

    return timer->timeout;
}

Bool timer_is_added(struct timer_t  *timer)
{
    return timer->enable;
}

