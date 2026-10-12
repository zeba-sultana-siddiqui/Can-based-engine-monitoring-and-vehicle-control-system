#include "project_functions.h"
#include "dashboard.h"
#include "delays.h"
#include "EINT.h"

u8 Process_Window_Control(u8 action, u8 current_level, u8 *fault_flag)
{
    CAN_MSG tx_msg, rx_msg;
    u16 timeout;
    u8 response_received;
    
    tx_msg.id = CAN_ID_WINDOW_CTRL;
    tx_msg.dlc = 1;
    tx_msg.rtr = 0;
    tx_msg.data[0] = action;
    
    /* 1. Boundary Checks */
    if(action == WINDOW_CMD_UP && current_level >= 8)
    {
        display_window_screen(WINDOW_CMD_UP, 8);
        delay_ms(400);
        LCD_clear();
        return 8;
    }
    if(action == WINDOW_CMD_DOWN && current_level == 0)
    {
        display_window_screen(WINDOW_CMD_DOWN, 0);
        delay_ms(400);
        LCD_clear();
        return 0;
    }
    
    /* 2. Transmit Window Command Frame (0x101) */
    CAN1_Tx(&tx_msg);
    
    /* 3. Wait for Node Response (0x201) with Timeout (500 ms max) */
    response_received = 0;
    for(timeout = 0; timeout < 50; timeout++)
    {
        if(CAN1_Rx(&rx_msg))
        {
            if(rx_msg.id == CAN_ID_WINDOW_STATUS)
            {
                current_level = rx_msg.data[0];
                response_received = 1;
                *fault_flag = 0; /* Clear ECU fault */
                break;
            }
        }
        delay_ms(10);
    }
    
    /* 4. Handle Node Timeout / Disconnection Error */
    if(!response_received)
    {
        *fault_flag = 1; /* Set ECU fault */
        display_node_error_screen(NODE_TYPE_WINDOW);
        return current_level;
    }
    
    /* Display updated position */
    display_window_screen(action, current_level);
    delay_ms(200);
    
    /* 5. Return updated level (Continuous Press Removed) */
    delay_ms(300);
    LCD_clear();
    return current_level;
}

void Process_Reverse_Event(u8 *reverse_mode_active)
{
    CAN_MSG tx_msg;
    
    tx_msg.id = CAN_ID_REVERSE_ENABLE;
    tx_msg.dlc = 1;
    tx_msg.rtr = 0;
    
    if(*reverse_mode_active == 0)
    {
        *reverse_mode_active = 1;
        tx_msg.data[0] = REVERSE_ENABLE;
    }
    else
    {
        *reverse_mode_active = 0;
        tx_msg.data[0] = REVERSE_DISABLE;
    }
    
    CAN1_Tx(&tx_msg);
    LCD_clear();
}
