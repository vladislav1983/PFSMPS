/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __BASEDEF_H
#define __BASEDEF_H

/* 
** Module Identification
*/
#define __BASEDEF

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
/* 
** Set up the oscillator for 39.614 MHz device operation frequency
** Configure Oscillator to operate the device at approx ~40Mhz
** Fosc= Fin*M/(N1*N2), Fcy=Fosc/2 where, M = PLLFBD + 2,  N1 = 2,  N2 = 2
** Fosc= 7.37*(43)/(2*2)=79.225 Mhz for Fosc, Fcy = 39.614 Mhz
*/
#define cFRC_Hz     7.37E6  /* 7.37 MHz internal oscillator frequency */
#define M           43.0    /* PLLFBD */
#define N1          2.0
#define N2          2.0
#define cFosc       ((cFRC_Hz*M)/(N1*N2))
#define cFCY_Hz     (cFosc/2.0)
#define cFCY_MHz    (cFCY_Hz/1.0E6)

/* ADC and PWM clock for 117.9MHz
** ((FRC * 16) / APSTSCLR ) = (7.37 * 16) / 1 = 117.9MHz
*/
#define cACLK_Hz    (cFRC_Hz * 16.0)
#define cACLK_Mhz   (cACLK_Hz/1000000.0)


/* FCY is used by library to calculate delay functions */
#ifndef FCY
    #define FCY     cFCY_Hz
#else
    #error "FCY must be defined here!"
#endif

/*
** INTERRUPT PRIORITY LEVELS
** DO NOT USE LEVEL 7 ISR PRIORITY
*/
#define cADCPair1_ICR_Priority          (5U)
#define cADCPair2_ICR_Priority          (5U)
#define cTIMER1_ISR_Priority            (4U)
#define cUART2_RX_ISR_Priority          (3U)
#define cUART2_TX_ISR_Priority          (3U)


/*
** Firmware version
*/
#define FIRMWARE_BASE_VERSION		    1000
#define FIRMWARE_SUB_VERSION		    10

/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/
typedef unsigned char       U8;
typedef unsigned int        U16;
typedef unsigned long       U32;
typedef unsigned long long  U64;
typedef unsigned int        tCPUItState;

typedef signed char         S8;
typedef signed int          S16;
typedef signed long         S32;
typedef S16                 Q15;

typedef float               F32;
typedef double              F64;
typedef U16                 tCtrl;
typedef U16                 tAddress;

typedef U8          	    BOOL;
#define cFalse              ((BOOL)(0))
#define cTrue        	    ((BOOL)(!cFalse))
typedef U16          	    HRESULT;
#define S_OK                ((HRESULT)0x00u)
#define S_FALSE             ((HRESULT)0x01u)
#define E_INVALIDARG        ((HRESULT)0x02u)
#define E_FAIL              ((HRESULT)0x03u)
#define S_NOK               ((HRESULT)0x04u)

typedef struct
{
    void* pBuff __attribute__ ((aligned));
    U16   Lng   __attribute__ ((aligned));
}tMsg;

/*
** Callback functions
*/
typedef void (*tCallback)();
typedef void (*tCallbackCtrl)(tCtrl);
typedef void (*tCallbackData)(tMsg);
typedef void (*tCallbackDataCtrl)(tCtrl, tMsg);
typedef void (*tFunc)();
typedef void (*tFuncData)(U8);
typedef void (*tFuncCallbackData)(U8);

#if !defined NULL
    #define NULL ((void*)0)
#endif

/* 
** Function parameters flow define
*/
#define _IN_ 
#define _OUT_ 


/* 
** System base definitions
*/
#define PWM_MODULE_REGISTER_TRACE
/*
** Enable freemaster used
*/
#define FREEMASTER_USED         1
#define SINGLESHOT_USED         0

/*
** Compiler specific macros
*/
#define _DECLARE_ISR(funct)         void __attribute__((__interrupt__,no_auto_psv)) (funct)(void)
#define _DECLARE_ISRFAST(funct)     void __attribute__((__interrupt__,shadow)) (funct)(void)
/* 
** System debug definitions
*/
#if defined(__DEBUG)

#endif
/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/


/*=====================================================================================================================
 * Included files to resolve specific definitions in project                        				                        
 *===================================================================================================================*/
#include <string.h>
#include "p33FJ64GS606.h"   /* CPU header */
#include "dsp.h"            /* DSP library */
#include "dio.h"
#include "libq.h"           /* Fixed point math library */
#include "api.h"            /* Shared API declarations */
#include "tools.h"          /* Arithmetic tools */
#include "traps.h"          /* CPU traps */
#include "system.h"
#if FREEMASTER_USED
#include "freemaster_tsa.h"
#include "freemaster.h"
#endif


#endif /* __BASEDEF_H */
