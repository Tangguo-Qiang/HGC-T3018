#include "remoteRF.h"

#define RF_CONFIG_WAIT 		5
#define RF_CONFIG_SLEEP     15
#define RF_QUERY_INTERVAL 10
#define RF_QUERY_LOST     15
#define RF_FIND_INTERVAL    30


static uint AddrTimer=0x01020304;

static uint8_t CustomRadioPacket[RADIO_MAX_PACKET_LENGTH];

static byte RfDelay=0;
//static byte RfQueryWait=0;
static byte RfCommLost=0;
static uint UnitAddr=0;
static uint RfState=RF_STATE_LOSE;

void RfRecvParse(byte code)
{
	byte index=0;
	uint addr;
	
	switch(code)
	{
		case RfSendId:   /* unit */
			if(App.SysState.RFState&RF_STATE_CONFIG)
			{
			System.Device.Beep.BeepOn(BEEP_LONG);
				UnitAddr = CustomRadioPacket[1];
				UnitAddr <<=8;
				UnitAddr |= CustomRadioPacket[2];
				UnitAddr <<= 8;
				UnitAddr |= CustomRadioPacket[3];
				UnitAddr <<= 8;
				UnitAddr |= CustomRadioPacket[4];
								
				index=0;
				CustomRadioPacket[index++]= 0;
				CustomRadioPacket[index++]= RfAnswerId;
				CustomRadioPacket[index++]= (UnitAddr>>24)&0xFF;
				CustomRadioPacket[index++]= (UnitAddr>>16)&0xFF;
				CustomRadioPacket[index++]= (UnitAddr>>8)&0xFF;
				CustomRadioPacket[index++]= UnitAddr&0xFF;
				
				RfCommLost = 0;
				CustomRadioPacket[0]= index;
				System.Device.RF.RfStartTX(0,&CustomRadioPacket[0],index);
			}
			break;
		case RfAnswerId:  /* dev */
			addr = CustomRadioPacket[1];
		  addr <<=8;
		  addr |= CustomRadioPacket[2];
		  addr <<= 8;
		  addr |= CustomRadioPacket[3];
		  addr <<= 8;
		  addr |= CustomRadioPacket[4];
		  if(addr == UnitAddr)
			{
			System.Device.Beep.BeepOn(BEEP_LONG);
				App.SysState.RFState =RF_STATE_CONNECT;
				RfCommLost = 0;
#ifdef __RF_UNIT
				if(!App.RfAddrID)
				{
					App.RfAddrID = UnitAddr;
					StorePost(STORE_RFCONNECT);
				}
				RfDelay = RF_QUERY_INTERVAL;
				System.Device.RF.RfStartRX(0,16);
#else				
				PostMessage(MessageRfCtrl, RfQuery);
#endif

			}
		  
			break;
		case RfQueryUnit:
			addr = CustomRadioPacket[1];
		  addr <<=8;
		  addr |= CustomRadioPacket[2];
		  addr <<= 8;
		  addr |= CustomRadioPacket[3];
		  addr <<= 8;
		  addr |= CustomRadioPacket[4];
		  if(addr == UnitAddr)
			{
				App.SysState.RFState |=RF_STATE_CONNECT;
				App.SysState.RFState &= ~RF_STATE_COMM;
				index=0;
				CustomRadioPacket[index++]= 0;
				CustomRadioPacket[index++]= RfUnitAnswer;
				CustomRadioPacket[index++]= (UnitAddr>>24)&0xFF;
				CustomRadioPacket[index++]= (UnitAddr>>16)&0xFF;
				CustomRadioPacket[index++]= (UnitAddr>>8)&0xFF;
				CustomRadioPacket[index++]= UnitAddr&0xFF;
				CustomRadioPacket[index++]=	App.SensorData.TempInside ;
				CustomRadioPacket[index++]=	App.SensorData.RHInside ;
				CustomRadioPacket[index++]=	App.SensorData.CO2Inside >>8;
				CustomRadioPacket[index++]=	App.SensorData.CO2Inside & 0xFF;
				CustomRadioPacket[index++]=	App.SensorData.PMInside>>8;
				CustomRadioPacket[index++]=	App.SensorData.PMInside&0xFF;
				
				CustomRadioPacket[0]= index;
				System.Device.RF.RfStartTX(0,&CustomRadioPacket[0],index);
		  }
			break;
		case RfUnitAnswer:		/*DEV*/
			addr = CustomRadioPacket[1];
		  addr <<=8;
		  addr |= CustomRadioPacket[2];
		  addr <<= 8;
		  addr |= CustomRadioPacket[3];
		  addr <<= 8;
		  addr |= CustomRadioPacket[4];
		  if(addr == UnitAddr)
			{
				App.SysState.RFState |= RF_STATE_CONNECT;
				App.SysState.RFState &= ~RF_STATE_COMM;
				App.SysState.RFState &= ~RF_STATE_SLEEP;
				if(!App.RfAddrID)
				{
					App.RfAddrID = UnitAddr;
					StorePost(STORE_RFCONNECT);
				}
				
				App.SensorData.TempInside = CustomRadioPacket[5];
				App.SensorData.RHInside = CustomRadioPacket[6];
				App.SensorData.CO2Inside = CustomRadioPacket[7];
				App.SensorData.CO2Inside <<= 8;
				App.SensorData.CO2Inside |= CustomRadioPacket[8];
				App.SensorData.PMInside = CustomRadioPacket[9];
				App.SensorData.PMInside <<= 8;
				App.SensorData.PMInside |= CustomRadioPacket[10];
				RfCommLost = 0;
			index=0;	
			CustomRadioPacket[index++]= 0;
			CustomRadioPacket[index++]= RfAnswerId;
			CustomRadioPacket[index++]= (UnitAddr>>24)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>16)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>8)&0xFF;
			CustomRadioPacket[index++]= UnitAddr&0xFF;
			
			CustomRadioPacket[0]= index;
		  System.Device.RF.RfStartTX(0,&CustomRadioPacket[0],index);
			}
			break;
		default:
			System.Device.RF.RfStartRX(0,16);
		  break;
	}
}

void RfCtrlParse(byte code)
{
	byte index=0;
	switch(code)
	{
		case RfConfig:
			RfDelay = RF_CONFIG_WAIT;
		  if(App.SysState.RFState&RF_STATE_CONNECT)
				RfState=App.SysState.RFState;
			App.SysState.RFState =RF_STATE_CONFIG;
			RfCommLost++;
			if(RfCommLost>RF_CONFIG_SLEEP)
			{
				RfCommLost=0;
				App.SysState.RFState =RfState; 
				RfState = RF_STATE_SLEEP;
			}
			
#ifdef __RF_UNIT
			System.Device.RF.RfStartRX(0,16);
#else
			System.Device.Beep.BeepOn(BEEP_2SHORT);
			if(!UnitAddr)
			{
				srand(AddrTimer);
				UnitAddr = rand();
			}
			index=0;
			CustomRadioPacket[index++]= 0;
			CustomRadioPacket[index++]= RfSendId;
			CustomRadioPacket[index++]= (UnitAddr>>24)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>16)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>8)&0xFF;
			CustomRadioPacket[index++]= UnitAddr&0xFF;
		  
			CustomRadioPacket[0]= index;
		  System.Device.RF.RfStartTX(0,&CustomRadioPacket[0],index);
		
#endif
			break;
		case RfSendOff:
			System.Device.RF.RfStartRX(0,16);
			break;
		case RfQuery:
			App.SysState.RFState |=RF_STATE_COMM;
			RfCommLost++;
			if(RfCommLost>RF_QUERY_LOST)
			{
				RfCommLost=0;
				App.SysState.RFState = RF_STATE_LOSE;
				if(App.RfAddrID)
				{
					App.RfAddrID = 0;
					StorePost(STORE_RFCONNECT);
				}
			}
			index=0;
			CustomRadioPacket[index++]= 0;
			CustomRadioPacket[index++]= RfQueryUnit;
			CustomRadioPacket[index++]= (UnitAddr>>24)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>16)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>8)&0xFF;
			CustomRadioPacket[index++]= UnitAddr&0xFF;

			CustomRadioPacket[0]= index;
		  System.Device.RF.RfStartTX(0,&CustomRadioPacket[0],index);
			break;
		case RfReady:
			index=0;
			CustomRadioPacket[index++]= 0;
			CustomRadioPacket[index++]= RfAnswerId;
			CustomRadioPacket[index++]= (UnitAddr>>24)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>16)&0xFF;
			CustomRadioPacket[index++]= (UnitAddr>>8)&0xFF;
			CustomRadioPacket[index++]= UnitAddr&0xFF;
			
			CustomRadioPacket[0]= index;
		  System.Device.RF.RfStartTX(0,&CustomRadioPacket[0],index);
			break;
		default:
			break;
	}
}

static void RFSystick10(void)
{
	static byte second=0;
	
	AddrTimer++;
	if(++second>9)   //per 1s
	{
		second=0;
		if(RfDelay)
			RfDelay--;
		else
		{
			if(App.SysState.RFState&RF_STATE_CONFIG)
			{
				PostMessage(MessageRfCtrl, RfConfig);
			}
			else if(App.SysState.RFState&RF_STATE_CONNECT)
			{
				RfDelay = RF_QUERY_INTERVAL;	
#ifdef __RF_UNIT
				System.Device.RF.RfStartRX(0,16);
				RfCommLost++;
				if(RfCommLost>RF_QUERY_LOST)
				{
					RfCommLost=0;
					App.SysState.RFState = RF_STATE_LOSE;
					if(App.RfAddrID)
					{
						App.RfAddrID = 0;
						StorePost(STORE_RFCONNECT);
					}

				}
#else
				PostMessage(MessageRfCtrl, RfQuery);
				
#endif
			}
			else if(App.SysState.RFState&RF_STATE_LOSE)
			{
				RfDelay = RF_QUERY_INTERVAL;	
#ifdef __RF_UNIT
				System.Device.RF.RfStartRX(0,16);
#else
				PostMessage(MessageRfCtrl, RfQuery);
#endif
			}
		}
		
	
				
	
	}
	
}

void UnitRFInit(void)
{
	System.Device.Systick.Register(Systick10, RFSystick10);
	System.Device.RF.RfInit(&CustomRadioPacket[0]);
	UnitAddr = App.RfAddrID;
	if(UnitAddr)
		App.SysState.RFState = RF_STATE_CONNECT;
	
}


