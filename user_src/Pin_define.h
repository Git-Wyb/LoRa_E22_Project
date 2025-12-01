#ifndef __PIN_DEFINE_H__
#define __PIN_DEFINE_H__

#include "Timer.h"
//以下是IO定义
/********************LED寄存器*****************************************/

#define LED_ON  1
#define LED_OFF 0

#define Receiver_LED_TX PB_ODR_ODR2       //PC_ODR_ODR1       // Output   受信机送信指示  高电平有效
#define Receiver_LED_TX_direc PB_DDR_DDR2 // Output   受信机送信指示  高电平有效
#define Receiver_LED_TX_CR1 PB_CR1_C12    // Output   受信机送信指示  高电平有效
#define Receiver_LED_TX_CR2 PB_CR2_C22    //输出频率

#define Receiver_LED_RX PB_ODR_ODR3       //PC_ODR_ODR0       // Output   受信机受信指示  高电平有效
#define Receiver_LED_RX_direc PB_DDR_DDR3 // Output   受信机受信指示  高电平有效
#define Receiver_LED_RX_CR1 PB_CR1_C13    // Output   受信机受信指示  高电平有效
#define Receiver_LED_RX_CR2 PB_CR2_C23    //输出频率

#define Receiver_LED_OUT PB_ODR_ODR4       //PB_ODR_ODR1              // Output   受信机继电器动作输出  高电平有效
#define Receiver_LED_OUT_direc PB_DDR_DDR4 // Output   受信机继电器动作输出  高电平有效
#define Receiver_LED_OUT_CR1 PB_CR1_C14    // Output   受信机继电器动作输出  高电平有效
#define Receiver_LED_OUT_CR2 PB_CR2_C21    //输出频率

/******************以下是KEY寄存器*******输入*****************************/
#define KEY_Empty 0
#define KEY_SW11_Down 1
#define KEY_SW12_Down 2
#define KEY_SW13_Down 3
#define KEY_SW14_Down 4
#define KEY_SW15_Down 5
#define KEY_SW16_Down 6

#define KEY_UP_IN     PE_IDR_IDR0
#define KEY_UP_DDR    PE_DDR_DDR0
#define KEY_UP_CR1    PE_CR1_C10
#define KEY_UP_CR2    PE_CR2_C20

#define KEY_DOWN_IN     PE_IDR_IDR1
#define KEY_DOWN_DDR    PE_DDR_DDR1
#define KEY_DOWN_CR1    PE_CR1_C11
#define KEY_DOWN_CR2    PE_CR2_C21

#define KEY_ENTER_IN     PE_IDR_IDR2
#define KEY_ENTER_DDR    PE_DDR_DDR2
#define KEY_ENTER_CR1    PE_CR1_C12
#define KEY_ENTER_CR2    PE_CR2_C22

#define KEY_OPEN_IN     PD_IDR_IDR0
#define KEY_OPEN_DDR    PD_DDR_DDR0
#define KEY_OPEN_CR1    PD_CR1_C10
#define KEY_OPEN_CR2    PD_CR2_C20

#define KEY_STOP_IN     PD_IDR_IDR1
#define KEY_STOP_DDR    PD_DDR_DDR1
#define KEY_STOP_CR1    PD_CR1_C11
#define KEY_STOP_CR2    PD_CR2_C21

#define KEY_CLOSE_IN     PD_IDR_IDR2
#define KEY_CLOSE_DDR    PD_DDR_DDR2
#define KEY_CLOSE_CR1    PD_CR1_C12
#define KEY_CLOSE_CR2    PD_CR2_C22

#define PIN_TestIN      PD_IDR_IDR3   //TEST PIN
#define PIN_TEST_DDR    PD_DDR_DDR3
#define PIN_TEST_CR1    PD_CR1_C13
#define PIN_TEST_CR2    PD_CR2_C23

#define SelDev_ModeIN   PD_IDR_IDR4
#define SelDev_Mode_DDR PD_DDR_DDR4
#define SelDev_Mode_CR1 PD_CR1_C14
#define SelDev_Mode_CR2 PD_CR2_C24

/******************以下是data寄存器************************************/
#define PIN_BEEP        PB_ODR_ODR1       //PA_ODR_ODR0       // Output   蜂鸣器
#define PIN_BEEP_DDR    PB_DDR_DDR1 // Output   蜂鸣器
#define PIN_BEEP_CR1    PB_CR1_C11    // Output   蜂鸣器

#define Receiver_OUT_OPEN       PB_ODR_ODR7
#define Receiver_OUT_OPEN_DDR   PB_DDR_DDR7
#define Receiver_OUT_OPEN_CR1   PB_CR1_C17

#define Receiver_OUT_STOP       PB_ODR_ODR6
#define Receiver_OUT_STOP_DDR   PB_DDR_DDR6
#define Receiver_OUT_STOP_CR1   PB_CR1_C16

#define Receiver_OUT_CLOSE       PB_ODR_ODR5
#define Receiver_OUT_CLOSE_DDR   PB_DDR_DDR5
#define Receiver_OUT_CLOSE_CR1   PB_CR1_C15

#define E22_M0         PC_ODR_ODR5
#define E22_M0_DDR     PC_DDR_DDR5
#define E22_M0_CR1     PC_CR1_C15
#define E22_M0_CR2     PC_CR2_C25

#define E22_M1         PC_ODR_ODR4
#define E22_M1_DDR     PC_DDR_DDR4
#define E22_M1_CR1     PC_CR1_C14
#define E22_M1_CR2     PC_CR2_C24

#define E22_AUX_IN     PD_IDR_IDR7
#define E22_AUX_DDR    PD_DDR_DDR7
#define E22_AUX_CR1    PD_CR1_C17
#define E22_AUX_CR2    PD_CR2_C27

#define E22_RST        PD_ODR_ODR6
#define E22_RST_DDR    PD_DDR_DDR6
#define E22_RST_CR1    PD_CR1_C16
#define E22_RST_CR2    PD_CR2_C26

/************************LCD GPIO*******************************/
#define	PIN_LCD_SCLK	    PA_ODR_ODR5			// 输出；LCD串行时钟(√)
#define	PIN_LCD_RST	        PA_ODR_ODR3			// 输出；LCD复位脚  低电平有效(√)
#define	PIN_LCD_SDATA		PA_ODR_ODR6			// 输出；LCD串行数据(√)
#define	PIN_LCD_A0_RS		PA_ODR_ODR4			// 输出；LCD命令数据选择端  0--命令   1--数据(√)
#define	PIN_LCD_SEL		    PA_ODR_ODR2			// 输出；芯片片选   低电平有效(√)
#define	PIN_LCD_LED		    PA_ODR_ODR7

#define PIN_LCD_LED_direc      PA_DDR_DDR7
#define PIN_LCD_SDATA_direc    PA_DDR_DDR6
#define PIN_LCD_SEL_direc      PA_DDR_DDR2
#define PIN_LCD_RST_direc      PA_DDR_DDR3
#define PIN_LCD_SCLK_direc     PA_DDR_DDR5
#define PIN_LCD_A0_RS_direc    PA_DDR_DDR4

#define PIN_LCD_LED_CR1       PA_CR1_C17
#define PIN_LCD_SDATA_CR1     PA_CR1_C16
#define PIN_LCD_SEL_CR1       PA_CR1_C12
#define PIN_LCD_RST_CR1       PA_CR1_C13
#define PIN_LCD_SCLK_CR1      PA_CR1_C15
#define PIN_LCD_A0_RS_CR1     PA_CR1_C14


/*************************IIC RTC***************************/
#define PIN_PCF8563_SDAOUT      PC_ODR_ODR0
#define PIN_PCF8563_SDAIN       PC_IDR_IDR0
#define PIN_PCF8563_SCL         PC_ODR_ODR1

#define PIN_PCF8563_SDA_direc   PC_DDR_DDR0
#define PIN_PCF8563_SCL_direc   PC_DDR_DDR1
#define PIN_PCF8563_SDA_CR1     PC_CR1_C10
#define PIN_PCF8563_SCL_CR1     PC_CR1_C11

#define PCF8563_RSTI_DDR        PE_DDR_DDR6
#define PCF8563_RSTI_CR1        PE_CR1_C16
#define PCF8563_RSTI_CR2        PE_CR2_C26
#define PCF8563_RSTI            PE_ODR_ODR6
/****************************************************/
#define Receiver_Login 0

#endif

