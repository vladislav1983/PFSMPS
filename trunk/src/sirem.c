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
#ifdef __SIREM_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __SIREM_C
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include <string.h>

#include "basedef.h"
#include "sirem.h"
#include "parlist.h"
#include "vuart.h"
#include "statemachine.h"
#ifdef TRACE_ENABLED
#include "trace.h"
#endif
#include "kernelinclude.h"

/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/
#define SIREM_COMMAND_BYTE		    au8SiremFrame[cSiremIndex_Cmd]	
#define _get_au8SiremFrame		    &au8SiremFrame[0]
#define cSiremTaskPeriodUs          20E3
#define cSiremCommTimeoutUs         20E3
#define cSiremProcessTimeoutUs      640E3    // 640ms

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/
typedef U8 	 HRESULT_SIREM_STATE;

/* 
** Main Sirem State Machine States - siremstate
*/
#define SIREM_WAIT_VALID_FRAME			    0x00
/* Protocol Defined States */
#define SIREM_READ_PRM_INFO                 cSiremCmd_ReadPrmInfo
#define SIREM_READ_PRM_LIST 	            cSiremCmd_ReadPrmList
#define SIREM_READ_PRM 					    cSiremCmd_ReadPrm
#define SIREM_WRITE_PRM					    cSiremCmd_WritePrm
#define SIREM_READ_TRACE_DATA			    cSiremCmd_ReadTrace
#define SIREM_RESTART_DRIVE				    cSiremCmd_Restart

/* 
** Sub Sirem State Machine States - siremsubstate
*/
#define SIREM_SUBSTATE_DEFAULT_VALUE        0x00
/* READ PRM INFO STATES */
#define READ_PRM_INFO_MSG_COMPOSE           0x00
#define READ_PRM_INFO_TRMT_FRAME            0x01
/* READ PRM LIST STATES */
#define READ_PRM_LIST_MSG_COMPOSE           0x00
#define READ_PRM_LIST_SEND_FRAME            0x01
#define READ_PRM_LIST_PARAM_COMPOSE         0x02
#define READ_PRM_LIST_SEND_PARAM_STRUCT     0x03
#define READ_PRM_LIST_PARAM_NAMES_COMPOSE   0x04
#define READ_PRM_LIST_SEND_PARAM_NAME       0x05
#define READ_PRM_LIST_CHECKSUM_COMPOSE      0x06
#define READ_PRM_LIST_CHECKSUM_SEND         0x07
/* READ PRM STATES */
#define READ_PRM_MSG_COMPOSE                0x00
#define READ_PRM_TRMT_FRAME                 0x01
/* WRITE PRM STATES */
#define WRITE_PRM_GET_PRM                   0x00
#define WRITE_PRM_TRMT_FRAME                0x01
/* READ TRACE DATA STATES */
#define READ_TRACE_COMPOSE_FRAME            0x00
#define READ_TRACE_TRMT_FRAME               0x01
#define READ_TRACE_ARRAY_HEADER_COMPOSE     0x02
#define READ_TRACE_TRMT_TRACE_ARRAY_HEADER  0x03
#define READ_TRACE_TRMT_TRACE_ARRAY         0x04
/* RESTART DRIVE STATES */
#define RESTART_DRIVE_CONFIRM               0x00
#define RESTART_DRIVE_TRMT_FRAME            0x01
#define RESTART_DRIVE_RESTART               0x02

/* Transmit Struct -> Transmit buffer pointer, and size */
typedef struct sTransmit
{
    U8  *pTrmBuff;
    U16 TrmBuffSize;
}tsTransmit;

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static U8         au8SiremFrame[cSiremFrameSize];
static tsTransmit trmt;
static tOSAlarm   SiremTaskAlarm;
static tOSTimer   SiremCommTimeoutTimer;
static tOSTimer   SiremSessionTimeoutTimer;
static U8         siremstate = SIREM_WAIT_VALID_FRAME;
static U8         siremsubstate = SIREM_SUBSTATE_DEFAULT_VALUE;

/*=====================================================================================================================
 * Constant Local Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local Functions Prototypes
 *===================================================================================================================*/
static HRESULT_SIREM_STATE GetValidSiremFrame(U8* pFrame);
static HRESULT             CmdExe_ReadPrmInfo(U8* pFrame);
static HRESULT             CmdExe_ReadPrmList(U8* pFrame);
static HRESULT             CmdExe_ReadPrm(U8* pFrame);
static HRESULT             CmdExe_WritePrm(U8* pFrame);
static void                CmdExe_Reset(U8* pFrame);
static U16                 Sirem_Checksum_Calc(U8 *pBuff, U16 BuffSize);
static void                Put_In_TRM_Buff(U8 *pBuff, U16 BuffSize);
static HRESULT             Transmit_Sirem_Buffer(void);


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
void SiremInit(void)
{
    /* Init sirem engine state */
    siremstate = SIREM_WAIT_VALID_FRAME;
    memset(au8SiremFrame, 0, sizeof(au8SiremFrame));

    SiremTaskAlarm.TaskID = cSiremCommunicationTaskId;
    OsSetAlarm(&SiremTaskAlarm, cSiremTaskPeriodUs/cOsAlarmTickUs);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void SiremTask(void)
{
    if(siremstate == SIREM_WAIT_VALID_FRAME)
    {
        if(cTrue == OSIsTimerElapsed(&SiremCommTimeoutTimer, (cSiremCommTimeoutUs/cOsTimerTickUs)))
        {
            // Timeout elapsed. Check is this sirem frame.
            siremstate = GetValidSiremFrame(au8SiremFrame);

            if(siremstate != SIREM_WAIT_VALID_FRAME)
            {
                // It is sirem frame. Process sirem frame. Start session timeout.
                OSStartTimer(&SiremSessionTimeoutTimer);
                // Reset siremsubstate
                siremsubstate = SIREM_SUBSTATE_DEFAULT_VALUE;
            }
        }
    }
    else
    {
        // Manage sirem frame processing timeout
        if(cTrue == OSIsTimerElapsed(&SiremSessionTimeoutTimer, (cSiremProcessTimeoutUs/cOsTimerTickUs)))
        {
            // Frame process is delay too much. Reset sirem state.
            siremstate = SIREM_WAIT_VALID_FRAME;
            // Reset siremsubstate
            siremsubstate = SIREM_SUBSTATE_DEFAULT_VALUE;
        }
    }
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void SiremUartRxCallback(void)
{
    // Only when frame waiting. If sirem engine process frame do not start timeout. 
    if(siremstate == SIREM_WAIT_VALID_FRAME)
        OSStartTimer(&SiremCommTimeoutTimer);
}
/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: Sirem Engine. Run In Background Loop
 *===================================================================================================================*/
void SiremEngine(void)
{
	switch(siremstate)
	{
	    /**************************************************************************/
	    /*
	     * Purpose: Block in this state until valid Sirem frame is received
	     */
	    /**************************************************************************/
	    case SIREM_WAIT_VALID_FRAME:
    	
	    break;
	    /**************************************************************************/
	    /*
	     * Purpose: get size and number of iolist member and send via rs232
	     */
	    /**************************************************************************/
	    case SIREM_READ_PRM_INFO:
    		
		    if( CmdExe_ReadPrmInfo(au8SiremFrame) == S_OK)
		    {
			    siremstate = SIREM_WAIT_VALID_FRAME;
		    }
    		
	    break;
	    /**************************************************************************/
	    /*
	     * Purpose: read param list
	     */
	    /**************************************************************************/
	    case SIREM_READ_PRM_LIST:
    		
		    if( CmdExe_ReadPrmList(au8SiremFrame) == S_OK )
		    {
			    siremstate = SIREM_WAIT_VALID_FRAME;
		    }
    	    
	    break;
	    /**************************************************************************/
	    /*
	     * Purpose: Parameter Read
	     */
	    /**************************************************************************/
	    case SIREM_READ_PRM:
    		
		    if( CmdExe_ReadPrm(au8SiremFrame) == S_OK )
		    {
			    siremstate = SIREM_WAIT_VALID_FRAME;
		    }
    		
	    break;
	    /**************************************************************************/
	    /*
	     * Purpose: Parameter Write
	     */
	    /**************************************************************************/
	    case SIREM_WRITE_PRM:
    		
		    if( CmdExe_WritePrm(au8SiremFrame) == S_OK )
		    {
			    siremstate = SIREM_WAIT_VALID_FRAME;
		    }
    	    
	    break;
	    /**************************************************************************/
	    /*
	     * Purpose: read trace data array
	     */
	    /**************************************************************************/
	    case SIREM_READ_TRACE_DATA:
    	
	    #ifdef TRACE_ENABLED
		    if( CmdExe_ReadTraceData(au8SiremFrame) == S_OK )
		    {
			    siremstate = SIREM_WAIT_VALID_FRAME;
		    }
        #else
                siremstate = SIREM_WAIT_VALID_FRAME;
	    #endif
	    break;
	    /**************************************************************************/
	    /*
	     * Purpose: Drive Restart Confirm and Restart
	     */
	    /**************************************************************************/
	    case SIREM_RESTART_DRIVE:
    		
		    CmdExe_Reset(au8SiremFrame);
		    // In this state CPU is restarted.

	    break;
    	
	    default:
		    _assert(cFalse);
		    siremstate = SIREM_WAIT_VALID_FRAME;
	    break;
	    /*-----------------------------------------------------------------------*/
	}// end switch
}

/*=====================================================================================================================
 *                                                                            
 *                                     L O C A L    F U N C T I O N S                   
 *                                                                            
 *===================================================================================================================*/
/*=====================================================================================================================
 * Parameters: Return pointer to sirem Frame
 *
 * Return: Extracted SIREM Command
 *
 * Description: Block here until valid Sirem frame is received
 *===================================================================================================================*/
static HRESULT_SIREM_STATE GetValidSiremFrame(U8* pFrame) //return pointer *pframe
{
    U8 u8Temp;
    U8 u8Chks;
    U8 sirem_state = 0;

    u8Temp = Uart_MsgGetDataCnt();

    if(0 == u8Temp)
    {
        return(0);
    }
    else if(cSiremFrameSize == u8Temp)
    {
        Uart_MsgGet(pFrame);

        if(cSiremValueFrameStart == pFrame[cSiremIndex_FrameStart])
        {
            u8Chks = (U8)Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks);

            if(((U8)(~u8Chks) == pFrame[cSiremIndex_Chks]))
            {
                sirem_state = au8SiremFrame[cSiremIndex_Cmd];
            }
            else
            {
                Uart_RxPurge();	//clean input rx structure
            }
        }
        else
        {
            Uart_RxPurge();	//clean input rx structure
        }
    }
    else
    {
        Uart_RxPurge();	//clean input rx structure
    }

    return(sirem_state);
}

/*=====================================================================================================================
 * Parameters: pointer of sirem frame
 *
 * Return: ack status
 *
 * Description: Get size and number of iolist member and send it.
 *===================================================================================================================*/
static HRESULT CmdExe_ReadPrmInfo(U8* pFrame)
{
	static U16 u16Chks = 0;	// checksum

    U32 u32Lenght;  	// Length of whole ReadPrmList frame.
    U16 u16Index;		// index
    U16 u16ParamCnt;	//parameter count
    
    switch(siremsubstate)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Compose message frame, calc checksum and put in trmt buffer
	 	*/
		/**********************************************************************/
  		case READ_PRM_INFO_MSG_COMPOSE:
  			
  			// ReadPrmList commands transmitted byte calculation
		    u32Lenght = (1+1+4);                        // add size of "start field", "functional field", "SoftVersion field"
		    u32Lenght += (U32)IF_Parlist_GetCntBytes(); // add size of whole parameters struct.
		    u16ParamCnt = IF_Parlist_GetCntMembers();	//get number of iolist members
		    for( u16Index=0; u16Index<u16ParamCnt; ++u16Index)
		    {                                           // add size for each "parameter name" including NULL terminator.
		        u32Lenght += 1 + strlen(iopar[u16Index].name);     // including NULL termination character
		    }
		    u32Lenght += 2;                             // add size of CRC field.
		
		    // fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadPrmInfo;
		    pFrame[2]                       = cSiremValueLowFirst;              // low byte first;
		    pFrame[3]                       = sizeof(iopar[0].name);            // sizeof of corresponding pointers
		    pFrame[4]                       = sizeof(iopar[0].read_adress);
		    pFrame[5]                       = (U8)(u32Lenght>>24);
		    pFrame[6]                       = (U8)(u32Lenght>>16);
		    pFrame[7]                       = (U8)(u32Lenght>>8);
		    pFrame[8]                       = (U8)(u32Lenght>>0);   // according above defined Endianes
		    
		    // Calculate Checksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks] = (U8)(~u16Chks);
			
			// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
			
			// GOTO transmit state
		    siremsubstate = READ_PRM_INFO_TRMT_FRAME;

    	break;    	
		/**********************************************************************/
		/*
	 	* Purpose: Transmit Frame
	 	*/
		/**********************************************************************/
  		case READ_PRM_INFO_TRMT_FRAME:
  			
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_PRM_INFO_MSG_COMPOSE;
	  			
	  			return(S_OK);
  			}
  			
    	break;
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			_assert(cFalse);
		break;
    } // end switch
    
    return(S_FALSE);  
}

/*=====================================================================================================================
 * Parameters: sirem frame
 *
 * Return: ack status
 *
 * Description: read param list
 *===================================================================================================================*/
static HRESULT CmdExe_ReadPrmList(U8* pFrame)
{
    static U16 u16Chks;
    static U16 u16Index;
    static U16 u16ParamCnt;

    U16 u16BuffSize; 
    U8* pu8Buff;
    
    
    switch(siremsubstate)
    {
		/**********************************************************************/
		/*
	 	* Purpose: Compose message frame, calc checksum and put in trmt buffer
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_MSG_COMPOSE:
  			
  			//Send - Start, Cmd, SoftVer
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadPrmList;
		    pFrame[2]                       = (U8)(cSoftwareVersion>>0);   // Little Endian (low first)
		    pFrame[3]                       = (U8)(cSoftwareVersion>>8);
		    pFrame[4]                       = (U8)(cSoftwareVersion>>16);
		    pFrame[5]                       = (U8)(cSoftwareVersion>>24);
		    
		    u16Chks = 0;
		    
		   	// Calculate Checksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, 6); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks] = (U8)(~u16Chks);
			
			// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, 6);
			
		    siremsubstate = READ_PRM_LIST_SEND_FRAME;
		    
    	break;    
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_SEND_FRAME:
  			
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_PRM_LIST_PARAM_COMPOSE;
  			}
  			
    	break;   
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_PARAM_COMPOSE:
  			
  			// Send - whole param struct
		    pu8Buff = (U8*)iopar;	//8bit pointer to iopar
		    
		    u16BuffSize = IF_Parlist_GetCntBytes();//get iopar size
		    
		    u16Chks += Sirem_Checksum_Calc(pu8Buff, u16BuffSize); 
		    
		    // Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pu8Buff, u16BuffSize);
		    
		    siremsubstate = READ_PRM_LIST_SEND_PARAM_STRUCT;
		      
    	break;	
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_SEND_PARAM_STRUCT:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_PRM_LIST_PARAM_NAMES_COMPOSE;
	  			u16Index = 0;
	  			u16ParamCnt = IF_Parlist_GetCntMembers();	//get number of members on structure iopar
  			}
    	break;   
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_PARAM_NAMES_COMPOSE:
  		
  			// Send line by line all param names from iopar[] (including last delimiter line with number 0)
		    if( u16Index < u16ParamCnt )
		    {
		        // Send - Param name string
		        pu8Buff     = (U8*)(iopar[u16Index].name);
		        u16BuffSize = 1 + strlen((char*)pu8Buff);                  // including NULL termination character
		        
		        u16Chks += Sirem_Checksum_Calc(pu8Buff, u16BuffSize);
		        
		    	// Put Buffer In transmit Buffer
		    	Put_In_TRM_Buff(pu8Buff, u16BuffSize);
		    	
		    	siremsubstate = READ_PRM_LIST_SEND_PARAM_NAME;
		    	
		    	++u16Index;
		    }
		    else
		    {
			    siremsubstate = READ_PRM_LIST_CHECKSUM_COMPOSE;
		    }
  			
    	break; 
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_SEND_PARAM_NAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_PRM_LIST_PARAM_NAMES_COMPOSE;
  			}
  			
    	break;
    	  
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_CHECKSUM_COMPOSE:
  		
  		 	pFrame[0] = (U8)(u16Chks>>0);               // Little endian
    		pFrame[1] = (U8)(u16Chks>>8);
    		
    		// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, 2);
		    
		    siremsubstate = READ_PRM_LIST_CHECKSUM_SEND;
    		
    	break;
    	
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_LIST_CHECKSUM_SEND:
  		  	if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_PRM_LIST_MSG_COMPOSE;
	  			
	  			return(S_OK);
  			}
    	break;
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			_assert(cFalse);
		break;
    } // end switch
    
    
    return(S_FALSE);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static HRESULT CmdExe_ReadPrm(U8* pFrame)
{	
    U32 u32PrmValue;
    U16 u16PrmNumber;
    U16 u16Chks = 0;
    
    iolist *pio;
	
	switch(siremsubstate)
    {
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_MSG_COMPOSE:
  		
  			u16PrmNumber = 0x100u * ((U16)pFrame[3]) + pFrame[4];       // Extract Parameter number
  			
		    pio = IF_Parlist_bnu(u16PrmNumber);
		    
		    if( sizeof(U16) == pio->size )                              // Read Parameter value
		    {
		        u32PrmValue = *((U16*)pio->read_adress);                    // 2 bytes parameter value
		    }
		    else if( sizeof(U32) == pio->size )
		    {
		        u32PrmValue = *((U32*)pio->read_adress);                    // 4 bytes parameter value
		    }
		    else
		    {
		        u32PrmValue = *((U8*)pio->read_adress);                     // 1 byte parameter value
		    }
		    
		    // fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadPrm;
		    pFrame[2]                       = 0; // ???
		    pFrame[3]                       = pFrame[3];                // PrmNumber - keep the same
		    pFrame[4]                       = pFrame[4];
		    pFrame[5]                       = (U8)(u32PrmValue>>24);
		    pFrame[6]                       = (U8)(u32PrmValue>>16);
		    pFrame[7]                       = (U8)(u32PrmValue>>8);
		    pFrame[8]                       = (U8)(u32PrmValue>>0);   // according above defined Endianes
  			
  			// Calculate Chacksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks] = (U8)(~u16Chks);
			
			// Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
			
		    siremsubstate = READ_PRM_TRMT_FRAME;
		    	    
    	break; 
    	
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_PRM_TRMT_FRAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_PRM_MSG_COMPOSE;
	  			
	  			return(S_OK);
  			}		    
    	break;       
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			_assert(cFalse);
		break;    	
    } // end switch
    
    return(S_FALSE);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static HRESULT CmdExe_WritePrm(U8* pFrame)
{	
    U32 u32PrmValue;
    U16 u16PrmNumber;
    U16 u16Chks = 0;
    iolist *pio;
    
    switch(siremsubstate)
    {
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case WRITE_PRM_GET_PRM:
  			
  			
  			u16PrmNumber = 0x100u * ((U16)pFrame[3]) + pFrame[4];       // Extract Parameter number
		
		    u32PrmValue = pFrame[5];                                    // Extract new Parameter value
		    u32PrmValue = (u32PrmValue<<8) + pFrame[6];
		    u32PrmValue = (u32PrmValue<<8) + pFrame[7];
		    u32PrmValue = (u32PrmValue<<8) + pFrame[8];
		
		    pio = IF_Parlist_bnu(u16PrmNumber);
		    
		    if( sizeof(U16) == pio->size )                              // Write new Parameter value
		    {
		        *((U16*)pio->write_adress) = (U16)u32PrmValue;               // 2 bytes parameter value
		    }
		    else if( sizeof(U32) == pio->size )
		    {
		        *((U32*)pio->write_adress) = (U32)u32PrmValue;               // 4 bytes parameter value
		    }
		    else
		    {
		        *((U8*)pio->write_adress) = (U8)u32PrmValue;                 // 1 byte parameter value
		    }
		
		    // fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_WritePrm;
		    pFrame[2]                       = 0; // ???
		    pFrame[3]                       = pFrame[3];                // PrmNumber - keep the same
		    pFrame[4]                       = pFrame[4];
		    pFrame[5]                       = pFrame[5];                // PrmValue  - keep the same
		    pFrame[6]                       = pFrame[6];
		    pFrame[7]                       = pFrame[7];
		    pFrame[8]                       = pFrame[8];
		    
  			// Calculate Checksum
		    u16Chks = Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
		    
		    // Invert Checksum
		    pFrame[cSiremIndex_Chks]        = (U8)(~u16Chks);
		    
		    // Put Buffer In transmit Buffer
		    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
			
			// Go to transmit frame state
		    siremsubstate = WRITE_PRM_TRMT_FRAME;
		    
    	break; 
    	
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case WRITE_PRM_TRMT_FRAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = WRITE_PRM_GET_PRM;
	  			
	  			return(S_OK);
  			}		    
    	break;       
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			_assert(cFalse);
		break;    	
    } // end switch

    return(S_FALSE);
}

#ifdef TRACE_ENABLED
/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static HRESULT CmdExe_ReadTraceData(U8* pFrame)
{
	static U16 u16Array_length;
	static U16 u16Chks;
	
	static U16 u16Index;
	U16 u16_store_pos;
	U16 u16_nb_var;
	U16 u16_nb_steps;
	
	
	switch(siremsubstate)
    {
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_TRACE_COMPOSE_FRAME:
  			
  			u16_nb_var = cNB_TRACE_VARS;	//channels
			u16_nb_steps = cTRACE_STEPS;	//samples
			u16_store_pos = cTRACE_STEPS;		//trigger_sample
			
			/* length (number of words to read) */
			u16Array_length = (pFrame[8] + ((U16)pFrame[7]<<8) + ((U32)pFrame[6]<<16));
		
			/* index of the first element to be read */
			u16Index = (((U16)pFrame[5]) + ((U16)pFrame[4]<<8) + ((U32)pFrame[3]<<16));
		
			if(u16Array_length == 0)	/* length = 0: return trace info */
			{
				pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
			    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadTrace;
				pFrame[2] = 0;
				pFrame[3] = (U8)u16_store_pos>>8;
				pFrame[4] = (U8)u16_store_pos;
				pFrame[5] = (U8)u16_nb_var>>8;
				pFrame[6] = (U8)u16_nb_var;
				pFrame[7] = (U8)u16_nb_steps>>8;	
				pFrame[8] = (U8)u16_nb_steps;
			    
	  			// Calculate Checksum
			    u16Chks = (U16)Sirem_Checksum_Calc(pFrame, cSiremIndex_Chks); 
			    
			    // Invert Checksum
			    pFrame[cSiremIndex_Chks]        = (U8)(~u16Chks);
			    
			    // Put Buffer In transmit Buffer
			    Put_In_TRM_Buff(pFrame, cSiremFrameSize);
				
				// Go to transmit frame state
			    siremsubstate = READ_TRACE_TRMT_FRAME;
			} 
			else
			{
				siremsubstate = READ_TRACE_ARRAY_HEADER_COMPOSE;
			}
    	break; 
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_TRACE_TRMT_FRAME:
  		  	if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_TRACE_COMPOSE_FRAME;
	  			return(S_OK);
  			}		 
  		break;
  		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_TRACE_ARRAY_HEADER_COMPOSE:
  		
  			pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_ReadTrace;
		    
			u16Chks = 0;
			u16Chks = (U16)(pFrame[0]<<8) + (pFrame[1]);
			
			// Put Buffer In transmit Buffer
			Put_In_TRM_Buff(pFrame, 2);
				
			// Go to transmit frame state
			siremsubstate = READ_TRACE_TRMT_TRACE_ARRAY_HEADER;
			
  		break;
  		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_TRACE_TRMT_TRACE_ARRAY_HEADER:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = READ_TRACE_TRMT_TRACE_ARRAY;
  			}
  		break;
  		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case READ_TRACE_TRMT_TRACE_ARRAY:
			if(u16Array_length > 0)
			{
				u16Chks += TRSB[u16Index];
				
				while( UART_PutByte(TRSB[u16Index] >> 0) == 0 ) {};
				while( UART_PutByte(TRSB[u16Index] >> 8) == 0 ) {};
				
				++u16Index;
				--u16Array_length;
			}
			else
			{
				while( UART_PutByte( (U8)(u16Chks >> 0) ) == 0 ) {};
				while( UART_PutByte( (U8)(u16Chks >> 8) ) == 0 ) {};
				siremsubstate = READ_TRACE_COMPOSE_FRAME;
				return(S_OK);
			}
  		break;
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			_assert(cFalse);
		break;    
    	
    }// end switch
    
	return(S_FALSE);
}
#endif

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static void CmdExe_Reset(U8* pFrame)
{    
    switch(siremsubstate)
    {
		/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case RESTART_DRIVE_CONFIRM:
  		
  			// fill acknowledge frame.
		    pFrame[cSiremIndex_FrameStart]  = cSiremValueFrameStart;
		    pFrame[cSiremIndex_Cmd]         = cSiremCmd_Restart;
		    pFrame[2]                       = 0; // ???
		    pFrame[3]                       = 0;
		    pFrame[4]                       = 0;
		    pFrame[5]                       = 0;
		    pFrame[6]                       = 0;
		    pFrame[7]                       = 0;
		    pFrame[8]                       = 0;
		    pFrame[cSiremIndex_Chks]		= 0xFF;	// Manual calculated checksum
		    
		   	Put_In_TRM_Buff(pFrame, cSiremFrameSize);
				
			// Go to transmit frame state
			siremsubstate = RESTART_DRIVE_TRMT_FRAME;
    	break; 
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case RESTART_DRIVE_TRMT_FRAME:
  			if( Transmit_Sirem_Buffer() == S_OK )
  			{
	  			siremsubstate = RESTART_DRIVE_RESTART;
  			}	
    	break; 
    	/**********************************************************************/
		/*
	 	* Purpose: 
	 	*/
		/**********************************************************************/
  		case RESTART_DRIVE_RESTART:
  			
  			/* RESET CPU */
  			asm("goto __resetPRI"); //changed from goto __reset to C30 V3.21 compatible
  			
  			// CPU Restarted ... who cares about fucking state.
	    
    	break;       
    	/**********************************************************************/
		/*
	 	* Purpose: Default State Handler
	 	*/
		/**********************************************************************/
    	default:
			_assert(cFalse);
		break;    	
    } // end switch
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static void Put_In_TRM_Buff(U8 *pBuff, U16 BuffSize)
{
    trmt.pTrmBuff 		= (U8 *)pBuff;
    trmt.TrmBuffSize 	= (U16)BuffSize;
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static HRESULT Transmit_Sirem_Buffer(void)
{
    U8 u8Temp;

    if(trmt.TrmBuffSize)	// if buffer contain data
    {
        u8Temp = *trmt.pTrmBuff;	// extract transmit byte

        if(UART_PutByte(u8Temp) == 1)		// if single byte is transmitted
        {
            --trmt.TrmBuffSize;					// decrement buffer size with number of transmitted bytes
            ++trmt.pTrmBuff;					// increment pointer of transmit buffer
        }
    }
    else
    {
        return(S_OK);
    }

    return(S_FALSE);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
static U16 Sirem_Checksum_Calc(U8 *pBuff, U16 BuffSize)
{
    U16 i;
    U16 u8Chks = 0;

    for( i=0; i<BuffSize; ++i)
    {
        u8Chks += pBuff[i];
    }

    return(u8Chks);
}
