#include "system.h"
#include "si446x_defs.h"
//#include "si446x_patch.h"
//#include "radio_config.h"
#include "radio_config_Si4438.h"


//#define RF_COMM_PORT		GPIOA
//#define RF_COMM_NSEL		GPIO_Pin_4
//#define RF_COMM_SCK			GPIO_Pin_5
//#define RF_COMM_SDO			GPIO_Pin_6
//#define RF_COMM_SDI			GPIO_Pin_7
//#define RF_CTRL_PORT		GPIOB
//#define RF_CTRL_NIRQ		GPIO_Pin_2
//#define RF_CTRL_SDN			GPIO_Pin_11
//#define RF_CTRL_GPIO1		GPIO_Pin_10
//#define RF_POWER_ON			GPIO_ResetBits(RF_CTRL_PORT,RF_CTRL_SDN)
//#define RF_POWER_OFF		GPIO_SetBits(RF_CTRL_PORT,RF_CTRL_SDN)

#define		RF_PORT_GPIO1		GPIOB
#define		RF_PIN_GPIO1		GPIO_Pin_0
#define		RF_PORT_SCK		GPIOC
#define		RF_PIN_SCK		GPIO_Pin_5
#define		RF_PORT_SDO		GPIOC
#define		RF_PIN_SDO		GPIO_Pin_4
#define		RF_PORT_SDI		GPIOA
#define		RF_PIN_SDI		GPIO_Pin_7
#define		RF_PORT_SEL		GPIOA
#define		RF_PIN_SEL		GPIO_Pin_6
#define		RF_PORT_GPIO3		GPIOA
#define		RF_PIN_GPIO3		GPIO_Pin_5
#define		RF_PORT_SDN		GPIOA
#define		RF_PIN_SDN		GPIO_Pin_4

#define SET_SDI_LOW 	GPIO_ResetBits	(RF_PORT_SDI, RF_PIN_SDI)
#define SET_SDI_HIGH 	GPIO_SetBits	(RF_PORT_SDI, RF_PIN_SDI)
#define SET_SCK_LOW 	GPIO_ResetBits	(RF_PORT_SCK, RF_PIN_SCK)
#define SET_SCK_HIGH 	GPIO_SetBits	(RF_PORT_SCK, RF_PIN_SCK)
#define SET_SEL_LOW 	GPIO_ResetBits	(RF_PORT_SEL, RF_PIN_SEL)
#define SET_SEL_HIGH 	GPIO_SetBits	(RF_PORT_SEL, RF_PIN_SEL)
#define GET_SDO_STATE()	GPIO_ReadInputDataBit(RF_PORT_SDO, RF_PIN_SDO)
#define GET_IRQ_STATE()	GPIO_ReadInputDataBit(RF_PORT_IRQ, RF_PIN_IRQ)
#define SET_SDN_LOW 	GPIO_ResetBits	(RF_PORT_SDN, RF_PIN_SDN)
#define SET_SDN_HIGH 	GPIO_SetBits	(RF_PORT_SDN, RF_PIN_SDN)

#define GET_GPIO3_STATE()	GPIO_ReadInputDataBit(RF_PORT_GPIO3, RF_PIN_GPIO3)
#define GET_GPIO1_STATE()	GPIO_ReadInputDataBit(RF_PORT_GPIO1, RF_PIN_GPIO1)
/*****************************************************************************
 *  Global Typedefs & Enums
 *****************************************************************************/
#define SI466X_FIFO_SIZE 64

typedef enum{
	NOBYTE	= (byte)0x00,
	YESBYTE = !(byte)0x01,
}bool_typ; 


enum
{
    SI446X_SUCCESS,
    SI446X_NO_PATCH,
    SI446X_CTS_TIMEOUT,
    SI446X_PATCH_FAIL,
    SI446X_COMMAND_ERROR
};

typedef struct
{
    const uint8_t   *Radio_ConfigurationArray;

    uint8_t   Radio_ChannelNumber;
    uint8_t   Radio_PacketLength;
    uint8_t   Radio_State_After_Power_Up;

    uint16_t  Radio_Delay_Cnt_After_Reset;
    uint8_t   Radio_CustomPayload[RADIO_MAX_PACKET_LENGTH];
} tRadioConfiguration;

#define RADIO_CTS_TIMEOUT 300

#define Frame_Head_LEN		4
#define Frame_Head				(0xAA55AA55)
const byte FrameHead[]={0xaa,0x55,0xaa,0x55};
/*****************************************************************************
 *  Global Variables
 *****************************************************************************/
const uint8_t Radio_Configuration_Data_Array[] =  RADIO_CONFIGURATION_DATA_ARRAY;

const tRadioConfiguration RadioConfiguration = RADIO_CONFIGURATION_DATA;

const tRadioConfiguration *pRadioConfiguration = &RadioConfiguration;

static union si446x_cmd_reply_union Si446xCmd;
static byte ctsWentHigh = 0;
static byte s_ucPro2Cmd[16];

static byte RadioPacket[SI466X_FIFO_SIZE];
static byte* pValidPacket;

/***********************************
***********************************/
static ushort yc_crc_cal(byte *data_to_cal, byte len)
{
	int i =0;
	ushort crc_value= 0xffff;
	
	while(len--)
	{
		crc_value ^= *data_to_cal++;
		for(i=0;i<8;i++)
		{
			if(crc_value&0x0001)
				crc_value = (crc_value>>1)^0xa001;
			else
				crc_value = crc_value>>1;
		}
	}
	return(crc_value);
}	


/*******************************************************************************
* 文件名	: Spi_ReadWriteSpi
* 描述    	: SPI读写位操作
* 输入		: biDataIn
* 输出		: 无
* 返回值	: 无
*******************************************************************************/
static byte Spi_ReadWriteSpi(byte biDataIn)//PR008B1：实测时钟周期=100K 2014.7.8
{
	volatile byte rl = 0, i;

	SET_SCK_LOW;
	for(i = 0; i < 8; i ++)
	{
		if((biDataIn & 0x80) > 0) //先发送高位
			SET_SDI_HIGH;
		else
			SET_SDI_LOW;
		
		DelayNull(10);
		SET_SCK_HIGH;
		DelayNull(10);
		biDataIn <<= 1;

		rl <<= 1;
		if(GET_SDO_STATE())
			rl += 1;

		SET_SCK_LOW;
	}
	SET_SCK_LOW;

	return rl;
}
/*******************************************************************************
* 文件名	: Spi_WriteDataSpi
* 描述    	: SPI写数据
* 输入		: biDataInLength，pabiDataIn
* 输出		: 无
* 返回值	: 无
*******************************************************************************/
static void Spi_WriteDataSpi(byte biDataInLength, byte *pabiDataIn)
{
  while (biDataInLength--) 
  {
    Spi_ReadWriteSpi(*pabiDataIn++);

  }
}


/*******************************************************************************
* 文件名	: Spi_ReadDataSpi
* 描述    	: SPI读数据
* 输入		: biDataOutLength，paboDataOut
* 输出		: 无
* 返回值	: 无
*******************************************************************************/
static void Spi_ReadDataSpi(byte biDataOutLength, byte *paboDataOut)
{
 
  while (biDataOutLength--) 
  {
    *paboDataOut++ = Spi_ReadWriteSpi(0xFF);

  }
}
/*******************************************************************************
* 文件名	: radio_comm_GetResp
* 描述    	: 获取命令返回值
* 输入		: 无
* 输出		: 无
* 返回值	: CTS value
*******************************************************************************/
static byte radio_comm_GetResp(byte byteCount, byte* pData)
{
  byte ctsVal = 0u;
  ushort errCnt = RADIO_CTS_TIMEOUT;

  //实测循环RADIO_CTS_TIMEOUT次花费时间, RP008B1实测最长循环时间~50ms 2014.7.9	
  //POWER UP命令等待时间最长约14ms  It takes approximately 14ms to complete the command (boot time)
  while (errCnt != 0)      //wait until radio IC is ready with the data
  {
		DelayMsNull(1);
    SET_SEL_LOW;
//    radio_hal_SpiWriteByte(0x44);    //read CMD buffer
    Spi_ReadWriteSpi(0x44);    //read CMD buffer
//    ctsVal = radio_hal_SpiReadByte();
    ctsVal = Spi_ReadWriteSpi(0xFF);;
    if (ctsVal == 0xFF)
    {
      if(byteCount)
      {
//        radio_hal_SpiReadData(byteCount, pData);
        Spi_ReadDataSpi(byteCount, pData);
      }
//      radio_hal_SetNsel();
        SET_SEL_HIGH;
      break;
    }
//    radio_hal_SetNsel();
    SET_SEL_HIGH;
    errCnt--;
  }
  if (ctsVal == 0xFF)
  {
    ctsWentHigh = 1;
  }

  return ctsVal;
}

/*******************************************************************************
* 文件名	: radio_comm_PollCTS
* 描述    	: Waits for CTS to be high
* 输入		: 无
* 输出		: 无
* 返回值	: CTS value
*******************************************************************************/
static byte radio_comm_PollCTS(void)
{
#ifdef RADIO_USER_CFG_USE_GPIO1_FOR_CTS  //使用GPIO口来判断
    while(!radio_hal_Gpio1Level())
    {
        /* Wait...*/
    }
    ctsWentHigh = 1;
    return 0xFF;
#else
    return radio_comm_GetResp(0, 0);
#endif
} 
/*******************************************************************************
* 文件名	: radio_comm_SendCmd
* 描述    	: 发送命令
* 输入		: 无
* 输出		: 无
* 返回值	: 无
*******************************************************************************/
static void radio_comm_SendCmd(byte byteCount, byte* pData)
{
    while (!ctsWentHigh)
    {
        radio_comm_PollCTS();
    }
//    radio_hal_ClearNsel();
		SET_SEL_LOW;
//    radio_hal_SpiWriteData(byteCount, pData);
		Spi_WriteDataSpi(byteCount, pData);
//    radio_hal_SetNsel();
		SET_SEL_HIGH;
    ctsWentHigh = 0;
}

/*******************************************************************************
* 文件名	: radio_comm_SendCmdGetResp
* 描述    	: 发送命令并返回命令响应值
* 输入		: 无
* 输出		: 无
* 返回值	: 无
*******************************************************************************/
static byte radio_comm_SendCmdGetResp(byte cmdByteCount, byte* pCmdData, byte respByteCount, byte* pRespData)
{
    radio_comm_SendCmd(cmdByteCount, pCmdData);
    return radio_comm_GetResp(respByteCount, pRespData);
}

 
/*******************************************************************************
* 文件名	: radio_comm_ReadData
* 描述    	: 读数据 Gets a command response from the radio chip
* 输入		:
			* @param cmd           Command ID
			* @param pollCts       Set to poll CTS
			* @param byteCount     Number of bytes to get from the radio chip.
			* @param pData         Pointer to where to put the data.
* 输出		: 无
* 返回值	: 无
*******************************************************************************/
static void radio_comm_ReadData(byte cmd, bool_typ pollCts, byte byteCount, byte* pData)
{
    if(pollCts)
    {
        while(!ctsWentHigh)
        {
            radio_comm_PollCTS();
        }
    }
//    radio_hal_ClearNsel();
		SET_SEL_LOW;
//    radio_hal_SpiWriteByte(cmd);    //read CMD buffer
    Spi_ReadWriteSpi(cmd);    //read CMD buffer
//        radio_hal_SpiReadData(byteCount, pData);
        Spi_ReadDataSpi(byteCount, pData);
//    radio_hal_SetNsel();
		SET_SEL_HIGH;
    ctsWentHigh = 0;
}


 
/*******************************************************************************
* 文件名	: radio_comm_WriteData
* 描述    	: 读数据 Gets a command response from the radio chip
* 输入		:
			* @param cmd           Command ID
			* @param pollCts       Set to poll CTS
			* @param byteCount     Number of bytes to get from the radio chip
			* @param pData         Pointer to where to put the data
* 返回值	: 无
*******************************************************************************/
static void radio_comm_WriteData(byte cmd, bool_typ pollCts, byte byteCount, byte* pData)
{
    if(pollCts)
    {
        while(!ctsWentHigh)
        {
            radio_comm_PollCTS();
        }
    }
//    radio_hal_ClearNsel();
		SET_SEL_LOW;
//    radio_hal_SpiWriteByte(cmd);    //read CMD buffer
    Spi_ReadWriteSpi(cmd);    //read CMD buffer
//    radio_hal_SpiWriteData(byteCount, pData);
    Spi_WriteDataSpi(byteCount, pData);
//    radio_hal_SetNsel();
		SET_SEL_HIGH;
    ctsWentHigh = 0;
}

/*!
 * This functions is used to reset the si446x radio by applying shutdown and
 * releasing it.  After this function @ref si446x_boot should be called.  You
 * can check if POR has completed by waiting 4 ms or by polling GPIO 0, 2, or 3.
 * When these GPIOs are high, it is safe to call @ref si446x_boot.
 */
static void si446x_reset(void) //延时时间需要实测
{
    //u16 loopCount;

    /* Put radio in shutdown, wait then release 
    radio_hal_AssertShutdown();*/
	SET_SDN_HIGH;

    //! @todo this needs to be a better delay function.
    //for (loopCount = 1000; loopCount != 0; loopCount--); // Wait ~300us (SDN pulse width)  		实测PR008B1:2ms 2014.7.9
	DelayMsNull(7);
//    radio_hal_DeassertShutdown();
	SET_SDN_LOW;
    //for (loopCount = 3500; loopCount != 0; loopCount--); //Wait for POR (power on reset); ~5ms  实测PR008B1:7ms 2014.7.9
	DelayMsNull(7);
//    radio_comm_ClearCTS();
	ctsWentHigh = 0;

}

/*!
 * This function is used to initialize after power-up the radio chip.
 * Before this function @si446x_reset should be called.
static void si446x_power_up(byte BOOT_OPTIONS, byte XTAL_OPTIONS, uint XO_FREQ)
{
    s_ucPro2Cmd[0] = SI446X_CMD_ID_POWER_UP;
    s_ucPro2Cmd[1] = BOOT_OPTIONS;
    s_ucPro2Cmd[2] = XTAL_OPTIONS;
    s_ucPro2Cmd[3] = (byte)(XO_FREQ >> 24);
    s_ucPro2Cmd[4] = (byte)(XO_FREQ >> 16);
    s_ucPro2Cmd[5] = (byte)(XO_FREQ >> 8);
    s_ucPro2Cmd[6] = (byte)(XO_FREQ);

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_POWER_UP, s_ucPro2Cmd );
}
 */
/*! Sends START_TX command to the radio.
 *
 * @param CHANNEL   Channel number.
 * @param CONDITION Start TX condition.
 * @param TX_LEN    Payload length (exclude the PH generated CRC).
 */
static void si446x_start_tx(byte CHANNEL, byte CONDITION, ushort TX_LEN)
{
    s_ucPro2Cmd[0] = SI446X_CMD_ID_START_TX;
    s_ucPro2Cmd[1] = CHANNEL;
    s_ucPro2Cmd[2] = CONDITION;
    s_ucPro2Cmd[3] = (byte)(TX_LEN >> 8);
    s_ucPro2Cmd[4] = (byte)(TX_LEN);
    s_ucPro2Cmd[5] = 0x00;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_TX, s_ucPro2Cmd );
}

/*!
 * Sends START_RX command to the radio.
 *
 * @param CHANNEL     Channel number.
 * @param CONDITION   Start RX condition.
 * @param RX_LEN      Payload length (exclude the PH generated CRC).
 * @param NEXT_STATE1 Next state when Preamble Timeout occurs.
 * @param NEXT_STATE2 Next state when a valid packet received.
 * @param NEXT_STATE3 Next state when invalid packet received (e.g. CRC error).
 */
static void si446x_start_rx(byte CHANNEL, byte CONDITION, ushort RX_LEN, byte NEXT_STATE1, byte NEXT_STATE2, byte NEXT_STATE3)
{
    s_ucPro2Cmd[0] = SI446X_CMD_ID_START_RX;
    s_ucPro2Cmd[1] = CHANNEL;
    s_ucPro2Cmd[2] = CONDITION;
    s_ucPro2Cmd[3] = (byte)(RX_LEN >> 8);
    s_ucPro2Cmd[4] = (byte)(RX_LEN);
    s_ucPro2Cmd[5] = NEXT_STATE1;
    s_ucPro2Cmd[6] = NEXT_STATE2;
    s_ucPro2Cmd[7] = NEXT_STATE3;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_START_RX, s_ucPro2Cmd );
}

/*!
 * Get the Interrupt status/pending flags form the radio and clear flags if requested.
 *
 * @param PH_CLR_PEND     Packet Handler pending flags clear.
 * @param MODEM_CLR_PEND  Modem Status pending flags clear.
 * @param CHIP_CLR_PEND   Chip State pending flags clear.
 */
static void si446x_get_int_status(byte PH_CLR_PEND, byte MODEM_CLR_PEND, byte CHIP_CLR_PEND)
{
    s_ucPro2Cmd[0] = SI446X_CMD_ID_GET_INT_STATUS;
    s_ucPro2Cmd[1] = PH_CLR_PEND;
    s_ucPro2Cmd[2] = MODEM_CLR_PEND;
    s_ucPro2Cmd[3] = CHIP_CLR_PEND;

    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_GET_INT_STATUS,
                              s_ucPro2Cmd,
                              SI446X_CMD_REPLY_COUNT_GET_INT_STATUS,
                              s_ucPro2Cmd );

    Si446xCmd.GET_INT_STATUS.INT_PEND       = s_ucPro2Cmd[0];
    Si446xCmd.GET_INT_STATUS.INT_STATUS     = s_ucPro2Cmd[1];
    Si446xCmd.GET_INT_STATUS.PH_PEND        = s_ucPro2Cmd[2];
    Si446xCmd.GET_INT_STATUS.PH_STATUS      = s_ucPro2Cmd[3];
    Si446xCmd.GET_INT_STATUS.MODEM_PEND     = s_ucPro2Cmd[4];
    Si446xCmd.GET_INT_STATUS.MODEM_STATUS   = s_ucPro2Cmd[5];
    Si446xCmd.GET_INT_STATUS.CHIP_PEND      = s_ucPro2Cmd[6];
    Si446xCmd.GET_INT_STATUS.CHIP_STATUS    = s_ucPro2Cmd[7];
}

/*!
 * Issue a change state command to the radio.
 *
 * @param NEXT_STATE1 Next state.
 */
static void si446x_change_state(byte NEXT_STATE1)
{
    s_ucPro2Cmd[0] = SI446X_CMD_ID_CHANGE_STATE;
    s_ucPro2Cmd[1] = NEXT_STATE1;

    radio_comm_SendCmd( SI446X_CMD_ARG_COUNT_CHANGE_STATE, s_ucPro2Cmd );
}

/*!
 * Send the FIFO_INFO command to the radio. Optionally resets the TX/RX FIFO. Reads the radio response back
 * into @Si446xCmd.
 *
 * @param FIFO  RX/TX FIFO reset flags.
 */
static void si446x_fifo_info(byte FIFO)
{
    s_ucPro2Cmd[0] = SI446X_CMD_ID_FIFO_INFO;
    s_ucPro2Cmd[1] = FIFO;

    radio_comm_SendCmdGetResp( SI446X_CMD_ARG_COUNT_FIFO_INFO,
                              s_ucPro2Cmd,
                              SI446X_CMD_REPLY_COUNT_FIFO_INFO,
                              s_ucPro2Cmd );

    Si446xCmd.FIFO_INFO.RX_FIFO_COUNT   = s_ucPro2Cmd[0];
    Si446xCmd.FIFO_INFO.TX_FIFO_SPACE   = s_ucPro2Cmd[1];
}


/*!
 * The function can be used to load data into TX FIFO.
 *
 * @param numBytes  Data length to be load.
 * @param pTxData   Pointer to the data (byte*).
 */
static void si446x_write_tx_fifo(byte numBytes, byte* pTxData)
{
  radio_comm_WriteData( SI446X_CMD_ID_WRITE_TX_FIFO, (bool_typ)0, numBytes, pTxData );
}

/*!
 * Reads the RX FIFO content from the radio.
 *
 * @param numBytes  Data length to be read.
 * @param pRxData   Pointer to the buffer location.
 */
static void si446x_read_rx_fifo(byte numBytes, byte* pRxData)
{
  radio_comm_ReadData( SI446X_CMD_ID_READ_RX_FIFO, (bool_typ)0, numBytes, pRxData );
}

/*!
 * This function is used to load all properties and commands with a list of NULL terminated commands.
 * Before this function @si446x_reset should be called.
 */
static byte si446x_configuration_init(const byte* pSetPropCmd)
{

	byte col;
	byte numOfBytes;

  /* While cycle as far as the pointer points to a command */
  while (*pSetPropCmd != 0x00)
  {
    /* Commands structure in the array:
     * --------------------------------
     * LEN | <LEN length of data>
     */

    numOfBytes = *pSetPropCmd++;

    if (numOfBytes > 16u)
    {
      /* Number of command bytes exceeds maximal allowable length */
      return SI446X_COMMAND_ERROR;
    }

    for (col = 0u; col < numOfBytes; col++)
    {
      s_ucPro2Cmd[col] = *pSetPropCmd;
      pSetPropCmd++;
    }

    if (radio_comm_SendCmdGetResp(numOfBytes, s_ucPro2Cmd, 0, 0) != 0xFF)
    {
      /* Timeout occured */
      return SI446X_CTS_TIMEOUT;
    }

    if (GET_SDO_STATE() == 0)
    {
      /* Get and clear all interrupts.  An error has occured... */
      si446x_get_int_status(0, 0, 0);
      if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_MASK)
      {
        return SI446X_COMMAND_ERROR;
      }
    }
  }

  return SI446X_SUCCESS;
}



static void vRadio_Init(byte *pCustomPacked)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
  uint16_t wDelay;

  /* Power Up the radio chip
  vRadio_PowerUp(); */
  /* Hardware reset the chip */
  si446x_reset();

  /* Wait until reset timeout or Reset IT signal */
  for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);

  /* Load radio configuration */
  while (SI446X_SUCCESS != si446x_configuration_init(pRadioConfiguration->Radio_ConfigurationArray))
  {
    for (wDelay = 0x7FFF; wDelay--; ) ;
    /* Power Up the radio chip 
    vRadio_PowerUp();*/
		/* Hardware reset the chip */
		si446x_reset();

		/* Wait until reset timeout or Reset IT signal */
		for (; wDelay < pRadioConfiguration->Radio_Delay_Cnt_After_Reset; wDelay++);
  }

  // Read ITs, clear pending ones
//  si446x_get_int_status(0xff, 0xff, 0xff);
  si446x_get_int_status(0u, 0u, 0u);
	
  
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource5);
  
  /* Enable and set EXTI0 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority  = 0x02;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable EXTIx to detect the start bit of the IR frame */
  EXTI_ClearITPendingBit(EXTI_Line5);
  EXTI_InitStructure.EXTI_Line = EXTI_Line5;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	
	pValidPacket = pCustomPacked;
	
}

static byte CrcCheckIsErr(void)
{
	byte i;
	ushort crc_val;
	
	for(i=0;i<Frame_Head_LEN;i++)
	{
		if(RadioPacket[i]==FrameHead[i])
			continue;
		else
			break;
	}
	if(i<Frame_Head_LEN)
		return FALSE;
	
	i= RadioPacket[Frame_Head_LEN];
	crc_val = RadioPacket[Frame_Head_LEN+i];
	crc_val <<=8;
	crc_val |= RadioPacket[Frame_Head_LEN+i+1];	
	if(crc_val==yc_crc_cal(&RadioPacket[Frame_Head_LEN],RadioPacket[Frame_Head_LEN]))
	{
		for(i=0;i<(RadioPacket[Frame_Head_LEN]-1);i++)
		   *(pValidPacket+i)= RadioPacket[Frame_Head_LEN+i+1];
		return TRUE;
	}
	else
		return FALSE;
	
}



/*!
 *  Set Radio to RX mode. .
 *
 *  @param channel Freq. Channel,  packetLength : 0 Packet handler fields are used , nonzero: only Field1 is used
 *
 *  @note
 *
 */
static void vRadio_StartRX(uint8_t channel,uint8_t packetLenght )
{
	byte i=0;
	
	packetLenght += (Frame_Head_LEN+2);
	
	for(i=0;i<SI466X_FIFO_SIZE;i++)
	  RadioPacket[i] =0;

  // Read ITs, clear pending ones
  si446x_get_int_status(0u, 0u, 0u);

   // Reset the Rx Fifo
   si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);

  /* Start Receiving packet, channel 0, START immediately, Packet length used or not according to packetLength */
  si446x_start_rx(pRadioConfiguration->Radio_ChannelNumber, 0u, packetLenght,
                  SI446X_CMD_START_RX_ARG_RXTIMEOUT_STATE_ENUM_NOCHANGE,
                  SI446X_CMD_START_RX_ARG_RXVALID_STATE_ENUM_READY,
                  SI446X_CMD_START_RX_ARG_RXINVALID_STATE_ENUM_RX );
}

/*!
 *  Set Radio to TX mode, variable packet length.
 *
 *  @param channel Freq. Channel, Packet to be sent length of of the packet sent to TXFIFO
 *
 *  @note
 *
 */
static void vRadio_StartTx_Variable_Packet(uint8_t channel, uint8_t *pioRadioPacket, uint8_t length)
{
	ushort crc_value=0;
	byte i=0;
	byte *pdata;
	
	pdata = &RadioPacket[0];
	for(i=0;i<Frame_Head_LEN;i++)
	{
		*pdata++ = FrameHead[i];
	}
	for(i=0;i<length;i++)
	{
		*pdata++ = *pioRadioPacket++;
	}
	i = Frame_Head_LEN;
	crc_value= yc_crc_cal(&RadioPacket[i],length);
	*pdata++= crc_value>>8;
	*pdata= crc_value&0xff;
	
	length += (Frame_Head_LEN+2);
	
  /* Leave RX state */
  si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_READY);

  /* Read ITs, clear pending ones */
  si446x_get_int_status(0u, 0u, 0u);

  /* Reset the Tx Fifo */
  si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_TX_BIT);

  /* Fill the TX fifo with datas */
//  si446x_write_tx_fifo(length, pioRadioPacket);
  si446x_write_tx_fifo(length, &RadioPacket[0]);

  /* Start sending packet, channel 0, START immediately */
   si446x_start_tx(pRadioConfiguration->Radio_ChannelNumber, 0x80, length);
 
}


/*!
 *  Check if Packet sent IT flag or Packet Received IT is pending.
 *
 *  @return   SI4455_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT / SI4455_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT
 *
 *  @note
 *
 */
uint8_t bRadio_Check_Tx_RX(void)
{
//	if(!GET_GPIO3_STATE())
//	{
      /* Read ITs, clear pending ones */
      si446x_get_int_status(0u, 0u, 0u);

	  if (Si446xCmd.GET_INT_STATUS.CHIP_PEND & SI446X_CMD_GET_CHIP_STATUS_REP_CMD_ERROR_PEND_BIT)
		{
      	/* State change to */
      	si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_SLEEP);
	
	  	/* Reset FIFO */
      	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
      
	  	/* State change to */
        si446x_change_state(SI446X_CMD_CHANGE_STATE_ARG_NEW_STATE_ENUM_RX);
    }

    if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT)
    {
			PostMessage(MessageRfCtrl, RfSendOff);
      return SI446X_CMD_GET_INT_STATUS_REP_PACKET_SENT_PEND_BIT;
    }

    if(Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT)
    {
      /* Packet RX */

			/* Get payload length */
      si446x_fifo_info(0x00);

			si446x_read_rx_fifo(Si446xCmd.FIFO_INFO.RX_FIFO_COUNT, &RadioPacket[0]);
			if(CrcCheckIsErr())
			{
				PostMessage(MessageRfRecv, RadioPacket[Frame_Head_LEN+1]);
			}
			else
				vRadio_StartRX(0,20);

      return SI446X_CMD_GET_INT_STATUS_REP_PACKET_RX_PEND_BIT;
    }
      
	  if (Si446xCmd.GET_INT_STATUS.PH_PEND & SI446X_CMD_GET_INT_STATUS_REP_CRC_ERROR_BIT)
    {
     	/* Reset FIFO */
     	si446x_fifo_info(SI446X_CMD_FIFO_INFO_ARG_RX_BIT);
    }

//  }

  return 0;
}   

void EXTI4_15_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)
	{
		bRadio_Check_Tx_RX();
    /* Clear the IR EXTI line pending bit */
    EXTI_ClearITPendingBit(EXTI_Line5);
	}
	
}

void Si4463_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC, ENABLE);
	
  GPIO_InitStruct.GPIO_Pin = RF_PIN_SEL  ;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_Speed =GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输出
  GPIO_Init( RF_PORT_SEL	, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin = RF_PIN_SCK  ;
  GPIO_Init( RF_PORT_SCK	, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin =  RF_PIN_SDI ;
  GPIO_Init( RF_PORT_SDI	, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Pin =  RF_PIN_SDN ;
  GPIO_Init( RF_PORT_SDN	, &GPIO_InitStruct);
	
		
	GPIO_InitStruct.GPIO_Pin = RF_PIN_GPIO3 ; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_UP; // 上拉输入
	GPIO_Init(RF_PORT_GPIO3, &GPIO_InitStruct);
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP; // 上拉输入
	GPIO_InitStruct.GPIO_Pin = RF_PIN_SDO; 
	GPIO_Init(RF_PORT_SDO, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = RF_PIN_GPIO1; 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL; // 上拉输入
	GPIO_Init(RF_PORT_GPIO1, &GPIO_InitStruct);
	GPIO_SetBits( RF_PORT_GPIO1	, RF_PIN_GPIO1 );


// RF4432_SEL=1;  
//	GPIO_SetBits(RF_PORT_SEL,RF_PIN_SEL ); 
//		// RF4432_SCLK=0;
//	GPIO_ResetBits(RF_PORT_SCK,RF_PIN_SCK ); 
//		 //  RF4432_SDN = 0;
//	GPIO_ResetBits(RF_PORT_SDN,RF_PIN_SDN ); 						 

//	GPIO_SetBits(RF_PORT_SDN,RF_PIN_SDN ); 
//	DelayMsNull(1);
//	GPIO_ResetBits(GPIOA,RF_PIN_SDN ); 
//	DelayMsNull(1);
	
	System.Device.RF.RfInit  =vRadio_Init;
	System.Device.RF.RfStartRX  =vRadio_StartRX;
	System.Device.RF.RfStartTX  =vRadio_StartTx_Variable_Packet;
	System.Device.RF.RfCheck  =bRadio_Check_Tx_RX;
}

