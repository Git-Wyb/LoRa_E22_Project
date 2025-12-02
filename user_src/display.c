#include "display.h"
#include <stdio.h>
#include <string.h>
#include "E22-900T.h"

u8 m_x = 0;
u8 m_y = 10;
u8 key_step = 1;
u8 host_buf[] = {"[LoRa Host Mode]"};
u8 relay_buf[] = {"[LoRa Relay Mode]"};
u8 e22_txdata[] = {0x11,0x12,0x13,0x14,0x15};
Slave_Str sla_stu = {0};
Slave_Str SlaNumStu[6];
u8 charbuf[25];
char *strbuf[] = {"OPEN ","STOP ","CLOSE"};
char *sfreq[] = {"F:922.2","F:922.4","F:922.6","F:922.8","F:923.0","F:923.2"};
u8 str_sta[] = {"No. State RSSI Ms "};
/*
display_freq(0,8,922200000);
FreqToString(sbuff,429.175,"MHz");
Display_String(0,40,"=>",2);
*/

void menu_start(void)
{
    Display_String(0,0,host_buf,strlen(host_buf));
    Display_String(0,10,str_sta,strlen(str_sta));
    Display_String(m_x+14,m_y+10,"1",1);
    Display_String(m_x+14,m_y+20,"2",1);
    Display_String(m_x+14,m_y+27,"3",1);
    Display_String(m_x+14,m_y+37,"4",1);
    Display_String(m_x+14,m_y+42,"5",1);
    Display_String(m_x+14,m_y+50,"Check All",9);

    SlaNumStu[5].acktime = 500;
    SlaNumStu[5].rssi = 68;
    SlaNumStu[5].sta = 0x02;
    Display_refresh(28,10,5);
    Display_String(0,m_y+10,"=>",2);
}

void Display_refresh(u8 x,u8 y,u8 snum)
{
    u8 n = 1;
    if(SlaNumStu[snum].sta == 0x08) n = 0;
    else if(SlaNumStu[snum].sta == 0x04) n = 1;
    else if(SlaNumStu[snum].sta == 0x02) n = 2;

    if(sla_stu.num == 0)
    {
        snprintf(charbuf,20,"%s -%d %ld",strbuf[n],SlaNumStu[snum].rssi,SlaNumStu[snum].acktime);
        Display_String(m_x+x,m_y+y,charbuf,strlen(charbuf));

        snprintf(charbuf,20,"%s -%d %ld",strbuf[n],SlaNumStu[snum].rssi,SlaNumStu[snum].acktime);
        Display_String(m_x+x,m_y+y+10,charbuf,strlen(charbuf));

        snprintf(charbuf,20,"%s -%d %ld",strbuf[n],SlaNumStu[snum].rssi,SlaNumStu[snum].acktime);
        Display_String(m_x+x,m_y+y+17,charbuf,strlen(charbuf));

        snprintf(charbuf,20,"%s -%d %ld",strbuf[n],SlaNumStu[snum].rssi,SlaNumStu[snum].acktime);
        Display_String(m_x+x,m_y+y+27,charbuf,strlen(charbuf));

        snprintf(charbuf,20,"%s -%d %ld",strbuf[n],SlaNumStu[snum].rssi,SlaNumStu[snum].acktime);
        Display_String(m_x+x,m_y+y+32,charbuf,strlen(charbuf));
    }
}

void check_key_sta(void)
{
    switch(key_sta)
    {
        case Key_Up:
            if(key_step>1) key_step--;
            else key_step = 6;
            flag_key_enter = 0;
            flag_step = 0;
            key_sta = Key_None;
            break;

        case Key_Down:
            if(key_step<6) key_step++;
            else key_step = 1;
            flag_key_enter = 0;
            flag_step = 0;
            key_sta = Key_None;
            break;

        case Key_Enter:
            flag_key_enter = 1;
            e22_txdata[2] = 0xF0 | sla_stu.num;
            e22_hal_uart_tx(e22_txdata,5);
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            key_sta = Key_None;
            break;

        case Key_Open:
            e22_txdata[2] = 0x08;
            e22_hal_uart_tx(e22_txdata,5);
            key_sta = Key_None;
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            break;

        case Key_Stop:
            e22_txdata[2] = 0x04;
            e22_hal_uart_tx(e22_txdata,5);
            key_sta = Key_None;
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            break;

        case Key_Close:
            e22_txdata[2] = 0x02;
            e22_hal_uart_tx(e22_txdata,5);
            key_sta = Key_None;
            Receiver_LED_TX = 1;
            time_sw = 300;
            flag_led = 1;
            break;

        case Key_None:
            display_step(key_step);
            break;
    }
}

u8 dis_num = 0;
void display_step(u8 step)
{
    if(flag_key_enter && flag_step == 0)
    {
        flag_step = 1;
        clear_step();
        Display_String(0,key_step*8+10,"=>",2);
        clear_data(step);
    }
    else if(flag_step == 0)
    {
        if(time_step == 0)
        {
            time_step = 500;
            if(dis_num == 0)
            {
                dis_num = 1;
                clear_step();
            }
            else
            {
                dis_num = 0;
                clear_step();
                Display_String(0,key_step*8+10,"=>",2);
            }
        }
    }
}

void clear_step(void)
{
    u8 i = 0;
    for(i=1; i<7; i++)
        Display_String(0,i*8+10,"  ",2);
}

void clear_data(u8 step)
{
    u8 i = 0;
    if(step == 6)
    {
        for(i=1; i<6; i++)
            Display_String(m_x+28,i*8+10,"              ",14);
    }
    else
        Display_String(m_x+28,step*8+10,"              ",14);
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
