#include "dashboard.h"
#include "delays.h"
#include "CAN_defines.h"

void display_splash_screen(void)
{
    LCD_clear();
    LCD_set_cursor(1, 0);
    LCD_string("====================");
    LCD_set_cursor(2, 0);
    LCD_string("  SMART VEHICLE ECU ");
    LCD_set_cursor(3, 0);
    LCD_string(" ENGINE & SAFETY OS ");
    LCD_set_cursor(4, 0);
    LCD_string("====================");
    delay_ms(2000);
    LCD_clear();
}

void display_dashboard(f32 temp, u8 window_level, u8 reverse_state, u8 ecu_fault_flag)
{
    static u8 critical_state = 0;
    u8 pct;
    
    /* 1. Sensor Disconnection Check */
    if(temp < -50.0f || temp > 150.0f)
    {
        LCD_set_cursor(1, 0);
        LCD_string("--- VEHICLE DASH ---");
        
        LCD_set_cursor(2, 0);
        LCD_string("TEMP  : SENSOR ERR  ");
        
        LCD_set_cursor(3, 0);
        LCD_string("STATUS: CHECK 1-WIRE");
        
        LCD_set_cursor(4, 0);
        if(window_level >= 8)      LCD_string("WIN:CLOSED  ");
        else if(window_level == 0) LCD_string("WIN:OPENED  ");
        else
        {
            pct = (u8)((window_level * 100) / 8);
            LCD_string("WIN: ");
            LCD_integer(pct);
            LCD_string("%    ");
            if(pct < 10) LCD_string(" ");
        }
        
        if(reverse_state == REVERSE_ENABLE) LCD_string("REV: ON ");
        else                                LCD_string("REV: OFF");
        return;
    }

    /* 2. Critical Temperature Overheat Alarm with Hysteresis */
    if(temp >= TEMP_THRESHOLD_CRITICAL)
    {
        critical_state = 1;
    }
    else if(temp <= (TEMP_THRESHOLD_CRITICAL - TEMP_HYSTERESIS))
    {
        critical_state = 0;
    }

    if(critical_state)
    {
        LCD_set_cursor(1, 0);
        LCD_string("********************");
        
        LCD_set_cursor(2, 0);
        LCD_string("! CRITICAL OVERHEAT!");
        
        LCD_set_cursor(3, 0);
        LCD_string("TEMP  : ");
        LCD_float(temp, 1);
        LCD_data(CHAR_DEGREE);
        LCD_string("C[STOP]");
        
        LCD_set_cursor(4, 0);
        LCD_string("STOP VEHICLE SAFELY!");
        return;
    }

    /* 3. Normal Operating Dashboard */
    LCD_set_cursor(1, 0);
    LCD_string("--- VEHICLE DASH ---");
    
    /* Line 2: Temperature Value & Status Tag */
    LCD_set_cursor(2, 0);
    LCD_string("TEMP  : ");
    LCD_float(temp, 1);
    LCD_data(CHAR_DEGREE);
    LCD_string("C ");
    
    if(temp >= TEMP_THRESHOLD_HIGH)
    {
        LCD_string("[HIGH]");
    }
    else if(temp >= TEMP_THRESHOLD_WARMING)
    {
        LCD_string("[WARM]");
    }
    else
    {
        LCD_string("[ OK ]");
    }
    
    /* Line 3: System Health & ECU Status */
    LCD_set_cursor(3, 0);
    if(ecu_fault_flag != 0)
    {
        LCD_string("! WARN: ECU OFFLINE ");
    }
    else if(temp >= TEMP_THRESHOLD_HIGH)
    {
        LCD_string("! WARN: ENGINE HOT !");
    }
    else if(temp >= TEMP_THRESHOLD_WARMING)
    {
        LCD_string("STATUS: FAN ACTIVE  ");
    }
    else
    {
        LCD_string("STATUS: ALL ECUS OK ");
    }
    
    /* Line 4: Live Window Percentage & Reverse State */
    LCD_set_cursor(4, 0);
    if(window_level >= 8)
    {
        LCD_string("WIN:CLOSED  ");
    }
    else if(window_level == 0)
    {
        LCD_string("WIN:OPENED  ");
    }
    else
    {
        pct = (u8)((window_level * 100) / 8);
        LCD_string("WIN: ");
        LCD_integer(pct);
        LCD_string("%    ");
        if(pct < 10) LCD_string(" ");
    }
    
    if(reverse_state == REVERSE_ENABLE)
    {
        LCD_string("REV: ON ");
    }
    else
    {
        LCD_string("REV: OFF");
    }
}

void display_window_screen(u8 action, u8 level)
{
    u8 i;
    u8 pct;
    
    if(level > 8) level = 8;
    pct = (u8)((level * 100) / 8);
    
    LCD_set_cursor(1, 0);
    LCD_string("=== POWER WINDOW ===");
    
    /* Line 2: Motion State or Boundary Limit */
    LCD_set_cursor(2, 0);
    if(action == WINDOW_CMD_UP)
    {
        if(level >= 8)
        {
            LCD_string("LIMIT : FULLY CLOSED");
        }
        else
        {
            LCD_string("MOTION: ROLLING UP ");
            LCD_data(CHAR_UP_ARROW);
        }
    }
    else /* WINDOW_CMD_DOWN */
    {
        if(level == 0)
        {
            LCD_string("LIMIT : FULLY OPENED");
        }
        else
        {
            LCD_string("MOTION: ROLLING DN ");
            LCD_data(CHAR_DOWN_ARROW);
        }
    }
    
    /* Line 3: 8-Segment Bar Graph */
    LCD_set_cursor(3, 0);
    if(level == 8)
    {
        LCD_string("GLASS:[||||||||]100%");
    }
    else
    {
        LCD_string("GLASS: [");
        for(i = 0; i < 8; i++)
        {
            if(i < level) LCD_data('|');
            else          LCD_data('.');
        }
        LCD_data(']');
        if(pct < 10) LCD_data(' ');
        LCD_integer(pct);
        LCD_data('%');
    }
    
    /* Line 4: Status */
    LCD_set_cursor(4, 0);
    if(level >= 8)      LCD_string("STATUS: FULLY CLOSED");
    else if(level == 0) LCD_string("STATUS: FULLY OPENED");
    else
    {
        LCD_string("STATUS: LEVEL ");
        LCD_integer(level);
        LCD_string(" OF 8 ");
    }
}

void display_reverse_radar(u8 distance_cm, u8 zone)
{
    static u8 last_dist = 255;
    static u8 last_zone = 255;
    
    if(distance_cm == last_dist && zone == last_zone)
    {
        return;
    }
    last_dist = distance_cm;
    last_zone = zone;
    
    if(zone == ZONE_CRITICAL || distance_cm <= 15)
    {
        LCD_set_cursor(1, 0);
        LCD_string("********************");
        LCD_set_cursor(2, 0);
        LCD_string("<<  REVERSE RADAR >>");
        LCD_set_cursor(3, 0);
        LCD_string("! STOP: ");
        if(distance_cm < 10) LCD_string(" ");
        LCD_integer(distance_cm);
        LCD_string(" cm DIST !");
        LCD_set_cursor(4, 0);
        LCD_string("********************");
    }
    else
    {
        LCD_set_cursor(1, 0);
        LCD_string("<<  REVERSE RADAR >>");
        
        LCD_set_cursor(2, 0);
        LCD_string("DIST  : ");
        if(distance_cm < 100) LCD_string(" ");
        if(distance_cm < 10)  LCD_string(" ");
        LCD_integer(distance_cm);
        LCD_string(" cm ");
        
        if(zone == ZONE_SAFE)
        {
            LCD_string("[SAFE]");
            LCD_set_cursor(3, 0);
            LCD_string("ZONE  : GREEN / OK  ");
            LCD_set_cursor(4, 0);
            LCD_string("BAR   :[||||||||||||]");
        }
        else if(zone == ZONE_WARNING)
        {
            LCD_string("[WARN]");
            LCD_set_cursor(3, 0);
            LCD_string("ZONE  : YELLOW/SLOW ");
            LCD_set_cursor(4, 0);
            LCD_string("BAR   :[||||||......]");
        }
        else /* ZONE_DANGER */
        {
            LCD_string("[DANG]");
            LCD_set_cursor(3, 0);
            LCD_string("ZONE  : ORANGE/ALERT");
            LCD_set_cursor(4, 0);
            LCD_string("BAR   :[|||.........]");
        }
    }
}

void display_node_error_screen(u8 node_type)
{
    LCD_clear();
    
    LCD_set_cursor(1, 0);
    LCD_string("! CAN BUS WARNING ! ");
    
    LCD_set_cursor(2, 0);
    if(node_type == NODE_TYPE_WINDOW)
    {
        LCD_string("NODE: WINDOW MODULE ");
    }
    else if(node_type == NODE_TYPE_REVERSE)
    {
        LCD_string("NODE: REVERSE RADAR ");
    }
    else
    {
        LCD_string("NODE: UNKNOWN ECU   ");
    }
    
    LCD_set_cursor(3, 0);
    LCD_string("ERROR: NO RESPONSE  ");
    
    LCD_set_cursor(4, 0);
    LCD_string("STATUS: OFFLINE /ERR");
    
    delay_ms(1800); /* Display error screen for 1.8 seconds */
    LCD_clear();
}
