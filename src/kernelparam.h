/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __KERNELPARAM_H
#define __KERNELPARAM_H

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
/*
** Include here modules that contains task function
*/
#include "system.h"
#include "dio.h"
#include "sirem.h"
#include "statemachine.h"
#include "measure.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
/*
** ************* Define task ID here *************
*/
/*1 */#define cIdleTaskId                         (tOSTaskID)(0x00000000) /* Idle task always have ID = 0 */
/*2 */#define cRefreshWatchDogTaskId              (tOSTaskID)(0x00000001)
/*3 */#define cOsTimerTaskId                      (tOSTaskID)(0x00000002)
/*4 */#define cDioTaskId                          (tOSTaskID)(0x00000004)
/*5 */#define cSiremCommunicationTaskId           (tOSTaskID)(0x00000008)
/*6 */#define cStateMachineTaskId                 (tOSTaskID)(0x00000010)
/*7 */#define cFastMeasureTaskId                  (tOSTaskID)(0x00000020)
/*8 */#define cSlowMeasureTaskId                  (tOSTaskID)(0x00000040)
///*9 */ #define cxxxTaskId                       (tOSTaskID)(0x00000080)
///*10*/ #define cxxxTaskId                       (tOSTaskID)(0x00000100)
///*11*/ #define cxxxTaskId                       (tOSTaskID)(0x00000200)
///*12*/ #define cxxxTaskId                       (tOSTaskID)(0x00000400)
///*13*/ #define cxxxTaskId                       (tOSTaskID)(0x00000800)
///*14*/ #define cxxxTaskId                       (tOSTaskID)(0x00001000)

/*
** ************* Define task number here - include idle task *************
*/
#define cOsTaskNumber                             8

/*
** ************* Define task list here *************
**
** | -> Lowest task priority
** |
** |
** v -> Highest task priority
** Order tasks according their ID's
*/
#define OSTaskListInit \
{\
/* Task Id  |Task Fct                           |    Period  | Description         */\
/* 0 */      OsIdleTask,                        /*   -       | IDLE task is only at position 0  */\
/* 1 */      RefreshWatchDogTask,               /*   5ms     | Refresh WatchDog task            */\
/* 2 */      OsTimerTask,                       /*   5ms     | OS timer task                    */\
/* 4 */      DioTask,                           /*   640ms   | DIO task                         */\
/* 8 */      SiremTask,                         /*   20ms    | Sirem communication task         */\
/* 10*/      StateMachineTask,                  /*   2.5ms   | Application state machine task   */\
/* 20*/      FastMeasureTask,                   /*   250us   | Fast measure task                */\
/* 40*/      SlowMeasureTask                    /*   20ms    | Slow measure task                */\
}


/*
** Define here OS interrupts level. This is maximum level of interrupt priority which OS will operate.
*/
#define cOsInterruptLevel               4
/*
** Define this to use scheduler stack
** Define cOsSchedulerStackMargin -> Margin of stack size
*/
#define NO_DYNAMIC_DATA
#define cOsSchedulerStackMargin         0

/*
** Alarm Parameters
*/
/* Define here hardware timer tick us */
#define cOsHardwareTimerTickUs          1250UL

/* Alarm tick definition 
**
*/
#define cOsAlarmTickUs                  2500UL
/*
** Define here OS timer period in us
*/
#define cOsTimerTickUs                  5000UL
/*
** Kernel options
*/
#define OS_AlarmGetSlotOptimization     1   // Optimize get slot function with dsPIC/PIC24 special instruction
/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/
/* 
** Define here services to suspend/resume OS interrupts
*/
#define _OsSuspendOsInterrupts()        { tCPUItState CPUItState; SET_AND_SAVE_CPU_IPL(CPUItState, cOsInterruptLevel);
#define _OsResumeOsInterrupts()           RESTORE_CPU_IPL(CPUItState); }

/*
** Define here services to enable/disable all interrupts
*/
#define _DisableAllInterrupts()         DisableAllInterrupts(7)
#define _EnableAllInterrupts()          EnableAllInterrupts()

/*
** Define here service to toggle OS interrupt
*/
#define _OSToggleOSInterrupts()                                         \
                                        do{                             \
                                            _EnableAllInterrupts();     \
                                            Nop();                      \
                                            _DisableAllInterrupts();    \
                                        }while(0)
/*
** Define here services to start/stop OS timer
*/
#define _OsStopHardwareTimer()          _StopSoftTimer(cSchedulerTimerId)
#define _OsStartHardwareTimer()         _StartSoftTimer(cSchedulerTimerId, cOsHardwareTimerTickUs)

/*
** Define here service to reset CPU
*/
#define _OsResetCpu()                   asm("goto __resetPRI")

/*
** Define here service to halt CPU
*/
#define _OsHaltCpu()                    Idle()

/*
** Define here OS startup hook function
** System init sequence - HAL_Init -> APPL_Init
*/
#define _OsStartupHook()                System_Init()

/*
** Define here external idle task
*/
#define _OsIdleTaskExternal()           IdleTask()



/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/


#endif /* __KERNELPARAM_H */
