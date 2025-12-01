/**
  ******************************************************************************
  * @file    Project/STM8L15x_StdPeriph_Template/main.c
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    13-May-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <iostm8l151c8.h> // CPU型号
#include <stdio.h>
#include "Pin_define.h"   // 管脚定义
#include "initial.h"      // 初始�?  预定�?
#include "ram.h"          // RAM定义
#include "Timer.h"        // 定时�?
#include "eeprom.h"       // eeprom
#include "uart.h"         // uart
#include "lcd.h"
#include "E22-900T.h"
#include "display.h"

u8 sstr[] = {"Setting"};
u8 sbuff[20];
u32 length = 0;
void main(void)
{
    _DI();             // 关全�?中断
    RAM_clean();       // 清除RAM
    //WDT_init();        //看门狿
    VHF_GPIO_INIT();   //IO初始�?
    SysClock_Init();   //系统时钟初始�?
    InitialFlashReg(); //flash EEPROM
    TIM4_Init();       // 定时�?
    UART1_INIT();
    _EI();
    if(Mode_Sel != SLAVE_MODE)
    {
        lcd_init();
        menu_start();
    }
    else{key_sta = Key_Stop;}
    E22_Test_Mode();
    e22_hal_work_mode(WORK_MODE_TRANSPARENT);
    //e22_test_mode(1);
    while (1)
    {
        ClearWDT(); // Service the WDT
        //time_sw = 500;
        //while(time_sw);
        if(Mode_Sel != SLAVE_MODE) check_key_sta();
        else tx_state();
        uart1_check_rx_done(sbuff,&length);
        if(time_sw==0 && flag_led)
        {
            flag_led = 0;
            Receiver_LED_TX = 0;
            Receiver_LED_RX = 0;
            Receiver_LED_OUT = 0;
        }
    }
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
