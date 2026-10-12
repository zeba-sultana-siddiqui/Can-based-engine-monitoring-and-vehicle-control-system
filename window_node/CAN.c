#include "CAN.h"

void CAN1_Init(void)
{
    PINSEL1 &= ~((3 << 16) | (3 << 18));
    PINSEL1 |=  ((1 << 16) | (1 << 18));
    
    C1MOD = 0x01;
    AFMR = 0x02; /* Acceptance filter bypass */
    C1BTR = 0x001C001D; /* 125 kbps */
    C1MOD = 0x00;
}

u8 CAN1_Tx(CAN_MSG *msg)
{
    if(C1SR & (1 << 2))
    {
        C1TFI1 = ((u32)msg->dlc << 16) | (msg->rtr ? (1 << 30) : 0);
        C1TID1 = msg->id;
        
        C1TDA1 = ((u32)msg->data[0]) |
                 ((u32)msg->data[1] << 8) |
                 ((u32)msg->data[2] << 16) |
                 ((u32)msg->data[3] << 24);
                 
        C1TDB1 = ((u32)msg->data[4]) |
                 ((u32)msg->data[5] << 8) |
                 ((u32)msg->data[6] << 16) |
                 ((u32)msg->data[7] << 24);
        
        C1CMR = (1 << 0) | (1 << 5);
        return 1;
    }
    return 0;
}

u8 CAN1_Rx(CAN_MSG *msg)
{
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
        
        C1CMR = (1 << 2);
        return 1;
    }
    return 0;
}
