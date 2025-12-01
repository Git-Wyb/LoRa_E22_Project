#ifndef __LCD_H__
#define __LCD_H__

#include <iostm8l151c8.h> // CPU型号
#include "Pin_define.h"   // 管脚定义
#include "initial.h"      // 初始匿  预定乿
#include "ram.h"          // RAM定义
#include "Timer.h"

void lcd_GPIO_init(void);
void lcd_init(void);
void display_map_xy(unsigned char x,unsigned char y,unsigned char l,unsigned char h,const unsigned char *p);
void lcd_clear_1and2_line(unsigned char data1);
void lcd_clear_3and8_line(unsigned char data1);
void lcd_clear_line1(unsigned char data1);
void lcd_clear_line8(unsigned char data1);
void lcd_clear(unsigned char data1);
void lcd_GPIO_init(void);

extern const unsigned char char_Small[];
extern const unsigned char char_Medium[];
extern const unsigned char char_RSSI[];
extern const unsigned char char_rssi[];
extern const unsigned char char_Contro[];

extern const unsigned char char_Small[];
extern const unsigned char char_Medium[];
extern const unsigned char char_Contro[];
extern const unsigned char char_RSSI[];
extern const unsigned char char_rssi[];


#endif
