/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __API_H
#define __API_H

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"

/*=====================================================================================================================
 * Generic constant data                                                              
 *===================================================================================================================*/
// Generic callback controls
#define cCallbackCtrlOK                     0
#define cCallbackCtrlNOK                    1
#define cCallbackCtrlError                  2

// Generic controls
#define cCtrl_Start                         3
#define cCtrl_Stop                          4

/*=====================================================================================================================
 * Callback channels                                                              
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern void SiremCommTimeoutCallback(void);

#endif /* __API_H */