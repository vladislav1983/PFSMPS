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
#ifdef __ADC_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __ADC_C
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "vadc.h"
#include "smpsadc.h"
#include "delay.h"
#include "freemaster_tsa.h"

/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/


/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/
#define _ADC_Enable()           ADCON |= (U16)ADC_MOD_EN
#define _ADC_Disable()          ADCON &= (U16)(~ADC_MOD_EN)

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/

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
void AdcInit(void)
{
    ADCON = ADCON_VALUE;
    ADCON = (ADC_MOD_DIS            /* A/D converter module disabled */
          |  ADC_IDLE_CONT          /* Continue device operation in idle mode */
          |  ADC_SLCLKDIV_DIS       /* Use the clock divider for high frequency ADC input clock*/
          |  ADC_DATA_FRA           /* Fractional Data */
          |  ADC_INT_EN_2CONV       /* Interrupt generated after 2nd conversion is completed */
          |  ADC_ORDER_EVEN_FST     /* EVEN numbered Analog I/P 1st converted followed by ODD numbered I/P */
          |  ADC_SAMP_SIM           /* Shared S&H sampled at the same time the dedicated S&H if shared S&H is not busy
								       If Shared S&H is busy at the time when dedicated S&H sampled, 
                                       then shared S&H will sample at the start of new conversion */
          |  ADC_SAMP_TRIG          /* Dedicated S&H circuit starts sampling when the trigger event is detected and completes the sampling process in 2 ADC clock cycles*/
          |  ADC_PLL_DIS_FADC_5);   /* ADC clock = FADC/6 = ~ 120MHz / 6 = 20MHz, 14*Tad = 1.4 MSPS, two SARs = 2.8 MSPS */ 

    // Clear ADC Status Register
    ADSTAT = 0;

    ADPCFG = 0xFFFF;    /* All pins in digital mode */
    ADPCFG =           /* PAIR 0 */
             (U16)(~ADC_PORT_PIN0_DIG   /* AN0 -> SMPS: I_SMPS */
           &       ~ADC_PORT_PIN1_DIG   /* AN1 -> SMPS: SMPS_V_FBK */
                       /* PAIR 1 */
           &       ~ADC_PORT_PIN2_DIG   /* AN2 -> SMPS: I_SMPS */
           &       ~ADC_PORT_PIN3_DIG   /* AN3 -> PFC: AC_SENSE */
                       /* PAIR 2 */
           &       ~ADC_PORT_PIN4_DIG   /* AN4 -> PFC: I_AC */
           &       ~ADC_PORT_PIN5_DIG   /* AN5 -> PFC: I_T1   */
                       /* PAIR 3 */
           &       ~ADC_PORT_PIN6_DIG   /* AN6 -> PFC: I_T2 */
           &       ~ADC_PORT_PIN7_DIG   /* AN7 -> PFC: DC_LINK_SENSE  */
                       /* PAIR 4 */
           &       ~ADC_PORT_PIN11_DIG); /* AN8 -> Temperature */

    // Pair 0 
    // PWM2H - primary channel trigger ADC conversion at the beginning of the SMPS ZVT PWM period
    // AN0 -> I_SMPS
    // AN1 -> SMPS_V_FBK
    ADCPC0 = ADCPC0_VALUE;
    ADCPC0 |= ADC_AN1_0_TRIG_SEC_PWM1; 
    // Pair 1
    // PWM1H - primary channel trigger ADC conversion at the middle of the SMPS ZVT PWM period
    // AN2 -> I_SMPS
    // AN3 -> AC_SENSE
    ADCPC0 |= ADC_AN3_2_TRIG_PRI_PWM1; 
    
    // Pair 2
    // PWM4H - primary channel trigger ADC conversion
    // AN4 -> I_AC
    // AN5 -> I_T1
    ADCPC1 = ADCPC1_VALUE;
    ADCPC1 |= ADC_AN5_4_TRIG_PRI_PWM4;
    
    // Pair 3
    // PWM4L - secondary channel trigger ADC conversion
    // AN5 -> I_T1
    // AN6 -> DC_LINK_SENSE
    ADCPC1 |= ADC_AN7_6_TRIG_SEC_PWM4; /* PWM generator #4 secondary trigger selected for AN7 & AN6 */

    // Pair 4
    // PWM4H - primary channel trigger ADC conversion
    // AN11 -> temperature
    ADCPC2 = ADCPC2_VALUE;
    ADCPC2 |= ADC_AN11_10_TRIG_PRI_PWM4; /* PWM generator #4 primary trigger selected for AN11 & AN10 */
              

    SetPriorityIntSmpsADCPair1(cADCPair1_ICR_Priority);
    SetPriorityIntSmpsADCPair2(cADCPair2_ICR_Priority);

    // Enable SMPS ADC interrupt
    SetIntSmpsADCPair1(cAdcIntEnable);
    //SetIntSmpsADCPair2(cAdcIntEnable);

    _ADC_Enable();
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

