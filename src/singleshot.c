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
#ifdef __SINGLESHOT_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __SINGLESHOT_C
#endif


/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "singleshot.h"

#if SINGLESHOT_USED
/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/

/*
** PFC singleshot states
*/
#define PFC_SS_WAIT_COMMAND         0
#define PFC_SS_WAIT_SHOTS           1
#define PFC_SS_RESET_DUTY           2
#define PFC_SS_STATE_LOAD           3

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static U16 pfc_singleshot_state;
static U16 u16PfcShotsCounter;
/*=====================================================================================================================
 * Constant Local Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Data
 *===================================================================================================================*/
U16 SINGLE_SHOT_FLAGS;
U16 u16PfcPrimaryChannelPulse;
U16 u16PfcSecondaryChannelPulse;
U16 u16Smps_Pulse;
U16 u16PfcShotsNumber;
U16 u16SmpsShotsNumber;
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
void SingleShotInit(void)
{
    SINGLE_SHOT_FLAGS = 0;
    u16PfcPrimaryChannelPulse = cPFC_PulseDefaultValue;
    u16PfcSecondaryChannelPulse = cPFC_PulseDefaultValue;
    u16Smps_Pulse = 200;
    u16PfcShotsNumber = 1;
    u16PfcShotsCounter = 1;
    u16SmpsShotsNumber = 1;

    pfc_singleshot_state = PFC_SS_WAIT_COMMAND;
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
inline void PFC_SingleshotProcess(void)
{
    switch(pfc_singleshot_state)
    {
    //------------------------------------------------------------------------------------------------------------------
    case PFC_SS_WAIT_COMMAND:
        // Get number of shots
        if(u16PfcShotsNumber <= cPFC_MaxShots)
            u16PfcShotsCounter = u16PfcShotsNumber;
        else
        {
            u16PfcShotsCounter = 1;
            break;
        }
        // Check that channels pulses are in range
        if(u16PfcPrimaryChannelPulse > (STPER - 100) || u16PfcSecondaryChannelPulse > (STPER - 100))
        {
            u16PfcPrimaryChannelPulse = cPFC_PulseDefaultValue;
            u16PfcSecondaryChannelPulse = u16PfcSecondaryChannelPulse;
            break;
        }

        if(_pfc_shot())
        {
            // get PFC channel and set duty. It will be updated on next PWM boundary
            if(_pfc_primary_channel_select() && _pfc_secondary_channel_select())
            {
                PDC4 = u16PfcPrimaryChannelPulse;
                SDC4 = u16PfcSecondaryChannelPulse;
                pfc_singleshot_state = PFC_SS_WAIT_SHOTS;
            }
            else if(_pfc_primary_channel_select())
            {
                PDC4 = u16PfcPrimaryChannelPulse;
                SDC4 = 0;
                pfc_singleshot_state = PFC_SS_WAIT_SHOTS;
            }else if(_pfc_secondary_channel_select())
            {
                PDC4 = 0;
                SDC4 = u16PfcSecondaryChannelPulse;
                pfc_singleshot_state = PFC_SS_WAIT_SHOTS;
            }
            _set_pfc_shot(0);
        }
        break;
    //------------------------------------------------------------------------------------------------------------------
    case PFC_SS_WAIT_SHOTS:

        --u16PfcShotsCounter;

        if(u16PfcShotsCounter == 0)
        {
            PDC4 = 0;
            SDC4 = 0;
            pfc_singleshot_state = PFC_SS_WAIT_COMMAND;
        }
        break;
    //------------------------------------------------------------------------------------------------------------------
    default:
        _assert(cFalse);
        break;
    }
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
inline void SMPS_SingleshotProcess(void)
{

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

#endif /* SINGLESHOT_USED */
