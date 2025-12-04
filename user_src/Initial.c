/***********************************************************************/
/*  FILE        :initial.c                                             */
/*  DATE        :Mar, 2013                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <stdio.h>
#include <iostm8l151c8.h>
#include "Pin_define.h" // 管脚定义
#include "initial.h"    // 初始�? 预定�?
#include "ram.h"        // RAM定义
#include "uart.h" // uart
#include "Timer.h"
#include "lcd.h"
#include "E22-900T.h"
#include <string.h>
#include "display.h"

uFLAG YellowLedFlag, RedLedFalg;

void RAM_clean(void)
{ // 清除RAM
    //  asm("ldw X,#0");
    //  asm("clear_ram1.l");
    //  asm("clr (X)");
    //  asm("incw X");
    //  asm("cpw X,#0x6ff");
    //  asm("jrule clear_ram1");
}
void WDT_init(void)
{
    IWDG_KR = 0xCC;
    IWDG_KR = 0x55;
    IWDG_PR = 5;      //约862ms触发看门狗复位
    IWDG_KR = 0xAA;
}
void ClearWDT(void)
{
    IWDG_KR = 0xAA;
}

void VHF_GPIO_INIT(void) // CPU端口设置
{
    KEY_GPIO_Init();
    Receiver_OUT_GPIO_Init();
    LED_GPIO_Init();
    E22_GPIO_Init();
    BEEP_GPIO_Init();
    lcd_GPIO_init();
}
//============================================================================================
void SysClock_Init(void)
{
    CLK_ICKCR_HSION = 1; // 使能内部RC OSC�?6.00MHz�?
    while ((CLK_ICKCR & 0x02) == 0)
        ;              // �?查内部晶�?
    CLK_SWR = 0x01;    // 指定HSI为主时钟
                       //	while(( CLK_SWCR & 0x08 ) == 0 );		// 等待HSI切换
    CLK_SWCR_SWEN = 1; // 执行切换
    CLK_CKDIVR = 0x00; // 设置时钟分频  f HSI= f HSI RC输出/1    f CPU= f MASTER
    //---------------------------------------- 外设
    //CLK_PCKENR1 = 0x84;						// T1,UART1
    CLK_PCKENR1 = 0x66; //0x64; // T3?T4,UART1,beep
    CLK_PCKENR2 = 0x03; // ADC,T1

    CLK_ICKCR_LSION = 1; // 使能内部LSI OSC�?8KHz�?
    while (CLK_ICKCR_LSIRDY == 0)
        ; // �?查内部LSI OSC
}

void beep_init(void)
{
    //BEEP_CSR=0x4E;
    BEEP_CSR2 = 0;
    BEEP_CSR2_BEEPDIV = 9;
    BEEP_CSR2_BEEPSEL = 2;
    CLK_CBEEPR_CLKBEEPSEL0 = 1;
    CLK_CBEEPR_CLKBEEPSEL1 = 0;

	BEEP_CSR2_BEEPEN = 0;
}

//===================Delayus()延时===============//    Crystal: 16M HSI
void Delayus(unsigned char timer)
{
    unsigned char x; //延时T=((timer-1)*0.313+2 us
    unsigned char timer_cache;
    timer_cache = timer / 3;
    for (x = 0; x < timer_cache; x++)
    {
        __asm("nop");
    }
}

void BEEP_GPIO_Init(void)
{
    PIN_BEEP_DDR = Output;
    PIN_BEEP_CR1 = 1;
    PIN_BEEP = 0;
}

void Receiver_OUT_GPIO_Init(void)
{
    Receiver_OUT_OPEN_DDR = Output; // Output   受信机继电器OPEN  高电平有�?
    Receiver_OUT_OPEN_CR1 = 1;
    Receiver_OUT_OPEN = 0;

    Receiver_OUT_STOP_DDR = Output; // Output   受信机继电器STOP  高电平有�?
    Receiver_OUT_STOP_CR1 = 1;
    if(mode_sel.Mode_Type == SLAVE_TYPE)
    {
        Receiver_OUT_STOP = 1;
        key_sta = Key_Stop;
    }
    else Receiver_OUT_STOP = 0;

    Receiver_OUT_CLOSE_DDR = Output; // Output   受信机继电器CLOSE  高电平有�?
    Receiver_OUT_CLOSE_CR1 = 1;
    Receiver_OUT_CLOSE = 0;
}
/**
****************************************************************************
* @Function	: void LED_GPIO_Init(void)
* @file		: Initial.c
* @Author	: Xiaowine
* @date		: 2017/4/10
* @version	: V1.0
* @brief
**/
void LED_GPIO_Init(void)
{
    Receiver_LED_OUT_direc = Output; // Output   受信机继电器动作输出  高电平有�?
    Receiver_LED_OUT_CR1 = 1;
    Receiver_LED_OUT = 0;

    Receiver_LED_TX_direc = Output; // Output   受信机�?�信指示  高电平有�?
    Receiver_LED_TX_CR1 = 1;
    Receiver_LED_TX = 0;

    Receiver_LED_RX_direc = Output; // Output   受信机受信指�? 高电平有�?
    Receiver_LED_RX_CR1 = 1;
    Receiver_LED_RX = 0;
}

/**
****************************************************************************
* @Function	: void KEY_GPIO_Init(void)
* @file		: Initial.c
* @Author	: Xiaowine
* @date		: 2017/4/10
* @version	: V1.0
* @brief
**/
void KEY_GPIO_Init(void)
{
    PIN_TEST_DDR = Input;
    PIN_TEST_CR1 = Pull_up;
    PIN_TEST_CR2 = InterruptDisable;

    SelDev_Mode_DDR = Input;
    SelDev_Mode_CR1 = Floating;
    SelDev_Mode_CR2 = InterruptDisable;

    KEY_UP_DDR = Input;            // 输入     test�?
    KEY_UP_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
    KEY_UP_CR2 = InterruptDisable; //禁止中断

    KEY_DOWN_DDR = Input;            // 输入     test�?
    KEY_DOWN_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
    KEY_DOWN_CR2 = InterruptDisable; //禁止中断

    KEY_ENTER_DDR = Input;            // 输入     test�?
    KEY_ENTER_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
    KEY_ENTER_CR2 = InterruptDisable; //禁止中断

    if(mode_sel.Mode_Type == SLAVE_TYPE)
    {
        KEY_OPEN_DDR = Input;            // 输入     test�?
        KEY_OPEN_CR1 = Floating;//Pull_up;          //1: Input with pull-up 0: Floating input
        KEY_OPEN_CR2 = InterruptDisable; //禁止中断

        KEY_STOP_DDR = Input;            // 输入     test�?
        KEY_STOP_CR1 = Floating;          //1: Input with pull-up 0: Floating input
        KEY_STOP_CR2 = InterruptDisable; //禁止中断

        KEY_CLOSE_DDR = Input;            // 输入     test�?
        KEY_CLOSE_CR1 = Floating;          //1: Input with pull-up 0: Floating input
        KEY_CLOSE_CR2 = InterruptDisable; //禁止中断
    }
    else
    {
        KEY_OPEN_DDR = Input;            // 输入     test�?
        KEY_OPEN_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
        KEY_OPEN_CR2 = InterruptDisable; //禁止中断

        KEY_STOP_DDR = Input;            // 输入     test�?
        KEY_STOP_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
        KEY_STOP_CR2 = InterruptDisable; //禁止中断

        KEY_CLOSE_DDR = Input;            // 输入     test�?
        KEY_CLOSE_CR1 = Pull_up;          //1: Input with pull-up 0: Floating input
        KEY_CLOSE_CR2 = InterruptDisable;
    }
}

void E22_GPIO_Init(void)
{
    E22_M0_DDR = Output;
    E22_M0_CR1 = 1;

    E22_M1_DDR = Output;
    E22_M1_CR1 = 1;

    E22_RST_DDR = Input;
    //E22_RST_CR1 = 1;
    //E22_RST = 0;

    E22_AUX_DDR = Input;
    E22_AUX_CR1 = Floating;
    E22_AUX_CR2 = InterruptDisable;

    E22_M0 = 0;
    E22_M1 = 0; //mode 0
}

void user_mode_init(void)
{
    u8 si = 0;
    if(SelDev_ModeIN)
        mode_sel.Mode_Set = NORMAL_MODE;
    else
        mode_sel.Mode_Set = RELAY_MODE;

    for(si=0; si<5; si++)
    {
        memcpy(&user_host_config[si],&host_config_default,sizeof(user_config_t));
        memcpy(&user_slave_config[si],&slave_config_default,sizeof(user_config_t));
    }
}


u8 key_ms = 0;
void key_1ms_scan_host(void)
{
    if(flag_key==0 && (KEY_UP_IN==0 || KEY_DOWN_IN==0 || KEY_ENTER_IN==0 || KEY_OPEN_IN==0 || KEY_STOP_IN==0 || KEY_CLOSE_IN==0))
    {
        key_ms++;
        if(key_ms > 50)  //ms
        {
            key_ms = 0;
            flag_key = 1;
            if(KEY_UP_IN == 0)          key_sta = Key_Up;
            else if(KEY_DOWN_IN == 0)   key_sta = Key_Down;
            else if(KEY_ENTER_IN == 0)  key_sta = Key_Enter;
            else if(KEY_OPEN_IN == 0)   key_sta = Key_Open;
            else if(KEY_STOP_IN == 0)   key_sta = Key_Stop;
            else if(KEY_CLOSE_IN == 0)  key_sta = Key_Close;
            else key_sta = Key_None;
        }
    }
    else
    {
        key_ms = 0;
        if(KEY_UP_IN==1 && KEY_DOWN_IN==1 && KEY_ENTER_IN==1 && KEY_OPEN_IN==1 && KEY_STOP_IN==1 && KEY_CLOSE_IN==1)
        {
            flag_key = 0;
        }
    }
}

void key_1ms_scan_slave(void)
{
    //if((key_sta=Key_Close&&KEY_OPEN_IN==1&&KEY_STOP_IN==1) || (key_sta=Key_Open&&KEY_STOP_IN==1&&KEY_CLOSE_IN==1) || (key_sta=Key_Stop&&KEY_OPEN_IN==1&&KEY_CLOSE_IN==1))
    {
       // return;
    }

    switch(key_sta)
    {
        case Key_Open:
            if(KEY_STOP_IN==1 && KEY_CLOSE_IN==0)
            {
                key_ms++;
                if(key_ms > 50)  //ms
                {
                    key_ms = 0;
                    if(KEY_STOP_IN == 1)// && flag_tx_stop == 0)
                    {
                        //flag_tx_stop = 1;
                        key_sta = Key_Stop;
                        Receiver_OUT_OPEN = 0;
                        Receiver_OUT_STOP = 1;
                        Receiver_OUT_CLOSE = 0;
                    }
                }
            }
            else if(KEY_STOP_IN==0 && KEY_CLOSE_IN==1)
            {
                key_ms++;
                if(key_ms > 50)  //ms
                {
                    key_ms = 0;
                    if(KEY_CLOSE_IN == 1)// && flag_tx_close == 0)
                    {
                        //flag_tx_close = 1;
                        key_sta = Key_Close;
                        Receiver_OUT_OPEN = 0;
                        Receiver_OUT_STOP = 0;
                        Receiver_OUT_CLOSE = 1;
                    }
                }
            }
            else key_ms = 0;
            break;

        case Key_Stop:
            if(KEY_OPEN_IN==1 && KEY_CLOSE_IN==0)
            {
                key_ms++;
                if(key_ms > 50)  //ms
                {
                    key_ms = 0;
                    if(KEY_OPEN_IN == 1)// && flag_tx_open == 0)
                    {
                        //flag_tx_open = 1;
                        key_sta = Key_Open;
                        Receiver_OUT_OPEN = 1;
                        Receiver_OUT_STOP = 0;
                        Receiver_OUT_CLOSE = 0;
                    }
                }
            }
            else if(KEY_OPEN_IN==0 && KEY_CLOSE_IN==1)
            {
                key_ms++;
                if(key_ms > 50)  //ms
                {
                    key_ms = 0;
                    if(KEY_CLOSE_IN == 1)// && flag_tx_close == 0)
                    {
                        //flag_tx_close = 1;
                        key_sta = Key_Close;
                        Receiver_OUT_OPEN = 0;
                        Receiver_OUT_STOP = 0;
                        Receiver_OUT_CLOSE = 1;
                    }
                }
            }
            else key_ms = 0;
            break;

        case Key_Close:
            if(KEY_OPEN_IN==1 && KEY_STOP_IN==0)
            {
                key_ms++;
                if(key_ms > 50)  //ms
                {
                    key_ms = 0;
                    if(KEY_OPEN_IN == 1)// && flag_tx_open == 0)
                    {
                        //flag_tx_open = 1;
                        key_sta = Key_Open;
                        Receiver_OUT_OPEN = 1;
                        Receiver_OUT_STOP = 0;
                        Receiver_OUT_CLOSE = 0;
                    }
                }
            }
            else if(KEY_OPEN_IN==0 && KEY_STOP_IN==1)
            {
                key_ms++;
                if(key_ms > 50)  //ms
                {
                    key_ms = 0;
                    if(KEY_STOP_IN == 1)// && flag_tx_stop == 0)
                    {
                        //flag_tx_stop = 1;
                        key_sta = Key_Stop;
                        Receiver_OUT_OPEN = 0;
                        Receiver_OUT_STOP = 1;
                        Receiver_OUT_CLOSE = 0;
                    }
                }
            }
            else key_ms = 0;
            break;
    }
}

