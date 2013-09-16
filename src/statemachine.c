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
#ifdef __STATEMACHINE_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __STATEMACHINE_C 
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "statemachine.h"
#include "measure.h"
#include "kernelinclude.h"


/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/
#define cStateMachineAlarmPeriodUs      2500UL


/*
** Application statemachine states
*/
#define ST1_INIT_STATE                  0
#define ST1_WAIT_POWER_UP_STATE         1
#define ST1_CHECK_INIT_CONDITIONS       2
#define ST1_RAMP_PFC_REFERENCE          3
#define ST1_TEST_SMPS_OUTPUT            4
#define ST1_CONTROL_STATE               5
#define ST1_RAMP_DOWN_REFERENCES        6
#define ST1_RECOVERED_ERROR_STATE       7
#define ST1_UNRECOVERED_ERROR_STATE     8

/*
**
*/
#define ST1E_WAIT_ERROR_STATE           0


/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static tOSAlarm StateMachineTaskAlarm;
static tOSTimer ST1_Timer;

static U16 ST1_mainstate;
static U16 ST1_error_state;

/*=====================================================================================================================
 * Constant Local Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Data
 *===================================================================================================================*/
U16 SYSTEM_FLAGS = 0;
U16 ERROR_FLAGS = 0;
U16 PC_CONTROL_FLAGS = 0;
/*=====================================================================================================================
 * Local Functions Prototypes
 *===================================================================================================================*/
static void StateMachineErrorProcessing(void);

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
void StateMachineInit(void)
{
    StateMachineTaskAlarm.TaskID = cStateMachineTaskId;
    OsSetAlarm(&StateMachineTaskAlarm, cStateMachineAlarmPeriodUs/cOsAlarmTickUs);
    ST1_mainstate = ST1_INIT_STATE;
    ST1_error_state = ST1E_WAIT_ERROR_STATE;
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void StateMachineTask(void)
{
    
    /*------------------------------------------------------------------------------------------------------------------
     * Error check before statemachine
     *----------------------------------------------------------------------------------------------------------------*/
    if(_error_check())
    {
        if(ERROR_FLAGS != 0)
        {
            if(_system_general_fault() || _stack_fault() || _ram_fault() || _rom_fault())
            {
                ST1_mainstate = ST1_UNRECOVERED_ERROR_STATE;
            }

            ST1_mainstate = ST1_RECOVERED_ERROR_STATE;
        }
    }

    switch(ST1_mainstate)
    {
    /*------------------------------------------------------------------------------------------------------------------
     * State after reset. Wait DC-Link stabilization
     *----------------------------------------------------------------------------------------------------------------*/
    case ST1_INIT_STATE:
        ERROR_FLAGS = 0;    // Clear error flags
        OSStartTimer(&ST1_Timer);
        ST1_mainstate = ST1_WAIT_POWER_UP_STATE;
        break;
    /*------------------------------------------------------------------------------------------------------------------
     * Wait to power up
     *----------------------------------------------------------------------------------------------------------------*/
    case ST1_WAIT_POWER_UP_STATE:
        if(OSIsTimerElapsed(&ST1_Timer, (cPowerUpTimeUs/cOsTimerTickUs)))
        {
            if(_system_general_fault() || _stack_fault() || _ram_fault() || _rom_fault())
            {
                ST1_mainstate = ST1_UNRECOVERED_ERROR_STATE;
            }
            else
            {
                ST1_mainstate = ST1_CHECK_INIT_CONDITIONS;
            }
        }
        break;
    /*------------------------------------------------------------------------------------------------------------------
     * Check initial converter conditions
     *--------------------------------------------------------------------------------------------------------------*/
    case ST1_CHECK_INIT_CONDITIONS:
        if(ERROR_FLAGS != 0)
        {
            ST1_mainstate = ST1_RECOVERED_ERROR_STATE;
        }
        else
        {
            // Enable error check
            _set_error_check(1);
        }
        break;
    /*------------------------------------------------------------------------------------------------------------------
     * Default state
     *----------------------------------------------------------------------------------------------------------------*/
    default:
        _assert(cFalse);
        break;
    }
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
static void StateMachineErrorProcessing(void)
{
    switch(ST1_error_state)
    {
    /*------------------------------------------------------------------------------------------------------------------
     * Wait for error state
     *----------------------------------------------------------------------------------------------------------------*/
    case ST1E_WAIT_ERROR_STATE:


        break;
    /*------------------------------------------------------------------------------------------------------------------
     * Default state
     *----------------------------------------------------------------------------------------------------------------*/
    default:
        _assert(cFalse);
        break;
    }

}