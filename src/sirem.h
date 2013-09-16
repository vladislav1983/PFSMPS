/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __SIREM_H
#define __SIREM_H


/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
#define cSiremIndex_FrameStart  0        // Field index into sirem frame.
#define cSiremIndex_Cmd         1
#define cSiremIndex_Chks        9

#define cSiremFrameSize         10       // Sirem frame count bytes

#define cSiremCmd_ReadPrmInfo   0x78     // Field Command values
#define cSiremCmd_ReadPrmList   0x7E
#define cSiremCmd_ReadPrm       0x68
#define cSiremCmd_WritePrm      0x91
#define cSiremCmd_ReadTrace     0x71
#define cSiremCmd_Restart       0x57

#define cSiremValueLowFirst     0x00     // value for field Endianes (ReadPrmInfo)
#define cSiremValueHighFirst    0xFF

#define cSiremValueFrameStart   0xA9     // value for field FrameStart

#define cSoftwareVersion        ((U32)0x12345678ul)

/* Possible slave answers if desired command could not be performed */
#define cWRONG_ARRAY_INDEX      0x04
#define cNOV_WRITE_WHEN_RUN     0x08
#define cOUT_OF_LIMIT           0x10
#define cACCESS_LEVEL_FAULT     0x20
#define cPARAM_NOT_FOUND        0x40
#define cINIT_PERFORMED         0x80

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
#define _SiremInit()            SiremInit()

/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern void SiremInit(void);
extern void SiremTask(void);
extern void SiremEngine(void);
extern void SiremUartRxCallback();

#endif /* __SIREM_H */
