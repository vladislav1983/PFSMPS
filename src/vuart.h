/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __UART_H
#define __UART_H

/*
** Module Identification
*/
#define __UART

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"
#include "freemaster_cfg.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
/*
** Define here UART module used
*/
#define UART2_USED  1
/*
** Define parity check
*/
#define UART_PARITY_CHECK   0
/*
** UART module settings
*/
#define cUART_Baudrate              115200.0
#define cUART_TxTimeout_us          500.0
#define cUart_RcvBuffSize           (10u)

/* defines for UxMODE register */
#define UART_EN                 0xFFFF  /* Module enable */
#define UART_DIS                0x7FFF  /* Module disable */

#define UART_IDLE_CON           0xDFFF  /* Work in IDLE mode */
#define UART_IDLE_STOP          0xFFFF  /* Stop all functions in IDLE mode*/

#define UART_IrDA_ENABLE        0xFFFF  /* IrDA encoder and decoder enabled*/
#define UART_IrDA_DISABLE       0xEFFF  /* IrDA encoder and decoder disabled */

#define UART_MODE_SIMPLEX       0xFFFF  /* UxRTS pin in Simplex mode */
#define UART_MODE_FLOW          0xF7FF  /* UxRTS pin in Flow Control mode*/

#define UART_UEN_11             0xFFFF  /*UxTX,UxRX and BCLK pins are enabled and used; UxCTS pin controlled by port latches*/
#define UART_UEN_10             0xFEFF  /*UxTX,UxRX, UxCTS and UxRTS pins are enabled and used*/
#define UART_UEN_01             0xFDFF  /*UxTX,UxRX and UxRTS pins are enabled and used; UxCTS pin controlled by port latches*/
#define UART_UEN_00             0xFCFF  /*UxTX and UxRX pins are enabled and used; UxCTS and UxRTS/BCLK pins controlled by port latches*/

#define UART_EN_WAKE            0xFFFF  /*Enable Wake-up on START bit Detect during SLEEP Mode bit*/
#define UART_DIS_WAKE           0xFF7F  /*Disable Wake-up on START bit Detect during SLEEP Mode bit*/

#define UART_EN_LOOPBACK        0xFFFF  /*Loop back enabled*/
#define UART_DIS_LOOPBACK       0xFFBF  /*Loop back disabled*/

#define UART_EN_ABAUD           0xFFFF  /*Enable baud rate measurement on the next character*/
#define UART_DIS_ABAUD          0xFFDF  /*Baud rate measurement disabled or completed*/

#define UART_UXRX_IDLE_ZERO     0xFFFF  /* UxRX Idle state is zero */
#define UART_UXRX_IDLE_ONE      0xFFEF  /* UxRx Idle state is one */

#define UART_BRGH_FOUR          0xFFFF  /* BRG generates 4 clocks per bit period */
#define UART_BRGH_SIXTEEN       0xFFF7  /* BRG generates 16 clocks per bit period */

#define UART_NO_PAR_9BIT        0xFFFF  /*No parity 9 bit*/
#define UART_ODD_PAR_8BIT       0xFFFD  /*odd parity 8 bit*/
#define UART_EVEN_PAR_8BIT      0xFFFB  /*even parity 8 bit*/
#define UART_NO_PAR_8BIT        0xFFF9  /*no parity 8 bit*/

#define UART_2STOPBITS          0xFFFF  /*2 stop bits*/
#define UART_1STOPBIT           0xFFFE  /*1 stop bit*/

/* defines for UART Status register */
#define UART_INT_TX_BUF_EMPTY   0xDFFF  /* Interrupt on TXBUF becoming empty */
#define UART_INT_TX_LAST_CH     0x7FFF  /* Interrupt when last character shifted out*/
#define UART_INT_TX             0x5FFF  /* Interrupt on transfer of every character to TSR */

#define UART_IrDA_POL_INV_ONE   0xFFFF  /*IrDA encoded, UxTX Idle state is '1' */
#define UART_IrDA_POL_INV_ZERO  0xBFFF  /* IrDA encoded, UxTX Idel state is '0' */

#define UART_SYNC_BREAK_ENABLED   0xFFFF  /* Send sync break on next transmission */
#define UART_SYNC_BREAK_DISABLED  0xF7FF  /* Sync break transmission disabled or completed */

#define UART_TX_ENABLE          0xFFFF  /* Transmit enable */
#define UART_TX_DISABLE         0xFBFF  /* Transmit disable */

#define UART_TX_BUF_FUL         0xFFFF  /* Transmit buffer is full */
#define UART_TX_BUF_NOT_FUL     0xFDFF  /* Transmit buffer is not full */

#define UART_INT_RX_BUF_FUL     0xFFFF  /* Interrupt on RXBUF full */
#define UART_INT_RX_3_4_FUL     0xFFBF  /* Interrupt on RXBUF 3/4 full */
#define UART_INT_RX_CHAR        0xFF7F  /* Interrupt on every char received */

#define UART_ADR_DETECT_EN      0xFFFF  /* address detect enable */
#define UART_ADR_DETECT_DIS     0xFFDF  /* address detect disable */

#define UART_RX_OVERRUN_CLEAR   0xFFFD  /* Rx buffer Over run status bit clear */
#define UART_RX_DATA_AVAILIBLE  0x0001

/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/
extern volatile U16 FMSTR_RX_INT;
extern volatile U8 FMSTR_UART_DATA;
/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/
#if UART1_USED
    #define _UART_DisableRxIsr()                _U1RXIE = 0
    #define _UART_DisableTxIsr()                _U1TXIE = 0
    #define _UART_EnableRxIsr()                 _U1RXIE = 1
    #define _UART_EnableTxIsr()                 _U1TXIE = 1
    #define _UART_ClearRxIsrFlag()              _U1RXIF = 0          
    #define _UART_ClearTxIsrFlag()              _U1TXIF = 0
    #define _UART_SetRxPriority(Priority)       _U1RXIP = (Priority)
    #define _UART_SetTxPriority(Priority)       _U1TXIP = (Priority)

    #define _UART_WriteModeRegister(Data)       (U1MODE = (U16)(Data))
    #define _UART_WriteBaudRateRegister(Data)   (U1BRG = (U16)(Data))
    #define _UART_WriteStatusRegister(Data)     (U1STA = (U16)(Data))
    #define _UART_GetStatusRegister()           (U1STA)

    #define _UART_GetTxBufferStatus()           ((U1STAbits.UTXBF) == 0)
    #define _UART_GetParityError()              ((U1STAbits.PERR) == 1)
    #define _UART_GetFramingError()             ((U1STAbits.FERR) == 1)
    #define _UART_GetRxBufferStatus()           ((U1STAbits.URXDA) == 1)
    #define _UART_ClearOverrunError()           (U1STAbits.OERR = 0)
    #define _UART_WriteTxBuffer(Data)           (U1TXREG = (U8)(Data))
    #define _UART_GetRxBuffer()                 (U1RXREG)

#elif UART2_USED
    #define _UART_DisableRxIsr()                _U2RXIE = 0
    #define _UART_DisableTxIsr()                _U2TXIE = 0
    #define _UART_EnableRxIsr()                 _U2RXIE = 1
    #define _UART_EnableTxIsr()                 _U2TXIE = 1
    #define _UART_ClearRxIsrFlag()              _U2RXIF = 0          
    #define _UART_ClearTxIsrFlag()              _U2TXIF = 0
    #define _UART_SetRxPriority(Priority)       _U2RXIP = (Priority)
    #define _UART_SetTxPriority(Priority)       _U2TXIP = (Priority)

    #define _UART_WriteModeRegister(Data)       (U2MODE = (U16)(Data))
    #define _UART_WriteBaudRateRegister(Data)   (U2BRG = (U16)(Data))
    #define _UART_WriteStatusRegister(Data)     (U2STA = (U16)(Data))
    #define _UART_GetStatusRegister()           (U2STA)

    #define _UART_GetTxBufferStatus()           ((U2STAbits.UTXBF) == 0)
    #define _UART_GetParityError()              ((U2STAbits.PERR) == 1)
    #define _UART_GetFramingError()             ((U2STAbits.FERR) == 1)
    #define _UART_GetRxBufferStatus()           ((U2STAbits.URXDA) == 1)
    #define _UART_ClearOverrunError()           (U2STAbits.OERR = 0)
    #define _UART_WriteTxBuffer(Data)           (U2TXREG = (U8)(Data))
    #define _UART_GetRxBuffer()                 (U2RXREG)
#else
    #error "Used UART module not defined !"
#endif

#define _UartInit()                             UartInit()
/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern                            void UartInit(void);
extern                              U8 Uart_MsgGetDataCnt(void);
extern                         HRESULT Uart_MsgGet(_OUT_ U8 *pDestinBuff);
extern                            void Uart_RxPurge(void);
extern                             U16 Uart_MsgPut(_IN_ U8 *pSrcBuff, _IN_ U16 u16CntBytes);
extern                              U8 UART_PutByte(_IN_ U8 u8TByte);
extern                            void UartEnterActive(void);
extern                            void UartEnterSleep(void);
extern                            void SiremCommTimeoutCallback(void);

#endif /* __UART */
