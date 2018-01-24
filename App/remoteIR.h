#ifndef __REMOTEIR_H
  #define __REMOTE_H
	
#define IR_REMOTE_AIRFLOW_PLUS   		0x44
#define IR_REMOTE_AIRFLOW_MINUS   	0x66
#define IR_REMOTE_LOCK   						0x55
#define IR_REMOTE_VENTIRATE_PLUS  	0x33
#define IR_REMOTE_VENTIRATE_MINUS   0x77

#define IR_REMOTE_POWER   0x11
#define IR_REMOTE_MUTE   	0x22

#define IR_REMOTE_CIRCLEOUT   0xCC
#define IR_REMOTE_CIRCLEIN    0xBB
#define IR_REMOTE_CIRCLEAUTO  0xAA

#define IR_REMOTE_TIMERMODE  	0x88
#define IR_REMOTE_THERMALMODE  0x99


void ParseRemoteIR(byte order);


#endif
