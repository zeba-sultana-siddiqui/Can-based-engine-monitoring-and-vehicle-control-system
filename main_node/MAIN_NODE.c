#include <lpc21xx.h>
#include "types.h"
#include "delays.h"
#include "LCD.h"
#include "DS18B20.h"
#include "CAN.h"
#include "dashboard.h"
#include "EINT.h"
#include "project_functions.h"

int main(void)
{
    f32 engine_temp = 32.5f;
    f32 read_val;
    u8 window_current_level = 0; /* 0 (Fully Open) to 8 (Fully Closed) */
    u8 ecu_fault_flag = 0;       /* 0 = All ECUs Healthy, 1 = ECU Offline */
    u8 reverse_mode_active = 0;
    u8 last_valid_distance = 100;
    u8 last_valid_zone = ZONE_SAFE;
    u16 reverse_watchdog = 0;
    CAN_MSG rx_msg;
    
    /* 1. Initialize LCD Display (P0.0 - P0.10) */
    LCD_init();
    
    /* 2. Show Modern 20x4 Splash Screen */
    display_splash_screen();
    
    /* 3. Initialize Switch Inputs (P0.14, P0.15, P0.16) */
    Switches_Init();
    
    /* 4. Initialize CAN1 Peripheral (250 kbps on P0.24, P0.25) */
    CAN1_Init();
    
    /* 5. Main Execution Loop */
    while(1)
    {
        /* Check Window UP Switch (P0.16) */
        if(flag_win_up)
        {
            window_current_level = Process_Window_Control(WINDOW_CMD_UP, window_current_level, &ecu_fault_flag);
            flag_win_up = 0; /* Clear flag AFTER processing to ignore switch bounce */
        }
        
        /* Check Window DOWN Switch (P0.14) */
        if(flag_win_down)
        {
            window_current_level = Process_Window_Control(WINDOW_CMD_DOWN, window_current_level, &ecu_fault_flag);
            flag_win_down = 0; /* Clear flag AFTER processing to ignore switch bounce */
        }
        
        /* Check Reverse Gear Switch (P0.15) */
        if(flag_reverse)
        {
            Process_Reverse_Event(&reverse_mode_active);
            if(reverse_mode_active)
            {
                reverse_watchdog = 0; // reset watchdog on entry
            }
            flag_reverse = 0; /* Clear flag AFTER processing to ignore switch bounce */
        }
        
        /* Non-blocking CAN reception for reverse telemetry (if active) */
        if(reverse_mode_active)
        {
            if(CAN1_Rx(&rx_msg))
            {
                if(rx_msg.id == CAN_ID_DISTANCE_DATA)
                {
                    last_valid_distance = rx_msg.data[0];
                    last_valid_zone = rx_msg.data[1];
                    reverse_watchdog = 0;
                    ecu_fault_flag = 0;
                }
            }
            else
            {
                reverse_watchdog++;
                if(reverse_watchdog >= 16)
                {
                    ecu_fault_flag = 1;
                    display_node_error_screen(NODE_TYPE_REVERSE);
                    Process_Reverse_Event(&reverse_mode_active); /* Toggles 1 -> 0 and sends DISABLE */
                }
            }
        }
        
        /* Sample Engine Temperature from DS18B20 Sensor (P0.22) */
        read_val = DS18B20_ReadTemperature();
        if(read_val > -50.0f && read_val < 150.0f)
        {
            engine_temp = read_val;
        }
        else
        {
            engine_temp = -999.0f; /* Sensor disconnected indicator */
        }
        
        /* Render Real-Time Display */
        if(reverse_mode_active)
        {
            display_reverse_radar(last_valid_distance, last_valid_zone);
        }
        else
        {
            display_dashboard(engine_temp, window_current_level, REVERSE_DISABLE, ecu_fault_flag);
        }
        
        delay_ms(50);
    }
    
    return 0;
}
