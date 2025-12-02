/***********************************************************************/
/*  FILE        :Uart.c                                                */
/*  DATE        :Mar, 2014                                             */
/*  Programmer	:xiang 'R                                              */
/*  CPU TYPE    :STM8L151G6     Crystal: 16M HSI                       */
/*  DESCRIPTION :                                                      */
/*  Mark        :ver 1.0                                               */
/***********************************************************************/

#include "Pin_define.h"   // 管脚定义
#include "initial.h"	  // 初始�? 预定�?
#include "ram.h"		  // RAM定义
#include "eeprom.h"		  // eeprom
#include "uart.h"
#include "IIC.h"
#include "fifo.h"
#include "Timer.h"
#include "E22-900T.h"

#define TXD1_enable (USART1_CR2 = 0x08) // 允许发�??
#define RXD1_enable (USART1_CR2 = 0x24) // 允许接收及其中断

u8 u1busyCache = 0;
u8 u1InitCompleteFlag = 0;

UINT8 UartStatus = FrameHeadSataus;
UINT8 UartLen = 0;
UINT8 UartCount = 0;
UINT8 UART_DATA_buffer[41] = {0};
UINT8 UART_DATA_ID98[41] = {0};

__Databits_t Databits_t;
__U1Statues U1Statues;
UINT8 ACKBack[3] = {0x02, 0x03, 0x00};
unsigned int U1AckTimer = 0;

UINT8 FLAG_testNo91=0;
UINT8 FLAG_testBEEP=0;
//UINT8 FLAG_testNo91_step=0;
UINT8 FLAG_testNo91SendUart=0;

static uint8_t fifo_buffer[1024];
static fifo_t fifo_uart1_rx;
static volatile uint32_t uart1_rx_timeout = 0;
static bool uart1_rx_done = false;

//*************** E22 通信*****************************
void UART1_INIT(void)
{
	unsigned int baud_div = 0;

	SYSCFG_RMPCR1_USART1TR_REMAP = 0;
	USART1_CR1_bit.M = 0;
	USART1_CR1_bit.PCEN = 0;
	USART1_CR1_bit.PS = 0;
	USART1_CR2_bit.TIEN = 0;
	USART1_CR2_bit.TCIEN = 0;
	USART1_CR2_bit.RIEN = 1;
	USART1_CR2_bit.ILIEN = 0;
	USART1_CR2_bit.TEN = 1;
	USART1_CR2_bit.REN = 1;

	/*设置波特�? */
	baud_div = 16000000 / 9600; /*求出分频因子*/
	USART1_BRR2 = baud_div & 0x0f;
	USART1_BRR2 |= ((baud_div & 0xf000) >> 8);
	USART1_BRR1 = ((baud_div & 0x0ff0) >> 4); /*先给BRR2赋�??�?后再设置BRR1*/

    if( fifo_create( &fifo_uart1_rx, fifo_buffer , sizeof(fifo_buffer)) != FIFO_OK )
    {
        while(1);
    }
    uart1_rx_timeout = 0;
	uart1_rx_done = false;
}
void UART1_end(void)
{ //
	SYSCFG_RMPCR1_USART1TR_REMAP = 0;

	USART1_CR1 = 0; // 1个起始位,8个数据位
	USART1_CR3 = 0; // 1个停止位
	USART1_CR4 = 0;
	USART1_CR5 = 0x00;  // 半双工模�?
	USART1_BRR2 = 0x00; // 设置波特�?600
	USART1_BRR1 = 0x00; // 3.6864M/9600 = 0x180
						//16.00M/9600 = 0x683
	USART1_CR2 = 0x00;  //禁止串口
}
//--------------------------------------------
void UART1_RX_RXNE(void)
{ // RXD中断服务程序
    if(USART1_SR_bit.RXNE == 1)
    {
        uart1_rx_data = USART1_DR; // 接收数据
        fifo_write(&fifo_uart1_rx, &uart1_rx_data,1);
        uart1_rx_timeout = 10;
    }
}

void uart1_rx_timeout_1ms_callback(void)
{
	/* 串口接收中断中会不停刷新倒计时
     但滴答定时每1毫秒中断内会递减倒计时	*/
	if( uart1_rx_timeout > 0 )
	{
        uart1_rx_timeout--;

        /*如果倒计时归0，则可以断包处理了 */
        if( uart1_rx_timeout == 0 )
        {
            uart1_rx_done = true;
        }
	}
}

void uart1_wait_response_blocked( uint8_t * buffer, uint16_t *length )
{
	uint32_t fifo_rx_len = 0;

	/* 清除串口接收队列 */
	fifo_clear( &fifo_uart1_rx );

	/* 设定100ms倒计时 滴答定时器1ms中断内不断递减*/
	time_sw = 100;

	/* 等待串口接收完成 */
	while( uart1_rx_done == false )
	{
        /* 如果100ms倒计时归零了 */
        if( time_sw == 0)
        {
            ClearWDT();
            /* 超时了 模组无应答 */
            while(1);
        }
	}
	/* 处理串口应答数据 */
	if( uart1_rx_done == true )
	{
		uart1_rx_done = false;

		/* 获取当前串口接收队列内容长度 */
		fifo_get_length( &fifo_uart1_rx , &fifo_rx_len);

		/* 串口数据拷贝 */
		fifo_read( &fifo_uart1_rx, buffer, fifo_rx_len );

		*length = fifo_rx_len;
	}
}

bool uart1_check_rx_done( unsigned char *buffer , unsigned long *length )
{
	bool ret = false;
	uint32_t fifo_rx_len = 0;

	if( uart1_rx_done == true )
	{
		uart1_rx_done = false;

		/* 获取当前串口接收队列内容长度 */
		fifo_get_length( &fifo_uart1_rx , &fifo_rx_len);

		/* 串口数据拷贝 */
		fifo_read( &fifo_uart1_rx, buffer, fifo_rx_len );
		*length = fifo_rx_len;
        fifo_clear(&fifo_uart1_rx);
		ret = true;

        E22_Data_Check(buffer,fifo_rx_len);
        _ReqBuzzer(400,1,1);
        Receiver_LED_RX = 1;
        Receiver_LED_OUT = 1;
        time_sw = 300;
        flag_led = 1;
	}
	return ret;
}

void UART2_INIT(void)
{
	unsigned int baud_div = 0;

    CLK_PCKENR3 = 0x08; //CLK_PCKENR3_UASRT2;
	USART2_CR1_bit.M = 0;
	USART2_CR1_bit.PCEN = 0;
	USART2_CR1_bit.PS = 0;
	USART2_CR2_bit.TIEN = 0;
	USART2_CR2_bit.TCIEN = 0;
	USART2_CR2_bit.RIEN = 1;
	USART2_CR2_bit.ILIEN = 0;
	USART2_CR2_bit.TEN = 1;
	USART2_CR2_bit.REN = 1;

	/*设置波特玿 */
	baud_div = 16000000 / 9600; /*求出分频因子*/
	USART2_BRR2 = baud_div & 0x0f;
	USART2_BRR2 |= ((baud_div & 0xf000) >> 8);
	USART2_BRR1 = ((baud_div & 0x0ff0) >> 4); /*先给BRR2赋忿朿后再设置BRR1*/
}

void UART2_End(void)
{
    USART2_CR1 = 0;
    USART2_CR2 = 0;
}

void UART2_RX_RXNE(void)
{ // RXD中断服务程序
    unsigned char dat = 0;
    if(USART2_SR_bit.RXNE == 1)
    {
        dat = USART2_DR; // 接收数据
        if(dat == '(' || Uart2_Recv_Buff[0] == '(')
        {
            Uart2_Recv_Buff[Uart2_Cnt++] = dat;
            if(dat == ')')
            {
                Uart2_Recv_Buff[0] = 0;
                Uart2_Cnt = 0;
                flag_uart2_rx = 1;
            }
            if(Uart2_Cnt > BUFFMAX - 1)
            {
                Uart2_Cnt = 0;
                Uart2_Recv_Buff[0] = 0;
                flag_uart2_rx = 0;
            }
        }
    }
}

void UART2_Send_char(unsigned char ch)
{
    while (!USART2_SR_TXE);
	USART2_DR = ch; // 发鿿
	while (!USART2_SR_TC);
}

//--------------------------------------------
void Send_char(unsigned char ch)
{
    flag_rx_done = 0;
	//TXD1_enable; // 允许发�??
	while (!USART1_SR_TXE)
		;
	USART1_DR = ch; // 发�??
	while (!USART1_SR_TC)
		;		 // 等待完成发�??
	//RXD1_enable; // 允许接收及其中断
}
//--------------------------------------------
void Send_String(unsigned char *string)
{ // 发�?�字符串
	unsigned char i = 0;
	//TXD1_enable; // 允许发�??
	while (string[i])
	{
		while (!USART1_SR_TXE)
			;				   // �?查发送OK
		USART1_DR = string[i]; // 发�??
		i++;
	}
	while (!USART1_SR_TC)
		;		 // 等待完成发�??
	//RXD1_enable; // 允许接收及其中断
				 //	BIT_SIO = 0;							// 标志
}
void UART1_Send_Data(unsigned char *P_data, unsigned int length)
{
	unsigned int i = 0;

	for (i = 0; i < length; i++)
	{
		while (!USART1_SR_TXE)
			;
		USART1_DR = *(P_data + i);
	}
	while (!USART1_SR_TC);
}


/***********************************************************************/
unsigned char asc_hex(unsigned char asc) // HEX
{
	unsigned char i;
	if (asc < 0x3A)
		i = asc & 0x0F;
	else
		i = asc - 0x37;
	return i;
}

unsigned char hex_asc(unsigned char hex)
{
	unsigned char i;
	hex = hex & 0x0F;
	if (hex < 0x0A)
		i = hex | 0x30;
	else
		i = hex + 0x37;
	return i;
}

unsigned char asc_hex_2(unsigned char asc1, unsigned char asc0)
{
	unsigned char i;
	i = (asc_hex(asc1) << 4) + (asc_hex(asc0) & 0x0F);
	return i;
}
