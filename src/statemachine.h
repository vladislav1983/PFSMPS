/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __STATEMCHINE_H
#define __STATEMCHINE_H

/*
** Module Identification
*/
#define __STATEMCHINE

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/
extern U16 SYSTEM_FLAGS;
extern U16 ERROR_FLAGS;
extern U16 PC_CONTROL_FLAGS;
/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/
#define cPowerUpTimeUs          30E3
/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/

/*=====================================================================================================================
 * SYSTEM FLAGS ACCESS                                                            
 *===================================================================================================================*/
#define _error_check()                              (_getbit(SYSTEM_FLAGS, 0))       /* bit 0 */
#define _set_error_check(x)                         (_putbit((x), SYSTEM_FLAGS, 0))  /* bit 0 */



/*=====================================================================================================================
 * ERROR FLAGS ACCESS                                                            
 *===================================================================================================================*/
#define _system_general_fault()                     (_getbit(ERROR_FLAGS, 0))        /* bit 0 */
#define _set_system_general_fault(x)                (_putbit((x), ERROR_FLAGS, 0))   /* bit 0 */

#define _stack_fault()                              (_getbit(ERROR_FLAGS, 1))        /* bit 1 */
#define _set_stack_fault(x)                         (_putbit((x), ERROR_FLAGS, 1))   /* bit 1 */

#define _ram_fault()                                (_getbit(ERROR_FLAGS, 2))        /* bit 2 */
#define _set_ram_fault(x)                           (_putbit((x), ERROR_FLAGS, 2))   /* bit 2 */

#define _rom_fault()                                (_getbit(ERROR_FLAGS, 3))        /* bit 3 */
#define _set_rom_fault(x)                           (_putbit((x), ERROR_FLAGS, 3))   /* bit 3 */

#define _PFC_VAC_undervoltage_fault()               (_getbit(ERROR_FLAGS, 4))        /* bit 4 */
#define _set_PFC_VAC_undervoltage_fault(x)          (_putbit((x), ERROR_FLAGS, 4))   /* bit 4 */

#define _PFC_VAC_overvoltage_fault()                (_getbit(ERROR_FLAGS, 5))        /* bit 5 */
#define _set_PFC_VAC_overvoltage_fault(x)           (_putbit((x), ERROR_FLAGS, 5))   /* bit 5 */

#define _PFC_VDC_undervoltage_fault()               (_getbit(ERROR_FLAGS, 6))        /* bit 6 */
#define _set_PFC_VDC_undervoltage_fault(x)          (_putbit((x), ERROR_FLAGS, 6))   /* bit 6 */

#define _PFC_VDC_overvoltage_fault()                (_getbit(ERROR_FLAGS, 7))        /* bit 7 */
#define _set_PFC_VDC_overvoltage_fault(x)           (_putbit((x), ERROR_FLAGS, 7))   /* bit 7 */

#define _PFC_IAC_overcurrent_fault()                (_getbit(ERROR_FLAGS, 8))        /* bit 8 */
#define _set_PFC_IAC_overcurrent_fault(x)           (_putbit((x), ERROR_FLAGS, 8))   /* bit 8 */

#define _PFC_IT1_overcurrent_fault()                (_getbit(ERROR_FLAGS, 9))        /* bit 9 */
#define _set_PFC_IT1_overcurrent_fault(x)           (_putbit((x), ERROR_FLAGS, 9))   /* bit 9 */

#define _PFC_IT2_overcurrent_fault()                (_getbit(ERROR_FLAGS, 10))       /* bit 10 */
#define _set_PFC_IT2_overcurrent_fault(x)           (_putbit((x), ERROR_FLAGS, 10))  /* bit 10 */

#define _SMPS_Vout_undervoltage_fault()             (_getbit(ERROR_FLAGS, 11))       /* bit 11 */
#define _set_SMPS_Vout_undervoltage_fault(x)        (_putbit((x), ERROR_FLAGS, 11))  /* bit 11 */

#define _SMPS_Vout_overvoltage_fault()              (_getbit(ERROR_FLAGS, 12))       /* bit 12 */
#define _set_SMPS_Vout_overvoltage_fault(x)         (_putbit((x), ERROR_FLAGS, 12))  /* bit 12 */

#define _SMPS_IH_fault()                            (_getbit(ERROR_FLAGS, 13))       /* bit 13 */
#define _set_SMPS_IH_fault(x)                       (_putbit((x), ERROR_FLAGS, 13))  /* bit 13 */

/*=====================================================================================================================
 * PC CONTROL FLAGS ACCESS                                                            
 *===================================================================================================================*/
//#define _()                           (_getbit(PC_CONTROL_FLAGS,0))       /* bit 0 */
//#define _(x)                          (_putbit((x),PC_CONTROL_FLAGS,0))   /* bit 0 */

/* PC_CONTROL_FLAGS  */
/*	
    Bit 0       -
    Bit 1       -
    Bit 2       -
    Bit 3       -
    Bit 4       -
    Bit 5       -
    Bit 6       -
    Bit 7       -
    Bit 8       -
    Bit 9       -
    Bit 10      -	
    Bit 11      -	
    Bit 12      -	
    Bit 13      -	
    Bit 14      -	
    Bit 15      -	
*/

#define _StateMachineInit()             StateMachineInit()

/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern void StateMachineInit(void);
extern void StateMachineTask(void);

#endif /* __STATEMCHINE_H */
