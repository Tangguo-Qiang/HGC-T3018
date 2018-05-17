#ifndef __LOGICCTRL_H
	#define __LOGICCTRL_H



#ifdef HD_GJ_360Z		/*2016.03.11, BypassInverse,300A-former*/
//#define __DOUBLE_MOTOS
//#define __EXCHANGE_FLOWS
#define __AUXI_HEATER

	#define		DISPFLOW_MIN   		80
	#define		DISPFLOW_MAX			360
	#define		DISPFLOW_ONESTEP	20
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	
	#define PWM_MOTO1_OUT_ACT				(uint16_t)1500    //100-70
	#define PWM_MOTO1_MIN						(uint16_t)7800    //70%
	#define PWM_MOTO1_OUT_STEP 			(PWM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)    //70%
  #define PWM_MOTO1_IN_STEP				PWM_MOTO1_OUT_STEP

	#define RPM_MOTO1_MIN							(uint16_t)400
	#define RPM_MOTO1_OUT_ACT					(uint16_t)840		//1500
	#define RPM_MOTO1_MAX							(RPM_MOTO1_OUT_ACT+RPM_MOTO1_MIN)
	#define RPM_MOTO1_OUT_STEP				(uint16_t)(RPM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)
	#define RPM_MOTO1_IN_STEP					RPM_MOTO1_OUT_STEP
	
#endif

#ifdef HD_GJ_180Z		/*2016.03.11, BypassInverse,300A-former*/
//#define __DOUBLE_MOTOS
//#define __EXCHANGE_FLOWS
#define __AUXI_HEATER

	#define		DISPFLOW_MIN   		60
	#define		DISPFLOW_MAX			180
	#define		DISPFLOW_ONESTEP	10
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	
	#define PWM_MOTO1_OUT_ACT				(uint16_t)1500    //100-70
	#define PWM_MOTO1_MIN						(uint16_t)7200    //70%
	#define PWM_MOTO1_OUT_STEP 			(PWM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)    //70%
  #define PWM_MOTO1_IN_STEP				PWM_MOTO1_OUT_STEP

	#define RPM_MOTO1_MIN							(uint16_t)500
	#define RPM_MOTO1_OUT_ACT					(uint16_t)900		
	#define RPM_MOTO1_MAX							(RPM_MOTO1_OUT_ACT+RPM_MOTO1_MIN)
	#define RPM_MOTO1_OUT_STEP				(uint16_t)(RPM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)
	#define RPM_MOTO1_IN_STEP					RPM_MOTO1_OUT_STEP
	
#endif



#endif
