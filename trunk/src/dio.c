/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/

/*=====================================================================================================================
 * Body Identification  
 *===================================================================================================================*/
#ifdef __DIO_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __DIO_C
#endif 

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "dio.h"
#include "kernelinclude.h"


/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/
#define cDioTaskPeriodUs            640e3   /* 640ms */

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static tOSAlarm DioTaskAlarm;

/*=====================================================================================================================
 * Constant Local Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local Functions Prototypes
 *===================================================================================================================*/


/*=====================================================================================================================
 *
 *                                  E X P O R T E D    F U N C T I O N S
 *
 *===================================================================================================================*/
/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void DioInit(void)
{

    /* Pins Init */
    PIN_ASSERT_LED          = 0;
    PIN_ASSERT_LED_dir      = cPinOutput;
    PIN_CPU_TRAP_LED        = 0;
    PIN_CPU_TRAP_LED_dir    = cPinOutput;
    PIN_PFC_FAULT_LED       = 0;
    PIN_PFC_FAULT_LED_dir   = cPinOutput;
    PIN_SMPS_FAULT_LED      = 0;
    PIN_SMPS_FAULT_LED_dir  = cPinOutput;
    PIN_GREEN_LED           = 0;
    PIN_GREEN_LED_dir       = cPinOutput;
    PIN_DEBUG_RD2           = 0;
    PIN_DEBUG_RD2_dir       = cPinOutput;
    PIN_DEBUG_RD6           = 0;
    PIN_DEBUG_RD6_dir       = cPinOutput;
    PIN_DEBUG_RF1           = 0;
    PIN_DEBUG_RF1_dir       = cPinOutput;
    PIN_DEBUG_RE4           = 0;
    PIN_DEBUG_RE4_dir       = cPinOutput;
    PIN_DEBUG_RE5           = 0;
    PIN_DEBUG_RE5_dir       = cPinOutput;

    DioTaskAlarm.TaskID = cDioTaskId;
    OsSetAlarm(&DioTaskAlarm, cDioTaskPeriodUs/cOsAlarmTickUs);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void DioTask(void)
{
    PIN_GREEN_LED = ~PIN_GREEN_LED;
}

/*=====================================================================================================================
 *                                                                            
 *                                     L O C A L    F U N C T I O N S                   
 *                                                                            
 *===================================================================================================================*/
/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
