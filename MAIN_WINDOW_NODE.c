#include <lpc21xx.h>
#include "types.h"
#include "delays.h"
#include "CAN.h"
#include "window_control.h"

int main(void)
{
    CAN_MSG rx_msg;
    
    /* 1. Initialize Window Motor GPIOs */
    Window_Control_Init();
    
    /* 2. Initialize CAN1 Peripheral (250 kbps) */
    CAN1_Init();
    
    /* 3. Main Listener Loop */
    while(1)
    {
        /* Check if CAN Frame Received */
        if(CAN1_Rx(&rx_msg))
        {
            if(rx_msg.id == CAN_ID_WINDOW_CTRL)
            {
                u8 cmd = rx_msg.data[0];
                Window_Execute_Command(cmd);
            }
        }
        delay_ms(10);
    }
    
    return 0;
}
