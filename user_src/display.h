#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "lcd.h"
#include "uart.h"

typedef struct{
    u8 s_num;
    u8 s_sta;
}Slave_Str;

void display_freq(u8 x,u8 y,u32 freq);
void Display_String(u8 x,u8 y,u8 *str,u8 len);
void FreqToString(char *buff,double freq,char *str);
void menu_start(void);
void clear_arr(void);
void check_key_sta(void);
void menu_creat(u8 *name,u8 len);

extern Slave_Str sla_stu;
extern u8 e22_txdata[];

#endif
