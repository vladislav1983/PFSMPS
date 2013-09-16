/*=====================================================================================================================
 *  Used with pic30-gcc. and dsPIC.
 *  Language tool versions: pic30-as.exe v3.30, pic30-gcc.exe v3.30, pic30-ld.exe v3.30, pic30-ar.exe v3.30
 *=====================================================================================================================
 *    File name   : freemaster_dspic33.h
 *    Description : dsPIC33 platform implementation
 *    V.Gyurov
 * +----------+-------------+--------------------------------------------------------+
 * |Date      | Version     | Description                                            |
 * +----------+-------------+--------------------------------------------------------+
 *  10.12.2011  V1.0            Initial creation
 *
 *
 *===================================================================================================================*/
#ifndef __FREEMASTER_DSPIC33_H
#define __FREEMASTER_DSPIC33_H

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "freemaster.h"
#include "..\vuart.h"
/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
/******************************************************************************
* platform-specific default configuration
******************************************************************************/

/* using 16bit addressing commands */
#ifndef FMSTR_USE_NOEX_CMDS
    #define FMSTR_USE_NOEX_CMDS 1
#endif

/* using 32bit addressing commands */
#ifndef FMSTR_USE_EX_CMDS
    #define FMSTR_USE_EX_CMDS   0
#endif

/* using inline access to buffer memory */
#ifndef FMSTR_USE_INLINE_BUFFER_ACCESS
    #define FMSTR_USE_INLINE_BUFFER_ACCESS 0
#endif

/*****************************************************************************
* Board configuration information 
******************************************************************************/

#define FMSTR_PROT_VER                          3U      /* protocol version 3 */
#define FMSTR_CFG_FLAGS                         0U      /* board info flags */
#define FMSTR_CFG_BUS_WIDTH                     1U      /* data bus width */
#define FMSTR_GLOB_VERSION_MAJOR                2U      /* driver version */
#define FMSTR_GLOB_VERSION_MINOR                0U
#define FMSTR_IDT_STRING                        "dsPIC33 FreeMASTER Driver"
#define FMSTR_TSA_FLAGS                         FMSTR_TSA_INFO_HV2BA    /* HawkV2 TSA workaround */
/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/
/******************************************************************************
* platform-specific types
******************************************************************************/
typedef unsigned char   FMSTR_U8;     /* smallest memory entity (mostly 8bit) */
typedef unsigned int    FMSTR_U16;    /* 16bit value */
typedef unsigned long   FMSTR_U32;    /* 32bit value */

typedef signed char     FMSTR_S8;     /* signed 8bit value */
typedef signed int      FMSTR_S16;    /* signed 16bit value */
typedef signed long     FMSTR_S32;    /* signed 32bit value */

#if FMSTR_REC_FLOAT_TRIG
typedef float           FMSTR_FLOAT;    /* float value */
#endif

typedef unsigned char   FMSTR_FLAGS;  /* type to be union-ed with flags (at least 8 bits) */
typedef unsigned char   FMSTR_SIZE8;  /* size value (at least 8 bits) */
typedef signed int      FMSTR_INDEX;  /* general for-loop index (must be signed) */

typedef unsigned char   FMSTR_BCHR;   /* type of a single character in comm.buffer */
typedef unsigned char*  FMSTR_BPTR;   /* pointer within a communication buffer */

typedef unsigned int    FMSTR_SCISR;  /* data type to store SCI status register */


/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/

/* FMSTR_SetExAddr is needed only if both EX and non-EX commands are used */
#if FMSTR_USE_EX_CMDS && FMSTR_USE_NOEX_CMDS
    void FMSTR_SetExAddr(FMSTR_BOOL bNextAddrIsEx);
#else
/* otherwise, we know what addresses are used, (ignore FMSTR_SetExAddr) */
    #define FMSTR_SetExAddr(bNextAddrIsEx) 
#endif

/******************************************************************************
* communication buffer access functions
******************************************************************************/
#define FMSTR_ValueFromBuffer8(pDest, pSrc) \
    ( (*((FMSTR_U8*)(pDest)) = *(FMSTR_U8*)(pSrc)), (((FMSTR_BPTR)(pSrc))+1) )

#define FMSTR_ValueFromBuffer16(pDest, pSrc) \
    (memcpy((FMSTR_U8*)(pDest), (FMSTR_U8*)(pSrc), 2), (((FMSTR_BPTR)(pSrc))+2))

#define FMSTR_ValueFromBuffer32(pDest, pSrc) \
    (memcpy((FMSTR_U8*)(pDest), (FMSTR_U8*)(pSrc), 4), (((FMSTR_BPTR)(pSrc))+4))

#define FMSTR_ValueToBuffer8(pDest, src) \
    ( (*((FMSTR_U8*)(pDest)) = (FMSTR_U8)(src)), (((FMSTR_BPTR)(pDest))+1) )

#define FMSTR_ValueToBuffer16(pDest, src) \
    (memcpy((FMSTR_U8*)(pDest), (FMSTR_U8*)&(src), 2), (((FMSTR_BPTR)(pDest))+2))

#define FMSTR_ValueToBuffer32(pDest, src) \
    (memcpy((FMSTR_U8*)(pDest), (FMSTR_U8*)&(src), 4), (((FMSTR_BPTR)(pDest))+4))

#define FMSTR_SkipInBuffer(pDest, nSize) \
    ( ((FMSTR_BPTR)(pDest)) + (nSize) )

/**************************************************************************//*!
*
* @brief  
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/
static inline FMSTR_BPTR FMSTR_AddressFromBuffer(FMSTR_ADDR* pAddr, FMSTR_BPTR pSrc)
{
    /* fetch 16bit value */
    FMSTR_U16 nAddr16;
    pSrc = FMSTR_ValueFromBuffer16(&nAddr16, pSrc);
    /* and convert it to the address type */
    *pAddr = (FMSTR_U8*)nAddr16;
    return pSrc;
}

/**************************************************************************//*!
*
* @brief  
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/
static inline FMSTR_BPTR FMSTR_AddressToBuffer(FMSTR_BPTR pDest, FMSTR_ADDR nAddr)
{
    //FMSTR_U16 nAddr16 = nAddr;
    pDest = FMSTR_ValueToBuffer16(pDest, nAddr);
    return pDest;
}

/**************************************************************************//*!
*
* @brief  
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/
static inline FMSTR_BPTR FMSTR_ConstToBuffer8(FMSTR_BPTR pDest, FMSTR_U16 src)
{
    FMSTR_U8 srcl = (FMSTR_U8)(src);
    *(FMSTR_U8*)(pDest) = (FMSTR_U8)srcl;
    return(pDest+1);
}

/**************************************************************************//*!
*
* @brief  
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - source memory in communication buffer, mask follows data
* @param  nSize     - buffer size (always in bytes)
*
******************************************************************************/
static inline FMSTR_BPTR FMSTR_ConstToBuffer16(FMSTR_BPTR pDest, FMSTR_U16 src)
{
    FMSTR_U16 srcl = (FMSTR_U16)(src);
    memcpy((FMSTR_U8*)(pDest), (FMSTR_U8*)&(srcl), 2);
    return(pDest+2);
}

/**************************************************************************//*!
*
* @brief    The "memcpy" used internally in FreeMASTER driver
*
* @param    nDestAddr - destination memory address
* @param    nSrcAddr  - source memory address
* @param    nSize     - memory size (always in bytes)
*
******************************************************************************/
static inline void FMSTR_CopyMemory(FMSTR_ADDR nDestAddr, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize)
{
    memcpy((FMSTR_U8*)nDestAddr, (FMSTR_U8*)nSrcAddr, nSize);
}

/**************************************************************************//*!
*
* @brief  Write-into the communication buffer memory
*
* @param  pDestBuff - pointer to destination memory in communication buffer
* @param  nSrcAddr  - source memory address
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
static inline FMSTR_BPTR FMSTR_CopyToBuffer(FMSTR_BPTR pDestBuff, FMSTR_ADDR nSrcAddr, FMSTR_SIZE8 nSize)
{
    memcpy((FMSTR_U8*)pDestBuff, (FMSTR_U8*)nSrcAddr, nSize);
    return (pDestBuff+nSize);
}

/**************************************************************************//*!
*
* @brief  Read-out memory from communication buffer
*
* @param  nDestAddr - destination memory address
* @param  pSrcBuff  - pointer to source memory in communication buffer
* @param  nSize     - buffer size (always in bytes)
*
* @return This function returns a pointer to next byte in comm. buffer
*
******************************************************************************/
static inline FMSTR_BPTR FMSTR_CopyFromBuffer(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize)
{
    memcpy((FMSTR_U8*)nDestAddr, (FMSTR_U8*)pSrcBuff, nSize);
    return (pSrcBuff+nSize);
}

/****************************************************************************************
* memory access helper macros (used in recorder trigger compare routines)
*****************************************************************************************/
#define FMSTR_GetS8(addr)  ( *(FMSTR_S8*)(addr) )
#define FMSTR_GetU8(addr)  ( *(FMSTR_U8*)(addr) )
// #define FMSTR_GetS16(addr) ( *(FMSTR_S16*)(addr) )
static inline FMSTR_S16 FMSTR_GetS16(FMSTR_ADDR addr) 
{
    FMSTR_S16 Dest;
    memcpy((FMSTR_U8*)&Dest, (FMSTR_U8*)addr, 2);
    return(Dest);
}
// #define FMSTR_GetU16(addr) ( *(FMSTR_U16*)(addr) )
static inline FMSTR_U16 FMSTR_GetU16(FMSTR_ADDR addr) 
{
    FMSTR_U16 Dest;
    memcpy((FMSTR_U8*)&Dest, (FMSTR_U8*)addr, 2);
    return(Dest);
}
//#define FMSTR_GetS32(addr) ( *(FMSTR_S32*)(addr) )
static inline FMSTR_S32 FMSTR_GetS32(FMSTR_ADDR addr) 
{
    FMSTR_S32 Dest;
    memcpy((FMSTR_U8*)&Dest, (FMSTR_U8*)addr, 4);
    return(Dest);
}
//#define FMSTR_GetU32(addr) ( *(FMSTR_U32*)(addr) )
static inline FMSTR_U32 FMSTR_GetU32(FMSTR_ADDR addr) 
{
    FMSTR_U32 Dest;
    memcpy((FMSTR_U8*)&Dest, (FMSTR_U8*)addr, 4);
    return(Dest);
}

/****************************************************************************************
* Other helper macros
*****************************************************************************************/
#define FMSTR_PTR2ADDR(tmpAddr, ptr)    ( (tmpAddr ) = (FMSTR_ADDR)(ptr) )
/* This macro assigns C pointer to FMSTR_ADDR-typed variable */
#define FMSTR_ARR2ADDR(addrvar, arr)    FMSTR_PTR2ADDR((addrvar), (arr))

/****************************************************************************************
* Platform-specific configuration
*****************************************************************************************/

/* sanity check, at least one of the modes should be enabled */
#if (!FMSTR_USE_EX_CMDS) && (!FMSTR_USE_NOEX_CMDS)
#error At least one of FMSTR_USE_EX_CMDS or FMSTR_USE_NOEX_CMDS should be set non-zero
#endif

/* pipes not yet tested */
#define FMSTR_PIPES_EXPERIMENTAL 1

/****************************************************************************************
* General peripheral space access macros
*****************************************************************************************/

#define FMSTR_SETBIT(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) |= bit)
#define FMSTR_CLRBIT(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) &= (FMSTR_U16)~((FMSTR_U16)(bit)))
#define FMSTR_TSTBIT(base, offset, bit)     (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) & (bit))
#define FMSTR_SETREG(base, offset, value)   (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG(base, offset)          (*(volatile FMSTR_U16*)(((FMSTR_U32)(base))+(offset)))
#define FMSTR_SETREG32(base, offset, value) (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)) = value)
#define FMSTR_GETREG32(base, offset)        (*(volatile FMSTR_U32*)(((FMSTR_U32)(base))+(offset)))

/****************************************************************************************
* SCI module constants
*****************************************************************************************/


#define FMSTR_SCISR_TDRE        ((U16)(~UART_TX_BUF_NOT_FUL))   /* Transmit buffer is not full */

/*
** Data is processed in RX interrupt in VUART.C
** Data is always availible. Use mash that is always TRUE
*/
#define FMSTR_SCISR_RDRF        0x0001


/*******************************************************************************************
* SCI access macros
*****************************************************************************************/                          
/* transmitter enable/disable */
#define FMSTR_SCI_TE()                      {_UART_ClearTxIsrFlag(); _UART_EnableTxIsr();}
#define FMSTR_SCI_TD()                      _UART_DisableTxIsr()

/* receiver enable/disable */
#define FMSTR_SCI_RE()                      _UART_EnableRxIsr()
#define FMSTR_SCI_RD()                      _UART_DisableRxIsr()
#define FMSTR_SCI_TE_RE()                   Nop()

/* Transmitter-empty interrupt enable/disable : casting long to pointer */
#define FMSTR_SCI_ETXI()                    _UART_EnableTxIsr()
#define FMSTR_SCI_DTXI()                    _UART_DisableTxIsr()

/* Receiver-full interrupt enable/disable */
#define FMSTR_SCI_ERXI()                    _UART_EnableRxIsr()
#define FMSTR_SCI_DRXI()                    _UART_DisableRxIsr()

/* Transmit character */
#define FMSTR_SCI_PUTCHAR(ch)               (U2TXREG = (U8)(ch))

/* Get received character */
#define FMSTR_SCI_GETCHAR()                 FMSTR_UART_DATA

/* read status register */
#define FMSTR_SCI_GETSR()                   _UART_GetStatusRegister()

/* read & clear status register (clears error status bits only) */
#define FMSTR_SCI_RDCLRSR()                 (_UART_GetStatusRegister() | FMSTR_RX_INT); FMSTR_RX_INT = 0

/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
void FMSTR_CopyFromBufferWithMask(FMSTR_ADDR nDestAddr, FMSTR_BPTR pSrcBuff, FMSTR_SIZE8 nSize);

#endif /* __FREEMASTER_DSPIC33_H */
