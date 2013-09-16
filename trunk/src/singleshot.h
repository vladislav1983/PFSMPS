/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __SINGLESHOT_H
#define __SINGLESHOT_H

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"
#include "pfcparam.h"
#include "smpsparam.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
#define cPFC_MaxShots               10
#define cPFC_PulseDefaultValue      1000



#ifndef SINGLESHOT_USED
#define SINGLESHOT_USED     0
#endif
/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/
extern U16 SINGLE_SHOT_FLAGS;
extern U16 u16PfcPrimaryChannelPulse;
extern U16 u16PfcSecondaryChannelPulse;
extern U16 u16Smps_Pulse;
extern U16 u16PfcShotsNumber;
extern U16 u16SmpsShotsNumber;
/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/
/*
** SingleShot Flags
*/
#define _pfc_primary_channel_select()               (_getbit(SINGLE_SHOT_FLAGS, 0))      /* bit 0 */
#define _set_pfc_primary_channel_select(x)          (_putbit((x),SINGLE_SHOT_FLAGS, 0))  /* bit 0 */

#define _pfc_secondary_channel_select()             (_getbit(SINGLE_SHOT_FLAGS, 1))      /* bit 1 */
#define _set_pfc_secondary_channel_select(x)        (_putbit((x),SINGLE_SHOT_FLAGS, 1))  /* bit 1 */

#define _smps_select()					            (_getbit(SINGLE_SHOT_FLAGS, 2))      /* bit 2 */
#define _set_smps_select(x)				            (_putbit((x),SINGLE_SHOT_FLAGS, 2))  /* bit 2 */

#define _pfc_shot()					                (_getbit(SINGLE_SHOT_FLAGS, 3))      /* bit 3 */
#define _set_pfc_shot(x)				            (_putbit((x),SINGLE_SHOT_FLAGS, 3))  /* bit 3 */

#define _smps_shot()					            (_getbit(SINGLE_SHOT_FLAGS, 4))      /* bit 4 */
#define _set_smps_shot(x)				            (_putbit((x),SINGLE_SHOT_FLAGS, 4))  /* bit 4 */

#define _SingleShotInit()                           SingleShotInit()
/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern        void SingleShotInit(void);
extern inline void PFC_SingleshotProcess(void);
extern inline void SMPS_SingleshotProcess(void);


#endif /* __SINGLESHOT_H */
