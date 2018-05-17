#ifndef __REMOTERF_H
 #define __REMOTERF_H

#include "system.h"
#include "app.h"

typedef enum{
	RfSendId = (byte)1,
	RfAnswerId= (byte)2,
	RfQueryUnit= (byte)3,
	RfUnitAnswer= (byte)4
}RfTransDataTypeDef;


void UnitRFInit(void);
void RfCtrlParse(byte code);
void RfRecvParse(byte code);
#endif
