
#include "bedSleep.h"

#define wdt_interrupt_enable(value)   \
    __asm__ __volatile__ (  \
                            "in __tmp_reg__,__SREG__" "\n\t"    \
                            "cli" "\n\t"    \
                            "wdr" "\n\t"    \
                            "sts %0,%1" "\n\t"  \
                            "out __SREG__,__tmp_reg__" "\n\t"   \
                            "sts %0,%2" "\n\t" \
                            : /* no outputs */  \
                            : "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
                            "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
                            "r" ((uint8_t) ((value & 0x08 ? _WD_PS3_MASK : 0x00) | \
                                            _BV(WDIE) | (value & 0x07)) ) \
                            : "r0"  \
                         )

SIGNAL(WDT_vect)
{
    wdt_disable();
    wdt_reset();
    WDTCSR &= ~_BV(WDIE);
}

void deepSleep(const uint8_t wdtTime)
{
    uint8_t adcsra_save;
    uint8_t prr_save;
    
    //MCUSR = 0;  /* reset various flags*/
    adcsra_save = ADCSRA;
    prr_save = PRR;
    
    //ADCSRA = 0;
    
    //PRR = 0b11101111; /* turn off various modules */
    
    //wdt_enable(wdtTime);
    //WDTCSR |= bit(WDIE);
    wdt_interrupt_enable(wdtTime);
    
    wdt_reset();
    
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    /* sleep mode is set here */
    sleep_enable();
    
    /* turn off brown-out enable in software */
    //MCUCR = bit (BODS) | bit (BODSE);
    //MCUCR = bit (BODS);
    sei();
    sleep_cpu();
    
    ADCSRA = adcsra_save; /* stop power reduction */
    PRR = prr_save;
}

void longDeepSleep(uint32_t delayTime)
{
    uint32_t remTime = delayTime;
    uint32_t nbCycle;
    
    uint32_t cycleTime = 0b10000 << WDTO_8S;
    uint8_t wdtTime = WDTO_8S;
    do
    {
        if (remTime > cycleTime)
        {
            nbCycle = remTime / cycleTime;
            remTime %= (nbCycle * cycleTime);
            for (; nbCycle > 0 ; nbCycle --)
            {
                deepSleep(wdtTime);
            }
        }
        cycleTime >>= 1;
    }
    while (wdtTime--);
}



