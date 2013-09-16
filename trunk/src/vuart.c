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
#ifdef __UART_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __UART_C
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include <string.h>
#include "vuart.h"
#include "timer.h"
#include "sirem.h"
#if FREEMASTER_USED
#include "freemaster_private.h"
#endif


/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/
#define cUART_Timeout          (U16)(cUART_TxTimeout_us * cFCY_MHz)

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/



/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/
typedef struct sMessage
{
    U8   au8Buff[cUart_RcvBuffSize] __attribute__ ((aligned)); 
    U8   u8DataCnt; 					// Available data in the buffer.number of bytes in buffer.
}tsRxMessage;

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static volatile tsRxMessage gRxMsg; /*Receiver object. Fill from IRQ.  Emptied from Appl.*/
/*NOTE! - gRxMsg IS NOT fully protected during access.*/

/*=====================================================================================================================
 * Constant Local Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Data
 *===================================================================================================================*/
volatile U8 FMSTR_UART_DATA;
#if FREEMASTER_USED
    volatile U16 FMSTR_RX_INT;
#endif
/*=====================================================================================================================
 * Local Functions Prototypes
 *===================================================================================================================*/
void __attribute__((__interrupt__,no_auto_psv)) _U2RXInterrupt();

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
 * Description: UART module initialization
 *===================================================================================================================*/
void UartInit(void)
{

    gRxMsg.u8DataCnt  = 0;
    memset((U8*)&gRxMsg.au8Buff[0], 0, sizeof(gRxMsg.au8Buff));

    _UART_WriteModeRegister(  UART_EN                /* Module enable */
                            & UART_IDLE_STOP         /* Stop all functions in IDLE mode*/
                            & UART_IrDA_DISABLE      /* IrDA encoder and decoder disabled */
                            & UART_MODE_SIMPLEX      /* UxRTS pin in Simplex mode */
                            & UART_UEN_00            /* UxTX and UxRX pins are enabled and used; UxCTS and UxRTS/BCLK pins controlled by port latches*/
                            & UART_DIS_WAKE          /* Disable Wake-up on START bit Detect during SLEEP Mode bit*/
                            & UART_DIS_LOOPBACK      /* Loop back disabled*/
                            & UART_DIS_ABAUD         /* Baud rate measurement disabled or completed*/
                            & UART_UXRX_IDLE_ONE     /* UxRx Idle state is one */
                            & UART_BRGH_SIXTEEN      /* BRG generates 16 clocks per bit period */
                            & UART_NO_PAR_8BIT
                            & UART_1STOPBIT);        /* 1 stop bit*/

    _UART_WriteBaudRateRegister(((U16)(cFCY_Hz / (16.0 * cUART_Baudrate)) - 1)); // U1BRG = Fcy/16/Baudrate - 1;

    _UART_WriteStatusRegister( UART_INT_TX                 /* Interrupt on transfer of every character to TSR */
                             & UART_IrDA_POL_INV_ZERO      /* IrDA encoded, UxTX Idel state is '0' */
                             & UART_SYNC_BREAK_DISABLED    /* Sync break transmission disabled or completed */
                             & UART_TX_ENABLE              /* Transmit enable */
                             & UART_INT_RX_CHAR            /* Interrupt on every char received */
                             & UART_ADR_DETECT_DIS);       /* address detect disable */

    _UART_DisableRxIsr();
    _UART_DisableTxIsr();
    _UART_ClearRxIsrFlag();
    _UART_ClearTxIsrFlag();
    _UART_SetRxPriority(cUART2_RX_ISR_Priority);
    _UART_SetTxPriority(cUART2_TX_ISR_Priority);
    _UART_EnableRxIsr();
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: 0 - no message, or number of available bytes
 *
 * Description: Check if message is received
 *===================================================================================================================*/
U8 Uart_MsgGetDataCnt(void)
{
    U8 u8DataCount = 0;

    _UART_DisableRxIsr();
    u8DataCount = gRxMsg.u8DataCnt;    // critical section manipulation
    _UART_EnableRxIsr();

    return(u8DataCount);	//return number of received bytes
}

/*=====================================================================================================================
 * Parameters: - Destination buffer to store read characters.
 *             - Size of buffer
 *
 * Return: S_OK    - message ready. Data buffer contain availing data
 *         S_FALSE - message not ready
 *
 * Description: Reads characters from the INPUT(Rx)
 *===================================================================================================================*/
HRESULT Uart_MsgGet(_OUT_ U8 *pDestinBuff)
{
    _UART_DisableRxIsr();
    memcpy((U8*)&pDestinBuff[0], (U8*)&gRxMsg.au8Buff[0], gRxMsg.u8DataCnt);
    gRxMsg.u8DataCnt = 0;	        // clear counter
    _UART_EnableRxIsr();

    return(S_OK);
}
/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: Purge input Rx buffer
 *===================================================================================================================*/
void Uart_RxPurge(void)
{
    _UART_DisableRxIsr();
    gRxMsg.u8DataCnt = 0;			//no bytes
    _UART_EnableRxIsr();
}

/*=====================================================================================================================
 * Parameters: - pSrcBuff   - Source buffer for transmitting
 *             - u16CntBytes - Size of buffer
 *
 * Return: Actually written bytes count
 *
 * Description: Send message via UART
 *===================================================================================================================*/
#ifdef UART_MSG_PUT
U16 Uart_MsgPut(_IN_ U8 *pSrcBuff, _IN_ U16 u16CntBytes)
{
    U16 u16Index;       // Input buffer index.
    U16 u16TxTimeout;

    _UART_DisableRxIsr();          // Rx Interrupt Disable. Echo are received here.
    u16TxTimeout = cUART_Timeout;   // Set timeout

    u16Index = 0;
    while(u16Index < u16CntBytes)
    {
        _UART_WriteTxBuffer(pSrcBuff[u16Index]); // transfer to serial shift reg.

        //wait byte transmission finish. used FIFO functionalities.
        while((_UART_GetTxBufferStatus() != 0)&& (u16TxTimeout != 0)) 
        {
            --u16TxTimeout;
        };

        u16Index++;
    }

    //_UART_ClearRxIsrFlag();
    _UART_EnableRxIsr();

    return(u16Index);        // actually written bytes
}
#endif

/*=====================================================================================================================
 * Parameters: U8 byte to transmit
 *
 * Return: Transmitted bytes. If transmit buffer is full return 0.
 *
 * Description: Send a single byte via UART
 *===================================================================================================================*/
U8 UART_PutByte(_IN_ U8 u8TByte)
{
    U8 tx_bytes = 0;

    _UART_DisableRxIsr();   // Rx Interrupt Disable. Echo are received here.

    if(_UART_GetTxBufferStatus()) // If Transmit Buffer is not full
    {
        _UART_WriteTxBuffer(u8TByte);
        tx_bytes++;
    }   
    else // transmit buffer full
    {
        tx_bytes = 0;
    }

    //_UART_ClearRxIsrFlag();
    _UART_EnableRxIsr();

    return(tx_bytes);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void __attribute__((__interrupt__,no_auto_psv)) _U2RXInterrupt()
{
    U16 u16RxTimeout = cUART_Timeout;
#if UART_PARITY_CHECK
    U8 u8Temp = 0;
#endif

    // Every received byte will reset sirem communication timeout
    SiremUartRxCallback();

#if UART_PARITY_CHECK
    while( ((_UART_GetParityError()) || (_UART_GetFramingError())) && (u16RxTimeout != 0))
    {
        --u16RxTimeout;
        u8Temp = _UART_GetRxBuffer();
    }
#endif
    u16RxTimeout = cUART_Timeout;

    while(_UART_GetRxBufferStatus() && (u16RxTimeout != 0))
    {
        FMSTR_UART_DATA = _UART_GetRxBuffer();
#if FREEMASTER_USED
        FMSTR_RX_INT = 1;
#endif

        if(gRxMsg.u8DataCnt < cUart_RcvBuffSize)
        {
            // Read from FIFO until it has a byte, and circular buffer is NOT full.
            gRxMsg.au8Buff[gRxMsg.u8DataCnt] = FMSTR_UART_DATA;
            gRxMsg.u8DataCnt++;
        }
#if FREEMASTER_USED && (FMSTR_LONG_INTR || FMSTR_SHORT_INTR)
        FMSTR_ProcessSCI();
#endif
        --u16RxTimeout;
    }

    _UART_ClearOverrunError();  // clear overrun error flag
    _UART_ClearRxIsrFlag();
    
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void __attribute__((__interrupt__,no_auto_psv)) _U2TXInterrupt(void)
{
    // Every transmitted byte will reset sirem communication timeout
    SiremUartRxCallback();

#if FREEMASTER_USED && (FMSTR_LONG_INTR || FMSTR_SHORT_INTR)
    FMSTR_ProcessSCI();
#endif
    _UART_ClearTxIsrFlag();
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

