#include "display.h"
#include <stdio.h>
#include <string.h>
#include "E22-900T.h"

u8 m_x = 0;
u8 m_y = 0;
u8 start_x = 7;
u8 ycoe = 8;
u8 host_buf[] = {"[LoRa Host Mode]"};
u8 relay_buf[] = {"[LoRa Relay Mode]"};
Slave_Str sla_stu = {0};
Slave_Str SlaNumStu[6];
u8 charbuf[25];
char *strbuf[] = {"OPEN ","STOP ","CLOSE"};
char *sfreq[] = {"F:922.2","F:922.4","F:922.6","F:922.8","F:923.0","F:923.2"};
//u8 str_sta[] = {"No State RSSI MS "};
u8 str_sta[] = {"No RX_PACKAGE MS "};
/*
display_freq(0,8,922200000);
FreqToString(sbuff,429.175,"MHz");
Display_String(0,40,"=>",2);
*/

void menu_start(void)
{
    if(mode_sel.Mode_Set == NORMAL_MODE)
    {
        //Display_String(0,0,host_buf,strlen(host_buf));
        Display_String(0,m_y,str_sta,strlen(str_sta));
        Display_String(start_x,m_y+1*ycoe,"1",1);
        Display_String(start_x,m_y+2*ycoe,"2",1);
        Display_String(start_x,m_y+3*ycoe,"3",1);
        Display_String(start_x,m_y+4*ycoe,"4",1);
        Display_String(start_x,m_y+5*ycoe,"5",1);
        //Display_String(start_x,m_y+6*ycoe,"Check All",9);
    }
    else Display_String(0,0,relay_buf,strlen(relay_buf));
    /*
    SlaNumStu[5].acktime = 1234;
    SlaNumStu[5].rssi = 114;
    SlaNumStu[5].sta = 0x02;

    SlaNumStu[1].rssi = 12;
    SlaNumStu[1].acktime = 99;

    Display_refresh(28,10,1);
    Display_refresh(28,10,2);
    Display_refresh(28,10,3);
    Display_refresh(28,10,4);
    Display_refresh(28,10,5);*/
}

void Display_refresh(u8 x,u8 y,u8 snum)
{
    u8 n = 1;
    if(SlaNumStu[snum].sta == 0x08) n = 0;
    else if(SlaNumStu[snum].sta == 0x04) n = 1;
    else if(SlaNumStu[snum].sta == 0x02) n = 2;
    /*
    memset(charbuf,0,sizeof(charbuf));
    snprintf(charbuf,20,"%s 0%d ",strbuf[n],(0));//256-SlaNumStu[snum].slave_rssi
    Display_String(x,snum*ycoe+y,"               ",15);
    Display_String(x,snum*ycoe+y,charbuf,strlen(charbuf));
    */
    Display_String(x,snum*ycoe+y,"               ",15);

    memset(charbuf,0,sizeof(charbuf));
    snprintf(charbuf,20,"%ld",SlaNumStu[snum].acktime);
    Display_String(x+11*7,snum*ycoe+y,charbuf,strlen(charbuf));

    memset(charbuf,0,sizeof(charbuf));
    if(SlaNumStu[snum].rssi == 0) snprintf(charbuf,20,"%ddBm",(0));
    else snprintf(charbuf,20,"-%ddBm",(256-SlaNumStu[snum].rssi));
    Display_String(start_x,7*ycoe+y,"        ",8);
    Display_String(start_x,7*ycoe+y,charbuf,strlen(charbuf));

    memset(charbuf,0,sizeof(charbuf));
    snprintf(charbuf,20,"%d/100",slave_rx_packnum);//slave_rx_packnum
    //Display_String(start_x+7*8,7*ycoe+y,"         ",9);
    //Display_String(start_x+7*8,7*ycoe+y,charbuf,strlen(charbuf));
    Display_String(x,snum*ycoe+y,charbuf,strlen(charbuf));
}

void display_restart(void)
{
    Display_String(start_x+7*8,7*ycoe+m_y,"0/100   ",8);
}

void check_key_sta(void)
{
    switch(key_sta)
    {
        case Key_Up:
            if(key_step>1) key_step--;
            else key_step = 5;      //no check_all
            mode_sel.enter_step = key_step;
            flag_key_enter = 0;
            flag_step = 0;
            key_sta = Key_None;
            flag_tx_start = 0;
            break;

        case Key_Down:
            if(key_step<5) key_step++;
            else key_step = 1;
            mode_sel.enter_step = key_step;
            flag_key_enter = 0;
            flag_step = 0;
            key_sta = Key_None;
            flag_tx_start = 0;
            break;

        case Key_Enter:
            flag_key_enter = 1;
            flag_tx_start = 1;
            mode_sel.enter_step = key_step;
            key_sta = Key_None;
            slave_rx_packnum = 0;
            break;

        case Key_Open:
            //if(flag_key_enter)
            {
                e22_txdata[5] = 0x08;
                //flag_tx_start = 1;
                flag_tx_open = 1;
                flag_tx_stop = 0;
                flag_tx_close = 0;
            }
            key_sta = Key_None;
            break;

        case Key_Stop:
            //if(flag_key_enter)
            {
                e22_txdata[5] = 0x04;
                //flag_tx_start = 1;
                flag_tx_stop = 1;
                flag_tx_close = 0;
                flag_tx_open = 0;
            }
            key_sta = Key_None;
            break;

        case Key_Close:
            //if(flag_key_enter)
            {
                e22_txdata[5] = 0x02;
                //flag_tx_start = 1;
                flag_tx_close = 1;
                flag_tx_stop = 0;
                flag_tx_open = 0;
            }
            key_sta = Key_None;
            break;

        case Key_None:
            if(key_last_step != key_step)
            {
                key_last_step = key_step;
                //display_step(key_step);
                clear_step();
                Display_String(0,key_step*ycoe+m_y,"*",1);
            }
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
        Display_String(0,key_step*ycoe+m_y,"*",2);
        clear_data(step);
    }
    else if(flag_step == 0)
    {
        if(time_step == 0)
        {
            time_step = 600;
            if(dis_num == 0)
            {
                dis_num = 1;
                clear_step();
            }
            else
            {
                dis_num = 0;
                clear_step();
                Display_String(0,key_step*ycoe+m_y,"=>",2);
            }
        }
    }
}

void clear_step(void)
{
    u8 i = 0;
    for(i=1; i<7; i++)
        Display_String(0,i*ycoe+m_y," ",1);
}

void clear_data(u8 step)
{
    u8 i = 0;
    if(step == 6)
    {
        for(i=1; i<6; i++)
            Display_String(m_x+28,i*ycoe+m_y,"               ",15);
    }
    else
        Display_String(m_x+28,step*ycoe+m_y,"               ",15);
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
