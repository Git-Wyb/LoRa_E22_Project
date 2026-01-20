#include "E22-900T.h"
#include <iostm8l151c8.h>
#include <string.h>
#include "ram.h"
#include "uart.h"
#include "display.h"

/*
中继的模块地址：高8位H = 模块1的网络ID；低8位L = 模块2的网络ID；
模块地址相同，可以相互通信。
Relay module address: The upper 8 bits H = Network ID of Module 1; the lower 8 bits L = Network ID of Module 2;
If the module addresses are the same, they can communicate with each other.
*/
/**
 * 模组数据操作缓存
 */
static e22_opt_buffer_t e22_buffer;

/**
 * 第1字节 ：控制指令  0xC1代表读取;
 * 第2字节 ：起始地址  0x00代表从寄存器地址0开始读取
 * 第3字节 ：读取长度  0x0B代表一共读取11个寄存器参数
 */
static const uint8_t request_config[3]={0xC1,0x00,0x0B};

/**
 * 第1~4字节：特殊读取指令头 C0 C1 C2 C3
 * 第5字节  ：起始地址  00
 * 第6字节  ：读取长度  01
 */
static const uint8_t request_env_rssi[6] = {0xC0,0xC1,0xC2,0xC3,0x00,0x01};

/**
 * AT指令 读取设备名
 */
static uint8_t request_name[] = "AT+DEVTYPE=?";

/**
 * AT指令 读取固件版本
 */
static uint8_t request_version[] = "AT+FWCODE=?";

//设置后⽴即保存,但需要重启或者切换模式后才⽣效.
static uint8_t set_fpower[] = "AT+FPOWER=10,13,17,22";

/*
发射频率922.2MHz,实测偏了2K，21ppm误差
SF5-9 BW125KHz  可以。SF9，速率实测2.44Kbps
SF10  BW125不行，250KHz可以
SF11  BW125、250不行，500KHz可以
*/
/*1使能⾃定义调试参数,5设置LoRa的扩频因⼦为SF5,4发射带宽BW为125KHz,1编码率为CR4/5*/
static uint8_t set_sf5[] = "AT+ULORAM=1,5,4,1";
static uint8_t set_sf6[] = "AT+ULORAM=1,6,4,1";
static uint8_t set_sf7[] = "AT+ULORAM=1,7,4,1";
static uint8_t set_sf8[] = "AT+ULORAM=1,8,4,1";
static uint8_t set_sf9[] = "AT+ULORAM=1,9,4,1";
static uint8_t set_sf10[] = "AT+ULORAM=1,10,5,1";
static uint8_t set_sf11[] = "AT+ULORAM=1,11,6,1";

/*使能⾃定义载波频率参数,设置起始基准频率为922.2MHz,
信道频率间隔200KHz,最⼤截⽌信道为第10信道:即为922.2+10*0.2=924.2MHz*/
static uint8_t set_freq[] = "AT+UFREQ=1,923200000,200000,10";
static uint8_t set_checkfreq[] = "AT+UFREQ=?";

/*1使能单载波发射,在922.2MHz持续输出,设置参数不做保存,重启丢失*/
static uint8_t tx_carrier1[] = "AT+SWAVE=1,923200000";
static uint8_t tx_carrier0[] = "AT+SWAVE=0,0";//关闭

/*使能调制波发射,在922.2MHz持续输出*/
static uint8_t tx_modul1[] = "AT+MWAVE=1,923200000";
static uint8_t tx_modul0[] = "AT+MWAVE=0,0";//0关闭

/*恢复到默认参数*/
static uint8_t tx_DEFAULT[] = "AT+DEFAULT";

/*模组重启*/
static uint8_t tx_RESET[] = "AT+RESET";

/* HOST TX: SlaveIDaddrH, SlaveIDaddrL, FreqChannel, set/check State, HostID, State, endCode */
u8 e22_txdata[100] = {0x00,0x00,0x00,0x11,0x00,0x00,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,
                   0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15,0x15};
/* SLAVE TX: addrH, addrL, FreqChannel, DataHead, SlaveID, State, slave_rssi */


user_config_t host_config_default = {
    .channel = 0,           //freq channel
    .modu_addr_h = 0x00,
    .modu_addr_l = 0x00,
    .network_id = 0x00,     //35
    .router_mode = OFF,
    .tx_power = TX_POWER_DBM_13,
    .rate_mode = RADIO_RATE_2400,
    .tx_count = 5
};

user_config_t slave_config_default = {
    .channel = 0,           //freq channel
    .modu_addr_h = 0x00,
    .modu_addr_l = 0x00,
    .network_id = 0x00,     //35
    .router_mode = OFF,
    .tx_power = TX_POWER_DBM_13,
    .rate_mode = RADIO_RATE_2400,
    .tx_count = 5
};

user_config_t user_relay_config = {
    .channel = 0,           //freq channel
    .modu_addr_h = 0x08,
    .modu_addr_l = 0x23,
    .network_id = 0x08,
    .router_mode = ON,
    .tx_power = TX_POWER_DBM_13,
    .rate_mode = RADIO_RATE_2400,
    .tx_count = 5
};

user_config_t user_host_config[5];
user_config_t user_slave_config[5];
/**
 * E22-900T22S 默认配置参数
 */
static const e22_register_t register_default =
{
	.register_0 = {
		.address_h = 0x00,
	},
	.register_1 = {
		.address_l = 0x00,
	},
	.register_2 = {
		.network_id = 0x00,
	},
	.register_3.field = {
		.radio_rate       = RADIO_RATE_2400,
		.uart_parity      = UART_8N1,
		.uart_baud_rate   = UART_RATE_9600,
	},
	.register_4.field = {
		.tx_power         = TX_POWER_DBM_13,
		.software_switch  = OFF,
		.environment_rssi = OFF,
		.packet_size      = PACKET_SIZE_240,
	},
	.register_5 = {
		.channel = 0,//freq channel
	},
	.register_6.field = {
		.wake_on_radio_period  = WOR_PERIOD_2000MS,
		.wake_on_radio_role    = WOR_SLAVE,
		.listen_before_talk    = OFF,
		.router_mode           = OFF, //relay enable
		.specify_target        = ON,  ///* 指定目标传输，也叫定点模式(寄存器地址:06H Bit6)将用户串口传入数据的前三字节用来改变地址与信道 */
		.packet_rssi           = ON,
	},
	.register_7 = {
		.password_h = 0x00,
	},
	.register_8 = {
		.password_l = 0x00,
	},
	.register_9 = {
		.wor_rx_echo_time_h = 0x00,
	},
	.register_10 = {
		.wor_rx_echo_time_l = 0x00,
	},
};

void e22_at_defaule(void)
{
    e22_config_atcommand(tx_DEFAULT,strlen(tx_DEFAULT));
}

void e22_parameter_init(void)
{
    e22_config_atcommand(set_freq,strlen(set_freq));
    e22_config_atcommand(set_sf9,strlen(set_sf9));
    e22_config_atcommand(tx_RESET,strlen(tx_RESET)); //模组重启，参数生效。
    mDelaymS(5);
    //e22_hal_reset();
    mDelaymS(5);
    //e22_config_atcommand(set_checkfreq,strlen(set_checkfreq));
}

void e22_user_config_init(MODE_SET_STU mode)
{
    switch(mode.Mode_Type)
    {
        case HOST_TYPE:
            if(mode.host_id<HOST_11 || mode.host_id>HOST_15) mode.host_id = HOST_11;
            user_host_config[mode.host_id-11].modu_addr_h = 0x00;
            user_host_config[mode.host_id-11].modu_addr_l = mode.host_id;
            user_host_config[mode.host_id-11].network_id = 0x08;
            if(mode_sel.Mode_Set == NORMAL_MODE) e22_send_userconfig(&user_host_config[mode.host_id-11]);
            else e22_send_userconfig(&user_relay_config);
            break;

        case SLAVE_TYPE:
            if(mode.salve_id<SLAVE_1 || mode.salve_id>SLAVE_5) mode.salve_id = SLAVE_1;
            if(mode_sel.Mode_Set == NORMAL_MODE)
            {
                user_slave_config[mode.salve_id-1].modu_addr_h = 0x00;
                user_slave_config[mode.salve_id-1].modu_addr_l = mode.salve_id;
                user_slave_config[mode.salve_id-1].network_id = 0x08;
            }
            else
            {
                user_slave_config[mode.salve_id-1].modu_addr_h = 0x00;
                user_slave_config[mode.salve_id-1].modu_addr_l = mode.salve_id;
                user_slave_config[mode.salve_id-1].network_id = 0x23;
            }
            e22_send_userconfig(&user_slave_config[mode.salve_id-1]);
            break;
    }
}

void e22_hal_uart_tx(uint8_t *buffer, uint16_t length)
{
    UART1_Send_Data(buffer,length);
    flag_led = 1;
    time_sw = 500;
    Receiver_LED_TX = 1;
}

void e22_config_atcommand(uint8_t *config,uint16_t length)
{
    e22_hal_work_mode(WORK_MODE_CONFIG);
    e22_hal_uart_tx(config,length);
    e22_hal_work_mode(WORK_MODE_TRANSPARENT);
}

void e22_command_test(uint8_t *com,uint16_t length)
{
    e22_hal_work_mode(WORK_MODE_CONFIG);
    e22_hal_uart_tx(com,length);
}

void host_tx_packge(MODE_SET_STU smode,u8 packnum)
{
    if(slave_rx_packnum == packnum)
    {
        flag_tx_start = 0;
        time_txcheck = 0;
    }
    if(flag_tx_start)
    {
        if(time_txcheck == 0 && flag_check_timeout == 0 && smode.enter_step < 6)
        {
            time_txcheck = E22_ACK_TIMEOUT + 500;
            e22_txdata[3] = 0x11;
            //Display_String(start_x+7*2,smode.enter_step*ycoe+m_y,"               ",15);
            e22_txdata[1] = smode.enter_step;
            e22_txdata[2] = user_host_config[smode.enter_step-1].channel; //channel
            e22_txdata[4] = smode.host_id;  //Host ID
            e22_hal_uart_tx(e22_txdata,sizeof(e22_txdata));
            e22_acktime_start(smode.enter_step);
        }
    }
    if(flag_slave_rx)
    {
        flag_slave_rx = 0;
        Display_refresh(start_x+7*2,m_y,rx_slave_id);
    }
}

u8 txnum = 0;
void host_tx_check_slave_sta(MODE_SET_STU smode)
{
    if(flag_tx_start)
    {
        if(smode.enter_step < 6)
        {
            //e22_user_config_init(smode);
            if(flag_tx_open || flag_tx_stop || flag_tx_close)
            {
                flag_tx_open = 0;
                flag_tx_stop = 0;
                flag_tx_close = 0;
                e22_txdata[3] = 0xF1; //ctrl SET code
            }
            else
            {
                e22_txdata[3] = 0x11;
                Display_String(start_x+7*2,smode.enter_step*ycoe+m_y,"               ",15);
            }
            /* HOST TX: SlaveIDaddrH, SlaveIDaddrL, FreqChannel, set/check State, HostID, State */
            e22_txdata[1] = smode.enter_step;
            e22_txdata[2] = user_host_config[smode.enter_step-1].channel; //channel
            e22_txdata[4] = smode.host_id;  //Host ID
            e22_hal_uart_tx(e22_txdata,sizeof(e22_txdata));
            e22_acktime_start(smode.enter_step);
            flag_tx_start = 0;
            time_txcheck = 0;
            txnum = 0;
        }
        else
        {
            if(time_txcheck == 0 && txnum < 5 && flag_check_timeout == 0)
            {
                time_txcheck = E22_ACK_TIMEOUT + 500;
                txnum++;
                if(flag_tx_open || flag_tx_stop || flag_tx_close)
                {
                    e22_txdata[3] = 0xF1;
                }
                else e22_txdata[3] = 0x11;
                e22_txdata[1] = txnum;  //slave addr
                e22_txdata[2] = user_host_config[txnum-1].channel; //channel
                e22_txdata[4] = smode.host_id;  //slave number
                e22_hal_uart_tx(e22_txdata,sizeof(e22_txdata));
                e22_acktime_start(txnum);
                Display_String(start_x+7*2,txnum*ycoe+m_y,"               ",15); //clear display data
                if(txnum >= 5)
                {
                    txnum = 0;
                    time_txcheck = 0;
                    flag_tx_start = 0;
                    flag_tx_open = 0;
                    flag_tx_stop = 0;
                    flag_tx_close = 0;
                }
            }
        }
    }
    if(flag_slave_rx)
    {
        flag_slave_rx = 0;
        Display_refresh(start_x+7*2,m_y,rx_slave_id);
    }
}

void slave_tx_sta(u8 num)
{
    if(flag_slave_rx)
    {
        if(mode_sel.Mode_Set == NORMAL_MODE && time_slave_ack != 0) //一般模式需要延时300ms发送,中继模式不需要延时。
        {}
        else
        {
            flag_slave_rx = 0;
            e22_txdata[1] = slave_rx_hostid; //host addr
            e22_txdata[2] = user_slave_config[num].channel; //tx channel
            e22_txdata[3] = 0x11; //data head
            e22_txdata[4] = num;  //slave id
            if(key_sta == Key_Open) e22_txdata[5] = 0x08;
            else if(key_sta == Key_Stop) e22_txdata[5] = 0x04;
            else e22_txdata[5] = 0x02;
            e22_txdata[6] = slave_rx_packnum;//slave_rx_rssi;
            e22_hal_uart_tx(e22_txdata,7);
        }
    }
}

void e22_acktime_start(u8 tstep)
{
    flag_rx_time = 1;
    SlaNumStu[tstep].oldtime = time_rxack;
    step_tx_timeout = tstep;
    flag_check_timeout = 1;
}

void e22_check_ack_timeout(void)
{
    if(flag_check_timeout && (time_rxack - SlaNumStu[step_tx_timeout].oldtime) > E22_ACK_TIMEOUT)
    {
        flag_check_timeout = 0;
        if(e22_txdata[3] == 0x11)
        {
            Display_String(start_x+7*2,step_tx_timeout*ycoe+m_y,"               ",15);
            Display_String(start_x+7*2+6*7,step_tx_timeout*ycoe+m_y,"Time Out",8);
        }
    }
}

void E22_Data_Check(unsigned char *buffer,unsigned long length)
{
    u8 i = 0;
    if(mode_sel.Mode_Type == HOST_TYPE)
    {
        if(mode_sel.Mode_Set == NORMAL_MODE && buffer[0] == 0x11 && buffer[1] <= 5)
        {
            flag_check_timeout = 0;
            SlaNumStu[buffer[1]].num = buffer[1];
            SlaNumStu[buffer[1]].sta = buffer[2];
            //SlaNumStu[buffer[1]].slave_rssi = buffer[3];
            slave_rx_packnum = buffer[3];
            SlaNumStu[buffer[1]].rssi = buffer[4];
            SlaNumStu[buffer[1]].acktime = time_rxack - SlaNumStu[buffer[1]].oldtime;
            flag_slave_rx = 1;
            rx_slave_id = SlaNumStu[buffer[1]].num;
            beep_led_on();
        }
        else if(mode_sel.Mode_Set == RELAY_MODE && buffer[1] == 'O' && buffer[2] == 'K')
        {
            //beep_led_on();
        }
    }
    else
    {
        if(buffer[0] == 0x11)
        {
            for(i = 3; i < 97; i++) //i==97:The last byte is RSSI.
            {
                if(buffer[i] != 0x15) return;
            }
            slave_rx_hostid = buffer[1];
            slave_rx_rssi = buffer[sizeof(e22_txdata) - 3];
            flag_slave_rx = 1;
            time_slave_ack = 600;    //NORMAL_MODE:slave delay ack to prevent conflicts with relays.RELAY_MODE:no delay.
            beep_led_on();
            slave_rx_packnum++;
            if(slave_rx_packnum > 100) slave_rx_packnum = 1;
        }
        else if(buffer[0] == 0xF1)
        {
            slave_rx_hostid = buffer[1];
            beep_led_on();
            if(buffer[2] == 0x08)
            {
                key_sta = Key_Open;
                Receiver_OUT_OPEN = 1;
                Receiver_OUT_STOP = 0;
                Receiver_OUT_CLOSE = 0;
            }
            else if(buffer[2] == 0x04)
            {
                key_sta = Key_Stop;
                Receiver_OUT_OPEN = 0;
                Receiver_OUT_STOP = 1;
                Receiver_OUT_CLOSE = 0;
            }
            else if(buffer[2] == 0x02)
            {
                key_sta = Key_Close;
                Receiver_OUT_OPEN = 0;
                Receiver_OUT_STOP = 0;
                Receiver_OUT_CLOSE = 1;
            }
        }
    }
}

void beep_led_on(void)
{
    _ReqBuzzer(400,1,1);
    Receiver_LED_RX = 1;
    Receiver_LED_OUT = 1;
    time_sw = 500;
    flag_led = 1;
}

void tx_state(void)
{
    if(flag_tx_stop)
    {
        flag_tx_stop = 0;
        e22_txdata[2] = 0x04;
        e22_hal_uart_tx(e22_txdata,5);
    }
    if(flag_tx_open)
    {
        flag_tx_open = 0;
        e22_txdata[2] = 0x08;
        e22_hal_uart_tx(e22_txdata,5);
    }
    if(flag_tx_close)
    {
        flag_tx_close = 0;
        e22_txdata[2] = 0x02;
        e22_hal_uart_tx(e22_txdata,5);
    }
}

static void e22_send_config_command(const e22_register_t *config)
{
	/* 第1字节：控制指令
	   C0代表写入参数并保存 */
	e22_buffer.hex_cmd.command = 0xC0;

	/* 第2字节：起始地址
	   00代表从寄存器地址0开始写入 */
	e22_buffer.hex_cmd.register_start = 0x00;

	/* 第3字节：写入数量 */
	e22_buffer.hex_cmd.register_number = sizeof(e22_register_t);

	/* 第4字节开始就是用户配置参数了 */
	memcpy( e22_buffer.hex_cmd.config , (uint8_t*)config , sizeof(e22_register_t));

	/* 串口写入 */
	e22_hal_uart_tx( (uint8_t*)&e22_buffer, sizeof(e22_register_t) + 3);
}
u8 ch = 0;
void e22_send_userconfig(user_config_t *config)
{
    uint16_t opt_length;
    e22_register_t register_write;

    /* 复制默认寄存器配置 */
	memcpy( (uint8_t*)&register_write , (uint8_t*)&register_default ,  sizeof(e22_register_t) );
    register_write.register_6.field.router_mode = config->router_mode;
    register_write.register_0.address_h = config->modu_addr_h;
    register_write.register_1.address_l = config->modu_addr_l;
    register_write.register_2.network_id = config->network_id;
    register_write.register_3.field.radio_rate = config->rate_mode;
    register_write.register_4.field.tx_power = config->tx_power;
    register_write.register_5.channel = config->channel;
    /* 切换到配置模式 */
	e22_hal_work_mode( WORK_MODE_CONFIG );
    /* 向模组串口写入配置 */
	e22_send_config_command( &register_write );

	/* 等待模组应答 */
	uart1_wait_response_blocked( e22_buffer.opt_buffer , &opt_length );
     ch = e22_buffer.opt_buffer[0];
	/* 应答数据检查 指令头必为C1 */
	if( e22_buffer.opt_buffer[0] != 0xC1 )
	{
			/* 模组如果回答FF FF FF 表示指令错误 */
			while(1);
	}
    /* 回到透明传输模式 */
	e22_hal_work_mode( WORK_MODE_TRANSPARENT );
}

void E22_Test_Mode(void)
{
    while(PIN_TestIN == 0)
    {
        ClearWDT();
        if(key_sta == Key_Open && flag_tx_carr == 0)
        {
            flag_tx_carr = 1;
            Receiver_LED_TX = 1;
            e22_test_mode(1);
            flag_tx_modu = 0;
        }
        else if(key_sta == Key_Close && flag_tx_modu == 0)
        {
            flag_tx_modu = 1;
            Receiver_LED_TX = 1;
            e22_test_mode(2);
            flag_tx_carr = 0;
        }
        else if(key_sta == Key_Stop && (flag_tx_carr==1 || flag_tx_modu==1))
        {
            flag_tx_carr = 0;
            flag_tx_modu = 0;
            Receiver_LED_TX = 0;
            e22_test_mode(0);
        }
    }
}

void e22_test_mode(uint8_t mode)
{
    switch(mode)
    {
        case 1:
            e22_command_test(tx_carrier1,strlen(tx_carrier1));
            break;

        case 2:
            e22_command_test(tx_modul1,strlen(tx_modul1));
            break;

        case 0:
            e22_command_test(tx_carrier0,strlen(tx_carrier0));
            e22_command_test(tx_modul0,strlen(tx_modul0));
            break;
    }
}

void e22_hal_aux_wait(void)
{
    if(E22_AUX_IN == 0)
	{
        /* 先等模块空闲 AUX引脚低为忙、高为空闲 */
        time_sw = 50;
        while(E22_AUX_IN == 0 )
        {
            ;//if(time_sw == 0) break;
        }
        /* 考虑兼容性，AUX空闲后再稍等1~2ms */
        mDelaymS(2);
	}
}

void e22_hal_reset(void)
{
    E22_RST = 0;
    mDelaymS(2);
    E22_RST = 1;
    mDelaymS(10);
    e22_hal_aux_wait();
}

void e22_hal_work_mode(work_mode_t mode)
{
    e22_hal_aux_wait();

    switch(mode)
	{
		/* 模式0： 透传模式 (M0=0 M1=0) */
		case WORK_MODE_TRANSPARENT:
            E22_M0 = 0;
            E22_M1 = 0;
            break;

        /* 模式1： 空中唤醒模式 (M0=1 M1=0) */
		case WORK_MODE_WAKE_ON_RADIO:
            E22_M0 = 1;
            E22_M1 = 0;
            break;

        /* 模式2： 配置模式 (M0=0 M1=1) */
		case WORK_MODE_CONFIG:
            E22_M0 = 0;
            E22_M1 = 1;
            break;

        /* 模式3： 休眠模式 (M0=1 M1=1) */
		case WORK_MODE_SLEEP:
            E22_M0 = 1;
            E22_M1 = 1;
            break;

        default://mode 0
            E22_M0 = 0;
            E22_M1 = 0;
            break;
    }
    /* 引脚切换后 模组有时AUX引脚并不是立即输出低电平，需要稍等再进行检查 */
	mDelaymS(5);
	e22_hal_aux_wait();
}




