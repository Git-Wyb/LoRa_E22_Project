#ifndef __UART_H__
#define __UART_H__
//#include "type_def.h"
/***********************************************************************/
/*  FILE        :Uart.H                                                */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/

//#define uint8_t UINT8
//#define uint16_t UINT16

extern u8 u1busyCache;
#define U1Busy_OUT u1busyCache
#define FrameHeadSataus 0x00 //帧头
#define DataStatus 0x01      //数据位置
#define FrameEndStatus 0x02  //桢结�?
#define FrameHead 0x02       //数据�?�?
#define FrameSingnalID 0x11  //信号ID

extern UINT8 UartStatus;
extern UINT8 UartLen;
extern UINT8 UartCount;
extern UINT8 UART_DATA_buffer[41];
extern UINT8 UART_DATA_ID98[41];

extern u8 u1InitCompleteFlag;

extern UINT8 ACKBack[3];
extern UINT8 FLAG_testNo91;
extern UINT8 FLAG_testBEEP;
//extern UINT8 FLAG_testNo91_step;
extern UINT8 FLAG_testNo91SendUart;



void ReceiveFrame(UINT8 Cache);
void OprationFrame(void);

typedef union {
    unsigned char Data[4];
    struct
    {
        unsigned char ID_No : 8;
        unsigned char Statues : 4;
        unsigned char Mode : 4;
        unsigned char Abnormal : 8;
        unsigned char Standby : 8;
    };
    struct
    {
        unsigned char ID_No98: 8;
        unsigned char SW_Info : 8;
        unsigned char AbnormalOut1 : 8;
        unsigned char AbnormalOut2 : 8;
    };
    struct
    {
        unsigned char ID_test_No91or93: 8;
        unsigned char SWorOUT : 8;
        unsigned char prep1 : 8;
        unsigned char prep2 : 8;
    };

} __Databits_t;
typedef enum {
    IdelStatues = 0,
    ReceivingStatues,
    ReceiveDoneStatues,
    ACKingStatues,
    ACKDoneStatues,
} __U1Statues;
extern __U1Statues U1Statues;
extern unsigned int U1AckTimer;
#define U1AckDelayTime 2

void UART1_INIT(void);
void UART1_RX_RXNE(void);
void Send_char(unsigned char ch);
void Send_String(unsigned char *string);
void UART1_Send_Data(unsigned char *P_data, unsigned int length);
void UART1_end(void);
void PC_PRG(void);
void UART2_INIT(void);
void UART2_RX_RXNE(void);
u8 Uart2_RTC_Write(void);
void Uart2_RTC_Read(void);
void UART2_End(void);
void uart1_rx_timeout_1ms_callback(void);
unsigned char asc_hex(unsigned char asc);
unsigned char hex_asc(unsigned char hex);
unsigned char asc_hex_2(unsigned char asc1, unsigned char asc0);
bool uart1_check_rx_done( unsigned char *buffer , unsigned long *length );
void uart1_wait_response_blocked( UINT8 * buffer, UINT16 *length );

#endif
