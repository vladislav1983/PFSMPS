/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __KERNELTIMER_H
#define __KERNELTIMER_H

/*
** Module Identification
*/
#define __KERNELTIMER

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "kernel.h"
/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/
extern tOSClock OSClock;

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern void     OsTimerInit(void);
extern void     OSStartTimer(tOSTimer *pTimer);
extern void     OSStopTimer(tOSTimer *pTimer);
extern BOOL     OSIsTimerStarted(tOSTimer *pTimer);
extern BOOL     OSIsTimerElapsed(tOSTimer *pTimer, tOSTimer Timeout);
extern tOSTimer OSGetTimerElapsedTime(tOSTimer *pTimer);
extern void     OsTimerTask(void);
extern void     OSEnableTimer(U16 Period);
extern void     OSDisableTimer(void);

#endif /* __KERNELTIMER_H */
