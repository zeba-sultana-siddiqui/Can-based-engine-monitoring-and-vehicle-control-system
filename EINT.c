#include "EINT.h"
#include "delays.h"

volatile u8 flag_win_up = 0;
volatile u8 flag_win_down = 0;
volatile u8 flag_reverse = 0;

void __irq EINT0_ISR(void)
{
    flag_win_up = 1;
    EXTINT = (1 << 0); /* Clear EINT0 interrupt flag */
    VICVectAddr = 0;   /* Acknowledge Interrupt */
}

void __irq EINT1_ISR(void)
{
    flag_win_down = 1;
    EXTINT = (1 << 1); /* Clear EINT1 interrupt flag */
    VICVectAddr = 0;   /* Acknowledge Interrupt */
}

void __irq EINT2_ISR(void)
{
    flag_reverse = 1;
    EXTINT = (1 << 2); /* Clear EINT2 interrupt flag */
    VICVectAddr = 0;   /* Acknowledge Interrupt */
}

void Switches_Init(void)
{
    /* P0.14 = EINT1 (bits 29:28 = 10), P0.15 = EINT2 (bits 31:30 = 10) */
    PINSEL0 &= ~((u32)0x0F << 28);
    PINSEL0 |=  ((u32)0x0A << 28);
    
    /* P0.16 = EINT0 (bits 1:0 = 01) */
    PINSEL1 &= ~((u32)0x03 << 0);
    PINSEL1 |=  ((u32)0x01 << 0);
    
    /* Configure as Edge-Sensitive (Falling Edge) */
    EXTMODE = (1 << 0) | (1 << 1) | (1 << 2);   /* Edge sensitive */
    EXTPOLAR = 0;                               /* Falling edge */
    
    /* Assign ISR to VIC */
    VICVectAddr0 = (u32)EINT0_ISR;
    VICVectCntl0 = 0x20 | 14; /* EINT0 is Channel 14 */
    
    VICVectAddr1 = (u32)EINT1_ISR;
    VICVectCntl1 = 0x20 | 15; /* EINT1 is Channel 15 */
    
    VICVectAddr2 = (u32)EINT2_ISR;
    VICVectCntl2 = 0x20 | 16; /* EINT2 is Channel 16 */
    
    /* Clear any pending interrupts */
    EXTINT = (1 << 0) | (1 << 1) | (1 << 2);
    
    /* Enable Interrupts in VIC */
    VICIntEnable = (1 << 14) | (1 << 15) | (1 << 16);
}

}
