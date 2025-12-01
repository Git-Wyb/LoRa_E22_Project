#include "display.h"
#include <stdio.h>
#include <string.h>
#include "E22-900T.h"

u8 m_x = 15;
u8 m_y = 0;
u8 key_step = 0;
u8 menu_num = 0;
u8 sy = 10;
u8 host_buf[] = {"[LoRa Host Mode]"};
u8 relay_buf[] = {"[LoRa Relay Mode]"};
u8 e22_txdata[] = {0x11,0x12,0x13,0x14,0x15};

/*
display_freq(0,8,922200000);
FreqToString(sbuff,429.175,"MHz");
Display_String(0,40,"=>",2);
*/

void menu_start(void)
{
    Display_String(0,0,host_buf,strlen(host_buf));
    Display_String(0,sy,"=>",2);
    menu_creat("Freq:922.2MHz",13);
    menu_creat("Slave:1",6);
    //menu_creat("Rx Mode",7);
    //menu_creat("Reset",5);
}

void menu_creat(u8 *name,u8 len)
{
    u8 buff[20];
    menu_num++;
    snprintf(buff,20,"%d.%s",menu_num,name);
    m_y = m_y+8+2;
    Display_String(m_x,m_y,buff,strlen(buff));
}

void check_key_sta(void)
{
    switch(key_sta)
    {
        case Key_Up:
            if(key_step) key_step--;
            else key_step = menu_num-1;
            clear_arr();
            Display_String(0,key_step*10+sy,"=>",2);
            key_sta = Key_None;
            break;

        case Key_Down:
            if(key_step<menu_num-1) key_step++;
            else key_step = 0;
            clear_arr();
            Display_String(0,key_step*10+sy,"=>",2);
            key_sta = Key_None;
            break;

        case Key_Enter:

            key_sta = Key_None;
            break;

        case Key_Open:
            e22_txdata[2] = Key_Open;
            e22_hal_uart_tx(e22_txdata,5);
            key_sta = Key_None;
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            break;

        case Key_Stop:
            e22_txdata[2] = Key_Stop;
            e22_hal_uart_tx(e22_txdata,5);
            key_sta = Key_None;
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            break;

        case Key_Close:
            e22_txdata[2] = Key_Close;
            e22_hal_uart_tx(e22_txdata,5);
            key_sta = Key_None;
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            break;
    }
}

void clear_arr(void)
{
    u8 i = 0;
    for(i=0; i<menu_num; i++)
        Display_String(0,i*10+sy,"  ",2);
}

void Display_String(u8 x,u8 y,u8 *str,u8 len)
{
    u8 i = 0;
    if(len > 20) len = 20;
    for(i=0; i<len; i++)
        display_map_xy(x+i*7,y,5,8,char_Small+(*(str+i) - 32)*5);
}

void FreqToString(char *buff,double freq,char *str)
{
    snprintf(buff,20,"%.3f%s",freq,str);
}

void display_freq(u8 x,u8 y,u32 freq)
{
    u32 num = 0;
    UINT8 i,data;
    num = freq / 1000;

    display_map_xy(x,y,5,8,char_Small+38*5); //F
    display_map_xy(x+7,y,5,8,char_Small+82*5); //r
    display_map_xy(x+14,y,5,8,char_Small+69*5); //e
    display_map_xy(x+21,y,5,8,char_Small+81*5); //q
    display_map_xy(x+28,y,5,8,char_Small+26*5); //:

    for(i=0;i<6;i++)
    {
        data = num % 10;
        num = num / 10;
        if(i < 3)
            display_map_xy(x+28+(7-i)*6,y,5,8,char_Small+(hex_asc(data)-0x20)*5);
        else
            display_map_xy(x+28+(6-i)*6,y,5,8,char_Small+(hex_asc(data)-0x20)*5);
    }
    display_map_xy(x+28+4*6,y,5,8,char_Small+('.'-0x20)*5);

    display_map_xy(x+28+7*6+7,y,5,8,char_Small+45*5); //M
    display_map_xy(x+28+7*6+14,y,5,8,char_Small+40*5); //H
    display_map_xy(x+28+7*6+21,y,5,8,char_Small+90*5); //z
}
