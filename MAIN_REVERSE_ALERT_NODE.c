#include <lpc21xx.h>
#include "types.h"
#include "delays.h"
#include "CAN.h"
#include "distance_sensor.h"

int main(void)
{
    CAN_MSG rx_msg, tx_msg;
    u8 reverse_active = 0;
    u8 distance_cm;
    u8 zone;
    
    /* 1. Initialize GP2D12 ADC Interface */
    Distance_Sensor_Init();
    
    /* 2. Initialize CAN1 Peripheral (250 kbps) */
    CAN1_Init();
    
    /* 3. Main Loop */
    while(1)
    {
        /* Check for CAN Commands from Main Node */
        if(CAN1_Rx(&rx_msg))
        {
            if(rx_msg.id == CAN_ID_REVERSE_ENABLE)
            {
                reverse_active = rx_msg.data[0];
            }
        }
        
        /* If Reverse Mode is Enabled, stream live distance data */
        if(reverse_active)
        {
            distance_cm = Distance_Calculate_cm();
            zone = Distance_Determine_Zone(distance_cm);
            
            /* Prepare and send Distance CAN Frame */
            tx_msg.id = CAN_ID_DISTANCE_DATA;
            tx_msg.dlc = 2;
            tx_msg.rtr = 0;
            tx_msg.data[0] = distance_cm;
            tx_msg.data[1] = zone;
            
            CAN1_Tx(&tx_msg);
            
            delay_ms(100); /* 10 Hz refresh rate */
        }
        else
        {
            delay_ms(20);
        }
    }
    
    return 0;
}
