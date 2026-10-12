#include "CAN.h"

/*
 * LPC2129 On-Chip CAN1 Driver
 * Configured for 250 kbps Baud Rate (Automotive Standard)
 */

void CAN1_Init(void)
{
    /* Configure P0.25 as RD1 (CAN1 Rx) and P0.24 as TD1 (CAN1 Tx) */
    PINSEL1 &= ~((3 << 16) | (3 << 18));
    PINSEL1 |=  ((1 << 16) | (1 << 18));
    
    /* Set CAN1 into Reset/Initialization Mode */
    C1MOD = 0x01;
    
    /* Bypass Acceptance Filter (Accept all CAN messages) */
    AFMR = 0x02;
    
    /* Configure Bus Timing for 125 kbps @ 60 MHz VPBDIV / CCLK */
    C1BTR = 0x001C001D;
    
    /* Return CAN1 to Normal Operating Mode */
    C1MOD = 0x00;
}

u8 CAN1_Tx(CAN_MSG *msg)
{
    /* Check if Transmit Buffer 1 is available */
    if(C1SR & (1 << 2))
    {
        /* Set Frame Information: DLC & Priority */
        C1TFI1 = ((u32)msg->dlc << 16) | (msg->rtr ? (1 << 30) : 0);
        
        /* Set Standard 11-bit Identifier */
        C1TID1 = msg->id;
        
        /* Set Data Bytes */
        C1TDA1 = ((u32)msg->data[0]) |
                 ((u32)msg->data[1] << 8) |
                 ((u32)msg->data[2] << 16) |
                 ((u32)msg->data[3] << 24);
                 
        C1TDB1 = ((u32)msg->data[4]) |
                 ((u32)msg->data[5] << 8) |
                 ((u32)msg->data[6] << 16) |
                 ((u32)msg->data[7] << 24);
        
        /* Initiate Transmission from Buffer 1 */
        C1CMR = (1 << 0) | (1 << 5);
        return 1; /* Success */
    }
    return 0; /* Buffer busy */
}

u8 CAN1_Rx(CAN_MSG *msg)
{
    /* Check if Receive Buffer contains a valid message */
    if(C1GSR & 0x01)
    {
        msg->dlc = (u8)((C1RFS >> 16) & 0x0F);
        msg->rtr = (u8)((C1RFS >> 30) & 0x01);
        msg->id  = C1RID;
        
        msg->data[0] = (u8)(C1RDA);
        msg->data[1] = (u8)(C1RDA >> 8);
        msg->data[2] = (u8)(C1RDA >> 16);
        msg->data[3] = (u8)(C1RDA >> 24);
        
        msg->data[4] = (u8)(C1RDB);
        msg->data[5] = (u8)(C1RDB >> 8);
        msg->data[6] = (u8)(C1RDB >> 16);
        msg->data[7] = (u8)(C1RDB >> 24);
        
        /* Release Receive Buffer */
        C1CMR = (1 << 2);
        return 1; /* Message received */
    }
    return 0; /* No message */
}
