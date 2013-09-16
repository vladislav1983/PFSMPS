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
#ifdef __SYSTEM_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __SYSTEM_C 
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "system.h"
#include "alarm.h"
#include "kernelparam.h"
#include "sirem.h"
#include "dio.h"
#include "measure.h"
#include "sirem.h"
#include "statemachine.h"
#include "timer.h"
#include "tools.h"
#include "vadc.h"
#include "vcmp.h"
#include "vpwm.h"
#include "vuart.h"
#include "singleshot.h"
#if FREEMASTER_USED
    #include "freemaster.h"
    #include "freemaster_main.h"
    #include "freemaster_cfg.h"
#endif

/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/
#define cRefreshWatchDogPeriodUs            5000UL

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static tOSAlarm RefreshWatchDogAlarm;

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
static void ClockInit(void);


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
void System_Init(void)
{
// drivers initialization
#ifdef _Timer1Init
    _Timer1Init();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _SoftTimersInit
    _SoftTimersInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _DSP_Engine_Init
    _DSP_Engine_Init();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _AdcInit
    _AdcInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _CMPInit
    _CMPInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _PWMInit
    _PWMInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _UartInit
    _UartInit();
#else
    #warning "Module not initialized!!!"
#endif
// application initialization
#ifdef _System_WDT_Init
    _System_WDT_Init();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _System_StreamsInit
    _System_StreamsInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _DioInit
    _DioInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _MeasureInit
    _MeasureInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _FreeMasterMainInit
    _FreeMasterMainInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _SiremInit
    _SiremInit();
#else
    #warning "Module not initialized!!!"
#endif
#ifdef _StateMachineInit
    _StateMachineInit();
#else
    #warning "Module not initialized!!!"
#endif
#if SINGLESHOT_USED != 0
    #ifdef _SingleShotInit
        PFC_SingleshotProcess();
    #else
        #warning "Module not initialized!!!"
    #endif
#endif
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void System_StreamsInit(void)
{
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void System_EnterSleep(void)
{
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void System_LeaveSleep(void)
{
}
/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void _system_lowlevel_init(void)
{   
    _WatchDOgDisable();
    /* Init CPU clock before startup OS */
    ClockInit(); 
    _WatchDOgEnable();
} 

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void System_WDT_Init(void)
{
    /* Init refresh watchdog task */
    RefreshWatchDogAlarm.TaskID = cRefreshWatchDogTaskId;
    OsSetAlarm(&RefreshWatchDogAlarm, cRefreshWatchDogPeriodUs/cOsAlarmTickUs);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void RefreshWatchDogTask(void)
{
    asm volatile ("clrwdt");
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void IdleTask(void)
{
    SiremEngine();
#if FREEMASTER_USED && (FMSTR_SHORT_INTR || FMSTR_POLL_DRIVEN)
    FMSTR_Poll();
#endif
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
static void ClockInit(void)
{
    /* 
    ** Set up the oscillator for 39.614 MHz device operation frequency
    ** Configure Oscillator to operate the device at approx ~40Mhz
    ** Fosc= Fin*M/(N1*N2), Fcy=Fosc/2 where, M = PLLFBD + 2,  N1 = 2,  N2 = 2
    ** Fosc= 7.37*(43)/(2*2)=79.225 Mhz for Fosc, Fcy = 39.614 Mhz
    */
    /* Configure PLL prescaler, PLL postscaler, PLL divisor */
    PLLFBD=41; 				/* M = PLLFBD + 2 */
    CLKDIVbits.PLLPOST=0;   /* N1 = 2 */
    CLKDIVbits.PLLPRE=0;    /* N2 = 2 */

    __builtin_write_OSCCONH(0x01);		/* New Oscillator FRC w/ PLL */
    __builtin_write_OSCCONL(0x01);  	/* Enable Switch */

    while(OSCCONbits.COSC != 0b001);	/* Wait for new Oscillator to become FRC w/ PLL */  
    while(OSCCONbits.LOCK != 1);		/* Wait for Pll to Lock */

    /* Now setup the ADC and PWM clock for 120MHz
	   ((FRC * 16) / APSTSCLR ) = (7.37 * 16) / 1 = 117.9MHz*/
	
	ACLKCONbits.FRCSEL = 1;					/* FRC provides Auxiliary PLL */
	ACLKCONbits.SELACLK = 1;				/* Auxiliary Oscillator provide clock source */
	ACLKCONbits.APSTSCLR = 7;				/* Divide Auxiliary clock by 1 */
	ACLKCONbits.ENAPLL = 1;					/* Enable Auxiliary PLL */
	
	while(ACLKCONbits.APLLCK != 1);			/* Wait for Auxiliary PLL to Lock */

}

