/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __DIO_H
#define __DIO_H

/*
** Module Identification
*/
#define __DIO

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
#define PIN_ASSERT_LED          _LATD0
#define PIN_ASSERT_LED_dir      _TRISD0

#define PIN_CPU_TRAP_LED        _LATD11
#define PIN_CPU_TRAP_LED_dir    _TRISD11

#define PIN_PFC_FAULT_LED       _LATD10
#define PIN_PFC_FAULT_LED_dir   _TRISD10

#define PIN_SMPS_FAULT_LED      _LATD9
#define PIN_SMPS_FAULT_LED_dir  _TRISD9

#define PIN_GREEN_LED           _LATD8
#define PIN_GREEN_LED_dir       _TRISD8

#define PIN_DEBUG_RD2           _LATD2
#define PIN_DEBUG_RD2_dir       _TRISD2

#define PIN_DEBUG_RD6           _LATD6
#define PIN_DEBUG_RD6_dir       _TRISD6

#define PIN_DEBUG_RF1           _LATF1
#define PIN_DEBUG_RF1_dir       _TRISF1

#define PIN_DEBUG_RE4           _LATE4
#define PIN_DEBUG_RE4_dir       _TRISE4

#define PIN_DEBUG_RE5           _LATE5
#define PIN_DEBUG_RE5_dir       _TRISE5



#define cPinInput               1
#define cPinOutput              0





/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/
typedef U16 tDioBuff;
typedef U8  tDioFilt;

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/
//extern U16 DioFilteredBuffer[cDioBuffLen];

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/
/*
** Pins config macros
*/
#define _DioAnalogConfig(pin, mode)         _DioAnalogPin(pin) = (mode)
#define _DioPinConfig(pin, mode)            _DioDirectionPin(pin) = (mode)
#define _DioPinOpenDrainConfig(pin, mode)   _DioOpenDrainPin(pin) = (mode)
/*
** Pins access macros
*/
#define _DioReadPinImmediate(pin)           _DioInputPin(pin)
#define _DioReadPinFiltered(pin)            ((BOOL)(DioFilteredBuffer[((pin##_Index)/sizeof(tDioBuff))] >> (pin##_Index)))
#define _DioWritePin(pin, val)              _DioOutputPin(pin) = (val)
#define _DioTogglePin(pin)                  _DioOutputPin(pin) = (~(_DioOutputPin(pin)))
/*
** Pins helper macros. Do not use these macros directly!
*/
#define _DeclarePin(port, pin)              port##pin
#define _DioInputPin(pin)                   _R##pin
#define _DioAnalogPin(pin)                  _ANS##pin
#define _DioOutputPin(pin)                  _LAT##pin
#define _DioDirectionPin(pin)               _TRIS##pin
#define _DioOpenDrainPin(pin)               _OD##pin

#define _DioInit()                          DioInit()

/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern void DioInit(void);
extern void DioTask(void);

#endif /* __DIO_H */
