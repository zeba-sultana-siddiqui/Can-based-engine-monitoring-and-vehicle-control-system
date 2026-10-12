#ifndef __CAN_H__
#define __CAN_H__

#include <lpc21xx.h>
#include "types.h"
#include "CAN_defines.h"

/* CAN Message Structure */
typedef struct {
    u32 id;       /* 11-bit standard or 29-bit extended identifier */
    u8  dlc;      /* Data length code (0 to 8 bytes) */
    u8  rtr;      /* Remote Transmission Request (0 = Data, 1 = RTR) */
    u8  data[8];  /* Data payload */
} CAN_MSG;

/* Function Prototypes */
void CAN1_Init(void);
u8   CAN1_Tx(CAN_MSG *msg);
u8   CAN1_Rx(CAN_MSG *msg);

#endif /* __CAN_H__ */
