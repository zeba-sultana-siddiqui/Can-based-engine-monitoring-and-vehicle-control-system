#ifndef __CAN_H__
#define __CAN_H__

#include <lpc21xx.h>
#include "types.h"
#include "CAN_Defines.h"

typedef struct {
    u32 id;
    u8  dlc;
    u8  rtr;
    u8  data[8];
} CAN_MSG;

void CAN1_Init(void);
u8   CAN1_Tx(CAN_MSG *msg);
u8   CAN1_Rx(CAN_MSG *msg);

#endif /* __CAN_H__ */
