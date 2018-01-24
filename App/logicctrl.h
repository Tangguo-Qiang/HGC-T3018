#ifndef __LOGICCTRL_H
	#define __LOGICCTRL_H

#ifdef HD_GJ_160Z		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		50
	#define		DISPFLOW_MAX			160
	#define		DISPFLOW_ONESTEP	10
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		((CTRLFLOW_STEPS/2)+1)

	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	#define MACHINE_CAPACITY     						300
	#define DISP_AIRFLOW_MIN								100
	#define DISP_AIRFLOW_VARY								200
	#define PWM_DUTYOCCUPY_MOTO1ACT											1500    //100-70
	#define PWM_DUTYOCCUPY_MOTO1MIN											7200    //70%
	#define PWM_DUTYOCCUPY_MOTO1MAX				(PWM_DUTYOCCUPY_MOTO1MIN+PWM_DUTYOCCUPY_MOTO1ACT)
 

	#define RPM_MOTO1_MAX							(uint16_t)1350		
	#define RPM_MOTO1_MIN							(uint16_t)400
	#define RPM_MOTO1_ACT							(RPM_MOTO1_MAX-RPM_MOTO1_MIN)
	#define RPM_MOTO1_STEP						(uint16_t)(RPM_MOTO1_ACT/CTRLFLOW_STEPS)

	#define RPM_MOTO1_TOP							(uint16_t)(RPM_MOTO1_MAX+50)
	#define RPM_MOTO1_BOTTOM					(uint16_t)(RPM_MOTO1_MIN-50)
	

#endif

#ifdef HD_GJ_300Z		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		100
	#define		DISPFLOW_MAX			300
	#define		DISPFLOW_ONESTEP	10
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	#define MACHINE_CAPACITY     						300
	#define DISP_AIRFLOW_MIN								100
	#define DISP_AIRFLOW_VARY								200
	#define PWM_DUTYOCCUPY_MOTO1ACT											1400    //100-70
	#define PWM_DUTYOCCUPY_MOTO1MIN											8000    //70%
	#define PWM_DUTYOCCUPY_MOTO1MAX				(PWM_DUTYOCCUPY_MOTO1MIN+PWM_DUTYOCCUPY_MOTO1ACT)

	#define RPM_MOTO1_TOP							(uint16_t)1240
	#define RPM_MOTO1_BTM							(uint16_t)480
 

	#define RPM_MOTO1_MAX							(uint16_t)1240		//1500
	#define RPM_MOTO1_MIN							(uint16_t)480
	#define RPM_MOTO1_ACT							(RPM_MOTO1_MAX-RPM_MOTO1_MIN)
	#define RPM_MOTO1_STEP						(uint16_t)((RPM_MOTO1_MAX-RPM_MOTO1_MIN)/CTRLFLOW_STEPS)
	
#endif


#ifdef HD_GJ_600Z		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		200
	#define		DISPFLOW_MAX			600
	#define		DISPFLOW_ONESTEP	20
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)  //20
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

	#define  __DOUBLE_MOTOS
	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	#define MACHINE_CAPACITY     						600
	#define DISP_AIRFLOW_MIN								200
	#define DISP_AIRFLOW_VARY								400
	
	#define PWM_DUTYOCCUPY_MOTO1ACT											1500    //100-70
	#define PWM_DUTYOCCUPY_MOTO1MIN											7400    //70%
	#define PWM_DUTYOCCUPY_MOTO1MAX				(PWM_DUTYOCCUPY_MOTO1MIN+PWM_DUTYOCCUPY_MOTO1ACT)
	#define PWM_MOTO1_STEP 								(PWM_DUTYOCCUPY_MOTO1ACT/CTRLFLOW_STEPS)    //70%
	
	#define PWM_DUTYOCCUPY_MOTO2ACT											1500    //100-70
	#define PWM_DUTYOCCUPY_MOTO2MIN											7400    //70%
	#define PWM_DUTYOCCUPY_MOTO2MAX				(PWM_DUTYOCCUPY_MOTO2MIN+PWM_DUTYOCCUPY_MOTO2ACT)
	#define PWM_MOTO2_STEP 								(PWM_DUTYOCCUPY_MOTO2ACT/CTRLFLOW_STEPS)    //70%


	#define RPM_MOTO1_MAX							(uint16_t)1060		//1500
	#define RPM_MOTO1_MIN							(uint16_t)400
	#define RPM_MOTO1_ACT							(RPM_MOTO1_MAX-RPM_MOTO1_MIN)
	#define RPM_MOTO1_STEP						(uint16_t)(RPM_MOTO1_ACT/CTRLFLOW_STEPS)

	#define RPM_MOTO2_MAX							(uint16_t)1060		//1500
	#define RPM_MOTO2_MIN							(uint16_t)400
	#define RPM_MOTO2_ACT							(RPM_MOTO2_MAX-RPM_MOTO2_MIN)
	#define RPM_MOTO2_STEP						(uint16_t)(RPM_MOTO2_ACT/CTRLFLOW_STEPS)
	
	#define RPM_MOTO1_TOP									(uint16_t)(RPM_MOTO1_MAX+50)
	#define RPM_MOTO2_TOP									(uint16_t)(RPM_MOTO2_MAX+50)
	#define RPM_MOTO1_BOTTOM							(uint16_t)(RPM_MOTO1_MIN-50)
	#define RPM_MOTO2_BOTTOM							(uint16_t)(RPM_MOTO2_MIN-50)

#endif


#ifdef DL_GJ_520Z		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		200
	#define		DISPFLOW_MAX			520
	#define		DISPFLOW_ONESTEP	20
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)  //20
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

	#define  __DOUBLE_MOTOS
	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	#define MACHINE_CAPACITY     						520
	#define DISP_AIRFLOW_MIN								200
	#define DISP_AIRFLOW_VARY								320
	
	#define PWM_DUTYOCCUPY_MOTO1ACT											1500    //100-70
	#define PWM_DUTYOCCUPY_MOTO1MIN											7400    //70%
	#define PWM_DUTYOCCUPY_MOTO1MAX				(PWM_DUTYOCCUPY_MOTO1MIN+PWM_DUTYOCCUPY_MOTO1ACT)
	
	#define PWM_DUTYOCCUPY_MOTO2ACT											1500    //100-70
	#define PWM_DUTYOCCUPY_MOTO2MIN											7400    //70%
	#define PWM_DUTYOCCUPY_MOTO2MAX				(PWM_DUTYOCCUPY_MOTO2MIN+PWM_DUTYOCCUPY_MOTO2ACT)


	#define RPM_MOTO1_MAX							(uint16_t)1060		//1500
	#define RPM_MOTO1_MIN							(uint16_t)400
	#define RPM_MOTO1_ACT							(RPM_MOTO1_MAX-RPM_MOTO1_MIN)
	#define RPM_MOTO1_STEP						(uint16_t)(RPM_MOTO1_ACT/CTRLFLOW_STEPS)

	#define RPM_MOTO2_MAX							(uint16_t)1060		//1500
	#define RPM_MOTO2_MIN							(uint16_t)400
	#define RPM_MOTO2_ACT							(RPM_MOTO2_MAX-RPM_MOTO2_MIN)
	#define RPM_MOTO2_STEP						(uint16_t)(RPM_MOTO2_ACT/CTRLFLOW_STEPS)
	
	#define RPM_MOTO1_TOP									(uint16_t)(RPM_MOTO1_MAX+50)
	#define RPM_MOTO2_TOP									(uint16_t)(RPM_MOTO2_MAX+50)
	#define RPM_MOTO1_BOTTOM							(uint16_t)(RPM_MOTO1_MIN-50)
	#define RPM_MOTO2_BOTTOM							(uint16_t)(RPM_MOTO2_MIN-50)

#endif


#ifdef DL_GJ_360Z		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		100
	#define		DISPFLOW_MAX			360
	#define		DISPFLOW_ONESTEP	10
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE												110
	#define MACHINE_CAPACITY     						300
	#define DISP_AIRFLOW_MIN								100
	#define DISP_AIRFLOW_VARY								200
	#define PWM_DUTYOCCUPY_MOTO1ACT											1400    //100-70
	#define PWM_DUTYOCCUPY_MOTO1MIN											8000    //70%
	#define PWM_DUTYOCCUPY_MOTO1MAX				(PWM_DUTYOCCUPY_MOTO1MIN+PWM_DUTYOCCUPY_MOTO1ACT)

	#define RPM_MOTO1_TOP							(uint16_t)1240
	#define RPM_MOTO1_BTM							(uint16_t)480
 

	#define RPM_MOTO1_MAX							(uint16_t)1240		//1500
	#define RPM_MOTO1_MIN							(uint16_t)480
	#define RPM_MOTO1_ACT							(RPM_MOTO1_MAX-RPM_MOTO1_MIN)
	#define RPM_MOTO1_STEP						(uint16_t)((RPM_MOTO1_MAX-RPM_MOTO1_MIN)/CTRLFLOW_STEPS)
	
#endif


#ifdef HD_GJ_360C		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		60
	#define		DISPFLOW_MAX			360
	#define		DISPFLOW_ONESTEP	20
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)  //15
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

//	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE						30
	#define  __DOUBLE_MOTOS
	#define  __EXCHANGE_FLOWS
//	#define __AUXI_HEATER
	
	#define PWM_MOTO1_OUT_ACT				(uint16_t)1500    //100-70
	#define PWM_MOTO1_IN_ACT				(uint16_t)1500    //100-70
	#define PWM_MOTO1_MIN						(uint16_t)7400    //70%
	#define PWM_MOTO1_MAX						(PWM_MOTO1_MIN+PWM_OUT_MOTO1_ACT)
	#define PWM_MOTO1_OUT_STEP 			(PWM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)    //70%
	#define PWM_MOTO1_IN_STEP 			(PWM_MOTO1_IN_ACT/CTRLFLOW_STEPS)    //70%
	
	#define PWM_MOTO2_MIN							(uint16_t)7600    //70%
	#define PWM_MOTO2_STOP						(uint16_t)7500    //70%
	#define PWM_MOTO2_ACT06						(uint16_t)600  //83-82
	#define PWM_MOTO2_ACT08						(uint16_t)900  //85-82
	#define PWM_MOTO2_ACT10						(uint16_t)1000  //87-82
	#define PWM_MOTO2_ACT12						(uint16_t)1200  //89-78
	#define PWM_MOTO2_MAX							(PWM_MOTO2_MIN+PWM_MOTO2_ACT12)


	#define RPM_MOTO1_MIN							(uint16_t)400
	#define RPM_MOTO1_OUT_ACT					(uint16_t)660		//1500
	#define RPM_MOTO1_IN_ACT					(uint16_t)660		//1500
	#define RPM_MOTO1_MAX							(RPM_MOTO1_OUT_ACT+RPM_MOTO1_MIN)
	#define RPM_MOTO1_OUT_STEP				(uint16_t)(RPM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)
	#define RPM_MOTO1_IN_STEP					(uint16_t)(RPM_MOTO1_IN_ACT/CTRLFLOW_STEPS)

	#define RPM_MOTO2_MIN								(uint16_t)400
	#define RPM_MOTO2_ACT12							(uint16_t)500		//1350
	#define RPM_MOTO2_ACT10							(uint16_t)340		//1350
	#define RPM_MOTO2_ACT08							(uint16_t)220		//1350
	#define RPM_MOTO2_ACT06							(uint16_t)120		//1350
	#define RPM_MOTO2_STOP							(uint16_t)350
	
	#define RPM_MOTO1_TOP									(uint16_t)(RPM_MOTO1_MAX+50)
	#define RPM_MOTO2_TOP									(uint16_t)(RPM_MOTO2_MAX+50)
	#define RPM_MOTO1_BOTTOM							(uint16_t)(RPM_MOTO1_MIN-50)
	#define RPM_MOTO2_BOTTOM							(uint16_t)(RPM_MOTO2_MIN-50)

#endif


#ifdef HD_GJ_160C		/*2016.03.11, BypassInverse,300A-former*/

	#define		DISPFLOW_MIN   		50
	#define		DISPFLOW_MAX			160
	#define		DISPFLOW_ONESTEP	5
	#define		DISPFLOW_RANGE		(DISPFLOW_MAX-DISPFLOW_MIN)
	#define		CTRLFLOW_STEPS	 (DISPFLOW_RANGE/DISPFLOW_ONESTEP)  //20
	#define		CTRLFLOW_STEP	 		1
	#define		CTRLFLOW_STOP	 		0
	#define		CTRLFLOW_STEP_MUTE	 		(CTRLFLOW_STEPS/2)

//	#define __BYPASS_INVERSE
	#define BYPASS_ANGLE						30
	#define  __DOUBLE_MOTOS
	#define  __EXCHANGE_FLOWS
//	#define __AUXI_HEATER
	
	#define PWM_MOTO1_OUT_ACT				(uint16_t)1500    //100-70
	#define PWM_MOTO1_IN_ACT				(uint16_t)1500    //100-70
	#define PWM_MOTO1_MIN						(uint16_t)7400    //70%
	#define PWM_MOTO1_MAX						(PWM_MOTO1_MIN+PWM_OUT_MOTO1_ACT)
	#define PWM_MOTO1_OUT_STEP 			(PWM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)    //70%
	#define PWM_MOTO1_IN_STEP 			(PWM_MOTO1_IN_ACT/CTRLFLOW_STEPS)    //70%
	
	#define PWM_MOTO2_MIN							(uint16_t)7600    //70%
	#define PWM_MOTO2_STOP						(uint16_t)7500    //70%
	#define PWM_MOTO2_ACT06						(uint16_t)600  //83-82
	#define PWM_MOTO2_ACT08						(uint16_t)900  //85-82
	#define PWM_MOTO2_ACT10						(uint16_t)1000  //87-82
	#define PWM_MOTO2_ACT12						(uint16_t)1200  //89-78
	#define PWM_MOTO2_MAX							(PWM_MOTO2_MIN+PWM_MOTO2_ACT12)


	#define RPM_MOTO1_MIN							(uint16_t)350
	#define RPM_MOTO1_OUT_ACT					(uint16_t)1100		//1450
	#define RPM_MOTO1_IN_ACT					(uint16_t)660		//1010
	#define RPM_MOTO1_MAX							(RPM_MOTO1_OUT_ACT+RPM_MOTO1_MIN)
	#define RPM_MOTO1_OUT_STEP				(uint16_t)(RPM_MOTO1_OUT_ACT/CTRLFLOW_STEPS)
	#define RPM_MOTO1_IN_STEP					(uint16_t)(RPM_MOTO1_IN_ACT/CTRLFLOW_STEPS)

	#define RPM_MOTO2_MIN								(uint16_t)350
	#define RPM_MOTO2_ACT12							(uint16_t)660		//1350
	#define RPM_MOTO2_ACT10							(uint16_t)550		//1350
	#define RPM_MOTO2_ACT08							(uint16_t)440		//1350
	#define RPM_MOTO2_ACT06							(uint16_t)330		//1350
	#define RPM_MOTO2_STOP							(uint16_t)350
	
	#define RPM_MOTO1_TOP									(uint16_t)(RPM_MOTO1_MAX+50)
	#define RPM_MOTO2_TOP									(uint16_t)(RPM_MOTO2_MAX+50)
	#define RPM_MOTO1_BOTTOM							(uint16_t)(RPM_MOTO1_MIN-50)
	#define RPM_MOTO2_BOTTOM							(uint16_t)(RPM_MOTO2_MIN-50)

#endif

#endif
