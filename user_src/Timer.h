#ifndef __TIMER_H__
#define __TIMER_H__
#include "type_def.h"
/***********************************************************************/
/*  FILE        :Timer.h                                               */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8S003     Crystal: 4M HSI                          */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/

extern u16 ErrStateTimeer;
extern u16 StateReadTimer;
//============================================ Timer 4
void TIM4_Init(void);
void TIM4_UPD_OVF(void);
void mDelaymS(u16 ms);
void mDelayuS(u8 us);
void _ReqBuzzer(u16 d_BEEP_on,u16 d_BEEP_off,u16 d_BEEP_freq);
void BEEP_Module(u16 time_beepON, u16 time_beepOFF);

#endif
