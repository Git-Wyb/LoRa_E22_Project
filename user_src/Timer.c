/***********************************************************************/
/*  FILE        :Timer.c                                               */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/
#include <iostm8l151c8.h> // CPU型号
#include "Pin_define.h"   // 管脚定义
#include "initial.h"      // 初始�? 预定�?
#include "ram.h"          // RAM定义
#include "uart.h"
#include "ID_Decode.h"
#include "Timer.h"
u16 ErrStateTimeer = 1;
u16 StateReadTimer = 500;
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Timer 4 start   1ms
void TIM4_Init(void)
{
    TIM4_PSCR = 0x06; // Timer 4 prescaler  计数器时钟频�? f CK_CNT  =f CK_PSC  / 2的N次方
                      //TIM4_PSCR = 0x08;	// Timer 4 prescaler  计数器时钟频�? f CK_CNT  = f CK_PSC/ 2(PSC[3:0])
    TIM4_ARR = 0xF9;  // Timer 4 period
    TIM4_CR1 |= 0x01; // Timer 4 Enable
    TIM4_IER |= 0x01; // Timer 4 OVR interrupt
}

u16 time_led = 0;
void key_1ms_scan_host(void);
void key_1ms_scan_slave(void);
void TIM4_UPD_OVF(void)
{
    if(time_sw) time_sw--;
    if(time_txcheck) time_txcheck--;
    if(flag_rx_time) time_rxack++;
    else time_rxack = 0;
    if(time_step) time_step--;

    BEEP_function();
    if(Mode_Sel == SLAVE_MODE) key_1ms_scan_slave();
    else key_1ms_scan_host();
    uart1_rx_timeout_1ms_callback();

    TIM4_SR1_bit.UIF = 0; // 清除中断标记
}

void mDelaymS(u16 ms)
{
    time_sw = ms;
    while(time_sw)
    {
        ClearWDT();
    }
}

void Delay_us(unsigned short timer)
{
    unsigned short x; //延时T=((timer-1)*0.313+2 us
    unsigned short timer_cache;
    timer_cache = timer / 3;
    for (x = 0; x < timer_cache; x++)
    {
        __asm("nop");
    }
}

void mDelayuS(u8 us) //10=10us,250=300us
{
    Delay_us(4 * us);
}

void TIM3_init(void)
{
    TIM3_CCMR1 = TIM3_CCMR1 | 0x70;
    TIM3_CCER1 = TIM3_CCER1 | 0x03;  //TIME3_CH1
    TIM3_ARRH = 0x08;                //0x07D0 -->PWM=2K        0x0880 -->PWM=1.83K
    TIM3_ARRL = 0x84;
                                     //TIM2_IER = 0x01;						// ??????????
    TIM3_CCR1H = 0x04;               //50%
    TIM3_CCR1L = 0x42;
    TIM3_PSCR = 0x02;                // ?????=Fsystem/(2(PSC[2:0])????4MHz=16MHz/2/2

    TIM3_CR1 = TIM3_CR1 | 0x01;
    TIM3_BKR = 0x80;
}

void BEEP_ON(void)
{
    TIM3_init();
}

void BEEP_OFF(void)
{                     // ???Tone   2015.3.11????
    TIM3_CR1_CEN = 0; // Timer 3 Disable
    TIM3_CCMR1 =  0x00;
    TIM3_CCER1 =  0x00; //????PWM?????????????????I/O
    PIN_BEEP = 0;
}

void BEEP_Module(u16 time_beepON, u16 time_beepOFF)
{
	UINT16 i;

	for (i = 0; i < time_beepON; i++)
	{
		//Receiver_Buzzer=!Receiver_Buzzer;   //蜂鸣器鿑翿.08KHZ
		if (FG_beep_on == 0)
		{
			FG_beep_on = 1;
			FG_beep_off = 0;
            BEEP_ON();
        }
		Delayus(250); //80us
		Delayus(250); //80us
		Delayus(250); //80us
		ClearWDT();   // Service the WDT
	}
	for (i = 0; i < time_beepOFF; i++)
	{
		if (FG_beep_off == 0)
		{
			FG_beep_off = 1;
			FG_beep_on = 0;
            BEEP_OFF();
        }
		Delayus(250); //80us
		Delayus(250); //80us
		Delayus(250); //80us
		ClearWDT();   // Service the WDT
	}

}

void BEEP_function(void)
{
    if(TIME_BEEP_on)
    {
        if(TIME_BEEP_on < 0xfff0)
        {     //大于0xfff0表示一直叫
            --TIME_BEEP_on;
            if(FG_beep_on_Motor == 0)
            {
                FG_beep_on_Motor = 1;
                FG_beep_off_Motor = 0;
                BEEP_ON();
            }
        }
    }
    else if(TIME_BEEP_off)
    {
        --TIME_BEEP_off;
        if(FG_beep_off_Motor == 0)
        {
            FG_beep_off_Motor = 1;
            FG_beep_on_Motor = 0;
            BEEP_OFF();
        }
    }
    else if(TIME_BEEP_freq)
    {
        if(TIME_BEEP_freq < 0xfff0) //大于0xfff0表示一直循环叫
        {
            --TIME_BEEP_freq;
        }
        TIME_BEEP_on = BASE_TIME_BEEP_on;
        TIME_BEEP_off = BASE_TIME_BEEP_off;
        if(FG_beep_on_Motor == 0)
        {
            FG_beep_on_Motor = 1;
            FG_beep_off_Motor = 0;
            TIM3_init();
        }
    }
}

void _ReqBuzzer(u16 d_BEEP_on,u16 d_BEEP_off,u16 d_BEEP_freq)
{
    BASE_TIME_BEEP_on = d_BEEP_on;
    BASE_TIME_BEEP_off = d_BEEP_off;
    TIME_BEEP_on = BASE_TIME_BEEP_on;
    TIME_BEEP_off = BASE_TIME_BEEP_off;
    TIME_BEEP_freq = d_BEEP_freq - 1;
}
