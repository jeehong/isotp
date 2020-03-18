#include "timer.h"

#define time_after(a,b)         ((long)(b) - (long)(a) <= 0)

#define time_before(a,b)        time_after(b,a)

#define time_interval(now,pre)  ((long)(now) - (long)(pre)) 

static U32 (*gTmr_tickMsFxn)(void);
static U8 gTmrcountType, gTmrtickFactor;

ERROR_CODE timer_init(U32 (*tickMs)(void), U8 countType, U8 tickFactor)
{
    ERROR_CODE retVal = STATUS_NORMAL;

    if (tickMs == NULL)
    {
        retVal = ERR_POINTER_0;
    }
    else
    {
        gTmr_tickMsFxn = tickMs;
        gTmrcountType  = countType;
        gTmrtickFactor = tickFactor;
    }

    return retVal;
}

void timer_add(struct timer_t *timer)
{
    if (gTmr_tickMsFxn != NULL)
    {
        timer->enable = TRUE;
        timer->timeout = FALSE;
        timer_refresh(timer);
    }
    else
    {
        timer->enable = FALSE;
    }
}

void timer_refresh(struct timer_t *timer)
{
    if(timer->enable == TRUE)
    {
        timer->markTime = gTmr_tickMsFxn();
    }
}

void timer_xdelete(struct timer_t * timer)
{
    timer->enable = FALSE;
}

Bool timer_overflow(struct timer_t * timer, U32 period_ms)
{
    if(timer->enable)
    {
        if (gTmrcountType == TIMER_COUNT_UP
            && time_after(gTmr_tickMsFxn(), timer->markTime + period_ms * gTmrtickFactor))
        {
            timer->timeout = TRUE;
        }
        else if (gTmrcountType == TIMER_COUNT_DOWN
            && time_before(gTmr_tickMsFxn(), timer->markTime - period_ms * gTmrtickFactor))
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

U32 timer_interval(struct timer_t * timer)
{
    U32 interVal = 0u;

    if (gTmrcountType == TIMER_COUNT_UP)
    {
        interVal = (S32)gTmr_tickMsFxn() - (S32)timer->markTime;
    }
    else if (gTmrcountType == TIMER_COUNT_DOWN)
    {
        interVal = (S32)timer->markTime - (S32)gTmr_tickMsFxn();
    }
    else
    {
        interVal = 0u;
    }

    interVal /= gTmrtickFactor;

    return interVal;
}

Bool timer_is_added(struct timer_t  *timer)
{
    return timer->enable;
}

