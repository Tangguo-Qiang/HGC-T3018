#include "system.h"
#include "app.h"




#define FRAME_HEAD	0
#define FRAME_MAINORDER	1
#define FRAME_AIDORDER	2
#define FRAME_DATABEGIN	3
#define FRAME_DATALENGTH	4
#define FRAME_END	7

uint8_t CommRxBuffer[COMMRXDATASIZE]={0};
uint8_t CommTxBuffer[COMMTXDATASIZE]={0};

const byte ResetOrder[COMMRXDATASIZE]={0x46,0x28,0x52,0x45,0x53,0x45,0x54,0x29};





uint8_t Data_Sum(uint8_t* pSrc,uint8_t len)
{
	uint8_t i,sum=0;
	
	for(i=0;i<len; i++)
	{
		sum +=*pSrc++;
	}
	return sum;
}


/**
* @brief  Compares two buffers.
* @param  pBuffer1, pBuffer2: buffers to be compared.
* @param  BufferLength: buffer's length
* @retval PASSED: pBuffer1 identical to pBuffer2
*         FAILED: pBuffer1 differs from pBuffer2
*/
TestStatus IfBufferEmpty(uint8_t* pBuffer, uint16_t BufferLength)
{
  while (BufferLength>0)
  {
    if (*pBuffer != 0x00)
    {
      return PASSED;
    }
    pBuffer++;
		
		BufferLength--;
  }
  
  return FAILED;
}



	
Bool CommTalk_Echo(byte length)
{
	Bool flag=FALSE;
	byte i=0;
	signed char val=0;
	ushort rpm;
	
	if(length<COMMRXDATASIZE)
	{
		for(i=0;i<length;i++)
		   System.Device.Usart2.ReadByte(&CommRxBuffer[i]);
		flag = FALSE;
	}
	else
	{
		while(length)
		{
			do
			{
				flag = System.Device.Usart2.ReadByte(&CommRxBuffer[0]);
				length--;
				if(CommRxBuffer[0]==0x46)
					break;
			}while(length);
			if(length>6) //>=7
			{
				for(i=1;i<COMMRXDATASIZE;i++)
				{
					flag =System.Device.Usart2.ReadByte(&CommRxBuffer[i]);
					length--;
				}
				if(flag==TRUE)
				{
					i =Data_Sum(CommRxBuffer,COMMRXDATASIZE-1);
					if(i == CommRxBuffer[FRAME_END])
					{
						switch(CommRxBuffer[FRAME_MAINORDER])
						{
							case CODE_XFMOTODUTY:
							  if(CommRxBuffer[FRAME_AIDORDER]>10)
									App.SysFault.MotoXF++ ;
								else if(CommRxBuffer[FRAME_AIDORDER])
									App.SysFault.MotoXF =0;
								
								App.SysRunStatus.XFmotoPWM = CommRxBuffer[FRAME_DATABEGIN];
								App.SysRunStatus.XFmotoPWM <<= 8;
								App.SysRunStatus.XFmotoPWM |= CommRxBuffer[FRAME_DATABEGIN+1];
								rpm = CommRxBuffer[FRAME_DATABEGIN+2];
								rpm <<= 8;
								rpm |= CommRxBuffer[FRAME_DATABEGIN+3];
								App.SysRunStatus.XFmotoRPM = rpm;
								PostMessage(MessageEchoDate, COMM_XFMOTODUTY);
							break;
							case CODE_PFMOTODUTY:
							  if(CommRxBuffer[FRAME_AIDORDER]>10)
									App.SysFault.MotoPF++ ;
								else if(CommRxBuffer[FRAME_AIDORDER])
									App.SysFault.MotoPF =0;
							
								App.SysRunStatus.PFmotoPWM = CommRxBuffer[FRAME_DATABEGIN];
								App.SysRunStatus.PFmotoPWM <<= 8;
								App.SysRunStatus.PFmotoPWM |= CommRxBuffer[FRAME_DATABEGIN+1];
								rpm = CommRxBuffer[FRAME_DATABEGIN+2];
								rpm <<= 8;
								rpm |= CommRxBuffer[FRAME_DATABEGIN+3];
								App.SysRunStatus.PFmotoRPM = rpm;
								PostMessage(MessageEchoDate, COMM_PFMOTODUTY);
							break;
							case CODE_TEMPER_RH:
//								val = *((int8_t *)&CommRxBuffer[FRAME_DATABEGIN]);
//							  if((val>(-30))&&(val<(40)))
//								{
//									App.SysFault.TempInSensors=0;
//									App.SysState.FaultFlag &= ~SENSORINTEMP_FAULT;
//								}
//								else
//								{
//									val = -40;
//									App.SysFault.TempInSensors++;
//									if(App.SysFault.TempInSensors>10)
//										App.SysState.FaultFlag |= SENSORINTEMP_FAULT;
//								}
//								App.SensorData.TempInside  = val;
								
								val = *((int8_t *)&CommRxBuffer[FRAME_DATABEGIN+1]);
							  if((val>(-30))&&(val<(40)))
								{
									App.SysFault.TempOutSensors=0;
									App.SysState.FaultFlag &= ~SENSOROUTTEMP_FAULT;
								}
								else
								{
									val = -40;
									App.SysFault.TempOutSensors++;
									if(App.SysFault.TempOutSensors>10)
										App.SysState.FaultFlag |= SENSOROUTTEMP_FAULT;
								}
								App.SensorData.TempOutside = val;
								
//								val = CommRxBuffer[FRAME_DATABEGIN+2];
//								if(val>99)
//								{
//									val = 99;
//									App.SysFault.RHSensor++;
//									if(App.SysFault.RHSensor>10)
//										App.SysState.FaultFlag |= SENSORRH_FAULT;
//								}
//								else
//								{
//									App.SysFault.RHSensor=0;
//									App.SysState.FaultFlag &= ~SENSORRH_FAULT;
//								}
//								App.SensorData.RHInside = val;
//								
//								PostMessage(MessageEchoDate, COMM_TEMPER_RH);
								
								break;
							case CODE_CIRCLEMODE:
								App.SysRunStatus.BypassMode  = (BypassModeTypedef)CommRxBuffer[FRAME_AIDORDER];
								break;
//							case HEATTEMPER:
//								App.SysRunStatus.ThermalMode  = (HeatModeTypedef)CommRxBuffer[FRAME_AIDORDER];
//								break;
							case CODE_IAQ_READ:
								rpm = CommRxBuffer[FRAME_DATABEGIN];
								rpm |= (((uint16_t)CommRxBuffer[FRAME_DATABEGIN+1])<<8);
							  if(rpm>900)
								{
									App.SysFault.PMSensor++;
									if(App.SysFault.PMSensor>10)
									{
										rpm = 500;
										App.SensorData.PMInside = rpm;
										App.SysState.FaultFlag |= SENSORPM_FAULT;
									}
								}
								else
								{
									App.SensorData.PMInside = rpm;
									App.SysFault.PMSensor=0;
									App.SysState.FaultFlag &= ~SENSORPM_FAULT;
								}
								PostMessage(MessageEchoDate, COMM_IAQ_READ);
								break;
							case CODE_CO2_READ:
								rpm = CommRxBuffer[FRAME_DATABEGIN];
								rpm |= (((uint16_t)CommRxBuffer[FRAME_DATABEGIN+1])<<8);
							  if(rpm>4000)
								{
									App.SysFault.CO2Sensor++;
									if(App.SysFault.CO2Sensor>10)
									{
										App.SysState.FaultFlag |= SENSORCO2_FAULT;
										rpm = 4000;
										App.SensorData.CO2Inside = rpm;
									}
								}
								else
								{
									App.SensorData.CO2Inside = rpm;
									App.SysFault.CO2Sensor=0;
									App.SysState.FaultFlag &= ~SENSORCO2_FAULT;
								}
								PostMessage(MessageEchoDate, COMM_CO2_READ);
								break;
							case CODE_HEATER_SET:
								App.SysRunStatus.AuxiliaryHeat = (AuxiliaryHeatTypedef)CommRxBuffer[FRAME_AIDORDER];
								PostMessage(MessageEchoDate, COMM_HEATERSET);
								break;
							case CODE_POWER_SET:
								App.SysRunStatus.Power=  (PowerSetTypedef)CommRxBuffer[FRAME_AIDORDER];
							  if(CommRxBuffer[FRAME_DATABEGIN])
								{
									App.SysVersion.PowerVersion=CommRxBuffer[FRAME_DATABEGIN];
									App.SysVersion.PowerVersion <<= 8;
									App.SysVersion.PowerVersion |= CommRxBuffer[FRAME_DATABEGIN+1];
								}
								PostMessage(MessageEchoDate, COMM_POWER_SET);
								break;
							case CODE_VERSION:
								App.SysRunStatus.Power=  (PowerSetTypedef)CommRxBuffer[FRAME_AIDORDER];
								App.SysVersion.PowerVersion=CommRxBuffer[FRAME_DATABEGIN];
								App.SysVersion.PowerVersion <<= 8;
								App.SysVersion.PowerVersion |= CommRxBuffer[FRAME_DATABEGIN+1];
								
								break;

						}
					} //end of if(i == CommRxBuffer[FRAME_END])
					else	if((App.SysCtrlPara.Power==POWER_OFF)&&(CommRxBuffer[FRAME_END]==0x29))
					{
							for(i=0;i<8;i++)
							{
								if(CommRxBuffer[i] != ResetOrder[i])
									break;
							}
							if(i==8)
							{
								System.Device.Rtc.RTC_SetFlag(2);
								NVIC_SystemReset();
							}
					}
				}
				else
					length =0;
			}
			else //length<7
			{
				do
				{
					 flag=System.Device.Usart2.ReadByte(&CommRxBuffer[0]);
				}while(flag==TRUE);
				length=0;
				flag = FALSE;
			}
		}
	}
	return flag;
	
}


Bool CommTalk_Trans(byte order)
{
	Bool flag;
//	ushort rpm;
	
	Fill_BufferZero((uint8_t*)&CommTxBuffer[FRAME_HEAD], (uint16_t)COMMTXDATASIZE);	
	CommTxBuffer[FRAME_HEAD]=0x58;
	CommTxBuffer[FRAME_END]= Data_Sum((uint8_t*)CommTxBuffer,COMMTXDATASIZE-1);
	switch(order)
	{
		case COMM_BEEPONE:
			CommTxBuffer[FRAME_MAINORDER]=CODE_BEEP_SETON;
			CommTxBuffer[FRAME_AIDORDER]= 0x00;
			break;
		case COMM_BEEPTWO:
			CommTxBuffer[FRAME_MAINORDER]=CODE_BEEP_SETON;
			CommTxBuffer[FRAME_AIDORDER]= 0x02;
			break;
		case COMM_BEEPTHREE:
			CommTxBuffer[FRAME_MAINORDER]=CODE_BEEP_SETON;
			CommTxBuffer[FRAME_AIDORDER]= 0x03;
			break;
		case COMM_BEEPLONG:
			CommTxBuffer[FRAME_MAINORDER]=CODE_BEEP_SETON;
			CommTxBuffer[FRAME_AIDORDER]= 0xFF;
			break;
		case COMM_XFMOTODUTY:
			CommTxBuffer[FRAME_MAINORDER]=CODE_XFMOTODUTY;
			CommTxBuffer[FRAME_AIDORDER]= 0x00;
	    CommTxBuffer[FRAME_DATABEGIN]= App.SysCtrlStatus.XFmotoPWM>>8 ;
	    CommTxBuffer[FRAME_DATABEGIN+1]= App.SysCtrlStatus.XFmotoPWM&0xff ;
	    CommTxBuffer[FRAME_DATABEGIN+2]= App.SysCtrlStatus.XFmotoRPM>>8 ;
	    CommTxBuffer[FRAME_DATABEGIN+3]= App.SysCtrlStatus.XFmotoRPM&0xff ;
		  break;
		case COMM_PFMOTODUTY:
			CommTxBuffer[FRAME_MAINORDER]=CODE_PFMOTODUTY;
			CommTxBuffer[FRAME_AIDORDER]= 0x00;
			CommTxBuffer[FRAME_DATABEGIN]=App.SysCtrlStatus.PFmotoPWM>>8;
			CommTxBuffer[FRAME_DATABEGIN+1]=App.SysCtrlStatus.PFmotoPWM&0xff;
	    CommTxBuffer[FRAME_DATABEGIN+2]= App.SysCtrlStatus.PFmotoRPM>>8 ;
	    CommTxBuffer[FRAME_DATABEGIN+3]= App.SysCtrlStatus.PFmotoRPM&0xff ;
			break;
		case COMM_TEMPER_RH:
			CommTxBuffer[FRAME_MAINORDER]=CODE_TEMPER_RH;
			break;
		case COMM_CIRCLEMODE:
			CommTxBuffer[FRAME_MAINORDER]=CODE_CIRCLEMODE;
			CommTxBuffer[FRAME_AIDORDER]=App.SysCtrlStatus.BypassMode ;
			CommTxBuffer[FRAME_DATABEGIN]=((ushort)BYPASS_ANGLE>>8);
			CommTxBuffer[FRAME_DATABEGIN+1]=BYPASS_ANGLE&0xFF;
			break;
		case COMM_IAQ_READ:	
			CommTxBuffer[FRAME_MAINORDER]=CODE_IAQ_READ;
			break;
		case COMM_CO2_READ:
			CommTxBuffer[FRAME_MAINORDER]=CODE_CO2_READ;
			break;
		case COMM_HEATERSET:
			CommTxBuffer[FRAME_MAINORDER]=CODE_HEATER_SET;
			CommTxBuffer[FRAME_AIDORDER]=App.SysCtrlStatus.AuxiliaryHeatSet;
			break;
		case COMM_POWER_SET:
			CommTxBuffer[FRAME_MAINORDER]=CODE_POWER_SET;
			CommTxBuffer[FRAME_AIDORDER]=App.SysCtrlStatus.Power;
			break;
		case COMM_VERSION:
			CommTxBuffer[FRAME_MAINORDER]=CODE_VERSION;
			CommTxBuffer[FRAME_AIDORDER]=0;
			CommTxBuffer[FRAME_DATABEGIN]=MAIN_VERSION;
			CommTxBuffer[FRAME_DATABEGIN+1]=SUB_VERSION;
			break;
	}
	CommTxBuffer[FRAME_END]= Data_Sum((uint8_t*)CommTxBuffer,COMMTXDATASIZE-1);
	
	flag =System.Device.Usart2.WriteString(CommTxBuffer,COMMTXDATASIZE);
	
	return flag;
}

