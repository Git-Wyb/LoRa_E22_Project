#ifndef __type_def_h__
#define __type_def_h__
/**
****************************************************************************
* @Warning :Without permission from the author,Not for commercial use
* @File    :type_def.h
* @Author  :Xiaowine
* @date    :2017/4/14
* @version :V 1.0.0
*************************************************
* @brief   :
****************************************************************************
**/

/*主控芯片的头文件*/
//

/*bool 类型头文�?*/
#include "stdbool.h"

/*总中断头文件*/
#include "intrinsics.h"

/*常用数据类型定义*/
typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned long u32_t;
#define u8 u8_t
#define u16 u16_t
#define u32 u32_t
#define EnableInterrupt __enable_interrupt()

#define SUCESS 1
#define FAILURE 0

//代码标志�?
typedef union _fullflag_ {
    unsigned char BYTE;
    struct
    {
        unsigned char Bit0 : 1;
        unsigned char Bit1 : 1;
        unsigned char Bit2 : 1;
        unsigned char Bit3 : 1;
        unsigned char Bit4 : 1;
        unsigned char Bit5 : 1;
        unsigned char Bit6 : 1;
        unsigned char Bit7 : 1;
    } BIT;
} uFLAG, *P_uFLAG;
typedef union {
    u32 whole_reg;
    u8 byte[4]; // Warning: Be endian-specific when accessing bytes

} ADF70XX_REG_T;

typedef union {
	  u16  ui ;
  u8   uc[2] ;
}uni_i;


typedef struct body    //����ṹ��
{
	union
	{
		u8 byte;
		struct
		{
			unsigned char type :5;
			unsigned char fno :3;
		} UN;
	}Fno_Type;

    uni_i data[4];  //无线data
}Wireless_Body;  //结构体


#define TRANSITION_STATUS_TRANSITION 0
#define TRANSITION_STATUS_EXECUTING 1
#define TRANSITION_STATUS_IDLE 2
typedef struct
{
    union {
        u8 VALUE; /*!< SPI Slave Status                                                      */

        struct
        {
            u8 MEM_ACC_STATUS : 1; /*!< *Instantaneous* value of an AHB read data underflow condition
                                                    for memory reads. The same bit doubles up as an AHB bus error
                                                     indicator for memory writes.                                         */
            u8 FW_STATUS : 4;      /*!< *Instantaneous* value of the firmware status of the radio controller
                                                    state machine.                                                        */
            u8 CMD_READY : 1;      /*!< *Instantaneous* value of the indicator to the host that no posted
                                                    (to the SPI Slave) radio controller command is currently awaiting
                                                     servicing by the firmware radio state machine running on the
                                                     Cortex-M0.                                                           */
            u8 EXT_IRQ_STATUS : 1; /*!< *Instantaneous* value of the commoned-up (bitwise OR of the)
                                                    external interrupt outputs from the ADF7030, made available
                                                     in this format to the host for use in pin-limited systems.           */
            u8 SPIS_READY : 1;     /*!< *Instantaneous* value of the confirmation to the external host
                                                    that the HCLK clock domain in the ADF7030 has power, is out
                                                     of reset and has a running clock.                                    */
        };                         /*!< BitSize                                                               */
    };
} ADF7030_1_STATUS_TYPE;



typedef union
{
    u8 Ststus_Sign;
    struct{
        u8 Flag_LowerLimit : 1;         /* 下限信号标志,0:有下限; 1:无下限 */
        u8 Flag_AbnormalSignal : 1;     /* 异常信号标志,0:异常; 1:无异常 */
        u8 Flag_ActionSignal : 1;       /* 动作中标志,0:动作中; 1:无动作 */
        u8 Exist_ID : 1;                /* 0:没有ID; 1:存在ID */
        u8 PROFILE_RxLowSpeed_TYPE : 1; /* 0:频率426MHz; 1:频率429MHz */
        u8 Receive_SignalType : 1;      /* 0:受信于自动信号; 1:受信于AAP或者遥控器;  */
        u8 ActionOpenOrClose : 1;       /* 0:闭动作; 1:开动作; 记录受信时接收的是开命令还是关命令*/
        u8 Buzzer_Switch : 1;           /* 蜂鸣器开关, 0:不允许开蜂鸣器; 1:可以开蜂鸣器*/
    };
}Basic_Un;


typedef union
{
    u8 Falg_Byte;
    struct{
        u8 FlagByte_bit0 : 1;
        u8 FlagByte_bit1 : 1;
        u8 FlagByte_bit2 : 1;
        u8 FlagByte_bit3 : 1;
        u8 FlagByte_bit4 : 1;
        u8 FlagByte_bit5 : 1;
        u8 FlagByte_bit6 : 1;
        u8 FlagByte_bit7 : 1;
    };
}Flag_Un;



#endif
