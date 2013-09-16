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
#ifdef __VPWM_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __VPWM_C
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "vpwm.h"
#include "smpspwm.h"
#include "pfcparam.h"
#include "smpsparam.h"
#include "tools.h"


/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/
#define _PWM_MODULE_ENABLE()        (PTCON |= PWM_MOD_EN)
#define _PWM_MODULE_DISABLE()       (PTCON &= (U16)~PWM_MOD_EN)

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
void PWMInit(void)
{
/*
** ===================== PWM secondary time base init =====================
*/
    STCON = STCON_VALUE;
    STCON = (PWM_SEVT_INT_DIS	    /* Special Event Interrupt Disable */
          |  PWM_PER_UPDATE_BOUND   /* Enable Period Update on cycle Boundary */
          |  PWM_PTB_SYNC_DIS	    /* Secondary Time Base Sync Disable */
          |  PWM_EXT_PTB_SYNC_DIS   /* External Primary Time Base Sync Disable */
          |  PWM_SEVT_OP_SCALE1);   /* Special Event Trig Output Postscaler 1:1  */ 

    // Secondary time base prescaler select
#if   cPwmSecondaryTimeBasePrescaler == 1
    STCON2 = PWM_INPUT_CLK_DIV0;
#elif cPwmSecondaryTimeBasePrescaler == 2
    STCON2 = PWM_INPUT_CLK_DIV1;
#elif cPwmSecondaryTimeBasePrescaler == 4
    STCON2 = PWM_INPUT_CLK_DIV2;
#elif cPwmSecondaryTimeBasePrescaler == 8
    STCON2 = PWM_INPUT_CLK_DIV3;
#elif cPwmSecondaryTimeBasePrescaler == 16
    STCON2 = PWM_INPUT_CLK_DIV4;
#elif cPwmSecondaryTimeBasePrescaler == 32
    STCON2 = PWM_INPUT_CLK_DIV5;
#elif cPwmSecondaryTimeBasePrescaler == 64
    STCON2 = PWM_INPUT_CLK_DIV6;
#else
    #error "Invalid secondary time base prescaler value"
#endif

    // Set STPER - secondary time base period register. Used for time base of PFC drive
    STPER = (U16)_round((U16)((cACLK_Mhz * 8.0 * cPfcPeriodUs)/cPwmSecondaryTimeBasePrescaler) - 8.0);
/*
** =====================     PWM4    =====================
** ===================== PFC PWM init=====================
** PWM4 use secondary time base
*/
    IOCON4 = IOCON4_VALUE;
    IOCON4 = (PWM4_H_PIN_EN                 /* PWM module controls PWM1H */
           |  PWM4_L_PIN_EN                 /* PWM module controls PWM1L */
           |  PWM4_H_PIN_ACTLOW	            /* PWM4H pin is low active */
           |  PWM4_L_PIN_ACTLOW	            /* PWM4L pin is low active */
           |  PWM4_IO_PIN_PAIR_TRUE_IND     /* PWM4 I/O Pair in True Independent output mode  */
           |  PWM4_ORENH_PWMGEN             /* PWM4 generator provides data for PWM4H pin  */
           |  PWM4_ORENL_PWMGEN             /* PWM generator provides data for PWM4L pin  */
           |  PWM4_ORENH1_OVRDAT_LL         /* If OVERNH=1 then OVRDAT<1>  provides data for PWM4H */
           |  PWM4_IFLT_EN_FLT_EN_FLTDAT_LL /* If Fault active, then FLTDAT<0> provides data for PWM4L */
           |  PWM4_CL_EN_CLDAT_LL           /* If Current Limit active  then CLDAT<0>  provides data for PWM4L */
           |  PWM4_PIN_SWAP_DIS             /* PWM4H and PWM4L pins are mapped to their respective pins */
           |  PWM4_OR_OVRDAT_NXT_CLK);      /* Output overrides via the OVRDAT<1:0> bits occur on next clk boundary */

    PWMCON4 = PWMCON4_VALUE;
    PWMCON4 = (PWM4_FLT_INT_DIS             /* PWM4 Fault Interrupt Disable */
            |  PWM4_CL_INT_DIS              /* PWM4 Current Limit Interrupt Disable  */
            |  PWM4_TRG_INT_DIS             /* PWM4 Trigger Interrupt Disable  */
            |  PWM4_TB_MODE_PTB             /* Primary time base (PTPER/STPER register) provides timing for PWM Gen   */
            |  PWM4_D_CYLE_DC4              /* Duty Cycle4 reg provides duty cycle info for PWM Gen        */
            |  PWM4_DT_DIS                  /* Dead time function disabled                  */
            |  PWM4_DT_COMD_UNUSED          /* DT compensation unused */
            |  PWM4_TIME_BASE_SECONDARY     /* PWM generator uses the secondary master time base */
            |  PWM4_CENTER_ALIGN_DIS        /* Center-aligned mode is Disabled */
            |  PWM4_EXT_RESET_DIS           /* External pins do not affect PWM time base   */
            |  PWM4_PDC_UPDATE_SYNC);       /* Updates to active PDC regs are sync with PWM time base */

    TRGCON4 = TRGCON4_VALUE;
#if SINGLESHOT_USED == 0
    TRGCON4 = (PWM4_TRIG_EVENT3             /* Trigger output on every 3rd trigger event */
            |  PWM4_TRIG_PS0);              /* Wait 0 PWM cycles before generating the first trigger event after the module is enabled */
#elif SINGLESHOT_USED == 1
    TRGCON4 = (PWM4_TRIG_EVENT1             /* Trigger output on every trigger event     */
            |  PWM4_TRIG_PS0);              /* Wait 0 PWM cycles before generating the first trigger event after the module is enabled */
#else
    #error "Configuration error"
#endif

    // Trigger special event of the middle of the PWM period
    TRIG4   = (U16)_round((((cACLK_Mhz * 8.0 * cPfcPeriodUs)/cPwmSecondaryTimeBasePrescaler) - 8.0) / 2.0);
    STRIG4  = (U16)_round((((cACLK_Mhz * 8.0 * cPfcPeriodUs)/cPwmSecondaryTimeBasePrescaler) - 8.0) / 2.0);
    // Primary channel (PWM4H) shifted to 0deg
    PHASE4 = 0;
    // Secondary channel shifted to 180deg
    SPHASE4 = (U16)_round((((cACLK_Mhz * 8.0 * cPfcPeriodUs)/cPwmSecondaryTimeBasePrescaler) - 8.0) / 2.0);
    // Set initial duty cycle of secondary PWM channel. PDC4 control PWM4H
    PDC4    = 0;
    // Set initial duty cycle of secondary PWM channel. SDC4 control PWM4L
    SDC4    = 0;

/*
** ===================== PWM primary time base init =====================
*/
    PTCON = PTCON_VALUE;
    PTCON = (PWM_MOD_DIS            /* PWM Module Disable */
          |  PWM_IDLE_CONT          /* PWM Time base in Idle mode Operation runs */
          |  PWM_SEVT_INT_DIS       /* Special Event Interrupt Disable */
          |  PWM_PER_UPDATE_BOUND   /* Enable Period Update on cycle Boundary */
          |  PWM_PTB_SYNC_DIS       /* Primary Time Base Sync Disable */
          |  PWM_EXT_PTB_SYNC_DIS   /* External Primary Time Base Sync Disable */
          |  PWM_SEVT_OP_SCALE1);   /* Special Event Trig Output Postscaler 1:1  */

    // Primary time base prescaler select
#if   cPwmPrimaryTimeBasePrescaler == 1
    PTCON2 = PWM_INPUT_CLK_DIV0;
#elif cPwmPrimaryTimeBasePrescaler == 2
    PTCON2 = PWM_INPUT_CLK_DIV1;
#elif cPwmPrimaryTimeBasePrescaler == 4
    PTCON2 = PWM_INPUT_CLK_DIV2;
#elif cPwmPrimaryTimeBasePrescaler == 8
    PTCON2 = PWM_INPUT_CLK_DIV3;
#elif cPwmPrimaryTimeBasePrescaler == 16
    PTCON2 = PWM_INPUT_CLK_DIV4;
#elif cPwmPrimaryTimeBasePrescaler == 32
    PTCON2 = PWM_INPUT_CLK_DIV5;
#elif cPwmPrimaryTimeBasePrescaler == 64
    PTCON2 = PWM_INPUT_CLK_DIV6;
#else
    #error "Invalid primary time base prescaller value"
#endif

    // Set PTPER - primary time base period register. Used for time base of ZVT drive
    PTPER = (U16)_round(((cACLK_Mhz * 8.0 * cSmpsPeriodUs)/cPwmPrimaryTimeBasePrescaler) - 8.0);
    // Master duty cycle - it give a duty cycle of PWM1 and PWM2. 50% duty cycle
    MDC = (U16)_round( (((cACLK_Mhz * 8.0 * cSmpsPeriodUs)/cPwmPrimaryTimeBasePrescaler) - 8.0) / 2.0 );
/*
** =====================     PWM1    =====================
** =========== SMPS ZVT Drive first leg init =============
** PWM1 use primary time base
*/
    IOCON1 = IOCON1_VALUE;
    IOCON1 = (PWM1_H_PIN_EN                 /* PWM module controls PWM1H */
           |  PWM1_L_PIN_EN                 /* PWM module controls PWM1L */
           |  PWM1_H_PIN_ACTHIGH            /* PWM1H pin is high active */
           |  PWM1_L_PIN_ACTHIGH            /* PWM1L pin is high active */
           |  PWM1_IO_PIN_PAIR_COMP	        /* PWM1 I/O Pair in complementary output mode  */
           |  PWM1_ORENH_PWMGEN             /* PWM1 generator provides data for PWM1H pin  */
           |  PWM1_ORENL_PWMGEN             /* PWM1 generator provides data for PWM1L pin  */
           |  PWM1_ORENH1_OVRDAT_LL	        /* If OVERNH=1 then OVRDAT<0>  provides data for PWM1L */
           |  PWM1_FLT_EN_FLTDAT_LL         /* If Fault active  then FLTDAT<0>  provides data for PWM1L */
           |  PWM1_IFLT_EN_FLT_EN_FLTDAT_LL /* If Fault active, then FLTDAT<0> provides data for PWM1L */
           |  PWM1_CL_EN_CLDAT_LL           /* If Current Limit active  then CLDAT<0>  provides data for PWM1L */
           |  PWM1_PIN_SWAP_DIS             /* PWM1H and PWM1L pins are mapped to their respective pins */
           |  PWM1_OR_OVRDAT_NXT_CLK);      /* Output overrides via the OVRDAT<1:0> bits occur on next clk boundary */

    PWMCON1 = PWMCON1_VALUE;
    PWMCON1 = (PWM1_FLT_INT_DIS             /* PWM1 Fault Interrupt Disable */
            |  PWM1_CL_INT_DIS              /* PWM1 Current Limit Interrupt Disable  */
            |  PWM1_TRG_INT_DIS             /* PWM1 Trigger Interrupt Disable  */
            |  PWM1_TB_MODE_PTB             /* Primary time base (PTPER register) provides timing for PWM Gen   */
            |  PWM1_D_CYLE_MDC              /* Master Duty Cycle reg provides duty cycle info for PWM Gen  */
            |  PWM1_DT_POS                  /* Positive dead time applied to all o/p modes  */
            |  PWM1_CENTER_ALIGN_DIS        /* Center-aligned mode is Disabled*/
            |  PWM1_EXT_RES_DIS             /* External pins do not affect PWM time base   */
            |  PWM1_PDC_UPDATE_IMM);        /* Updates to active PDC regs are immediate  */

    TRGCON1 = TRGCON1_VALUE;
#if SINGLESHOT_USED == 0
    TRGCON1 = (PWM1_TRIG_EVENT2             /* Trigger output on every 2nd trigger event */
            |  PWM1_TRIG_PS0);              /* Wait 0 PWM cycles before generating the first trigger event after the module is enabled */
#elif SINGLESHOT_USED == 1
    TRGCON1 = (PWM1_TRIG_EVENT1             /* Trigger output on every trigger event     */
            |  PWM1_TRIG_PS0);              /* Wait 0 PWM cycles before generating the first trigger event after the module is enabled */
#else
    #error "Configuration error"
#endif

    // PWM1 primary sampling point -> center of the PWM - (sampling margin) -> Used to trigger ADC Pair 1
    TRIG1 = (U16)(_round( (((cACLK_Mhz * 8.0 * cSmpsPeriodUs)        /cPwmPrimaryTimeBasePrescaler) - 8.0) / 2.0)
          -       _round(   (cACLK_Mhz * 8.0 * cSmpsSamplingMarginUs)/cPwmPrimaryTimeBasePrescaler ));

    // PWM1 secondary sampling point -> end of PWM - (sampling margin) -> Used to trigger ADC Pair 0
    STRIG1 = (U16)(_round( ((cACLK_Mhz * 8.0 * cSmpsPeriodUs)        /cPwmPrimaryTimeBasePrescaler) - 8.0)
           -       _round(  (cACLK_Mhz * 8.0 * cSmpsSamplingMarginUs)/cPwmPrimaryTimeBasePrescaler ));

    // Dead time DTR1 and ALTDTR1 are 14 bit wide registers
    DTR1    = (U16)((U16)_round( (cACLK_Mhz * 8.0 * cSmpsDeadTimeUs)/cPwmPrimaryTimeBasePrescaler ) & 0x3FFF);
    ALTDTR1 = (U16)((U16)_round( (cACLK_Mhz * 8.0 * cSmpsDeadTimeUs)/cPwmPrimaryTimeBasePrescaler ) & 0x3FFF);

    // PWM1 zero phase
    PHASE1 = 0;
    
    // Fault control
    FCLCON1 = FCLCON1_VALUE;
    FCLCON1 = (PWM1_IND_FLT_DIS         /* Normal fault mode */
            |  PWM1_FLT_CMP1            /* Fault control signal select source CMP1 for PWM1 Gen bits */
            |  PWM1_FLT_SOURCE_HIGH	    /* Selected Fault source is active High  */
            |  PWM1_HL_FLTDAT_LATCH);   /* Selected Fault source forces PWM1H and PWM1L pins to FLTDAT values (Latched Mode) */

/*
** =====================     PWM2    =====================
** =========== SMPS ZVT Drive second leg init =============
** PWM2 use primary time base
*/
    IOCON2 = IOCON2_VALUE;
    IOCON2 = (PWM2_H_PIN_EN                 /* PWM module controls PWM2H */
           |  PWM2_L_PIN_EN                 /* PWM module controls PWM2L */
           |  PWM2_H_PIN_ACTHIGH            /* PWM2H pin is high active */
           |  PWM2_L_PIN_ACTHIGH            /* PWM2L pin is high active */
           |  PWM2_IO_PIN_PAIR_COMP	        /* PWM2 I/O Pair in complementary output mode  */
           |  PWM2_ORENH_PWMGEN             /* PWM2 generator provides data for PWM2H pin  */
           |  PWM2_ORENL_PWMGEN             /* PWM2 generator provides data for PWM2L pin  */
           |  PWM2_ORENH1_OVRDAT_LL	        /* If OVERNH=1 then OVRDAT<0>  provides data for PWM2L */
           |  PWM2_FLT_EN_FLTDAT_LL         /* If Fault active  then FLTDAT<0>  provides data for PWM2L */
           |  PWM2_IFLT_EN_FLT_EN_FLTDAT_LL /* If Fault active, then FLTDAT<0> provides data for PWM2L */
           |  PWM2_CL_EN_CLDAT_LL           /* If Current Limit active  then CLDAT<0>  provides data for PWM2L */
           |  PWM2_PIN_SWAP_DIS             /* PWM2H and PWM2L pins are mapped to their respective pins */
           |  PWM2_OR_OVRDAT_NXT_CLK);      /* Output overrides via the OVRDAT<1:0> bits occur on next clk boundary */

    PWMCON2 = PWMCON2_VALUE;
    PWMCON2 = (PWM2_FLT_INT_DIS             /* PWM2 Fault Interrupt Disable */
            |  PWM2_CL_INT_DIS              /* PWM2 Current Limit Interrupt Disable  */
            |  PWM2_TRG_INT_DIS             /* PWM2 Trigger Interrupt Disable  */
            |  PWM2_TB_MODE_PTB             /* Primary time base (PTPER register) provides timing for PWM Gen   */
            |  PWM2_D_CYLE_MDC              /* Master Duty Cycle reg provides duty cycle info for PWM Gen  */
            |  PWM2_DT_POS                  /* Positive dead time applied to all o/p modes  */
            |  PWM2_CENTER_ALIGN_DIS        /* Center-aligned mode is Disabled*/
            |  PWM2_EXT_RES_DIS             /* External pins do not affect PWM time base   */
            |  PWM2_PDC_UPDATE_IMM);        /* Updates to active PDC regs are immediate  */

    // Dead time DTR1 and ALTDTR1 are 14 bit wide registers
    DTR2    = (U16)((U16)_round( (cACLK_Mhz * 8.0 * cSmpsDeadTimeUs)/cPwmPrimaryTimeBasePrescaler ) & 0x3FFF);
    ALTDTR2 = (U16)((U16)_round( (cACLK_Mhz * 8.0 * cSmpsDeadTimeUs)/cPwmPrimaryTimeBasePrescaler ) & 0x3FFF);

    // PWM2 zero phase init. Used for control.
    PHASE2 = 0;
    
    // Fault control
    FCLCON2 = FCLCON2_VALUE;
    FCLCON2 = (PWM2_IND_FLT_DIS         /* Normal fault mode: Current-limit feature maps CLDAT<1:0> bits to the PWM2H and PWM2L outputs. 
                                                              The PWM fault feature maps FLTDAT<1:0> to the PWM2H and PWM2L outputs.*/	
            |  PWM2_FLT_CMP1            /* Fault control signal select source CMP1 for PWM2 Gen bits */
            |  PWM2_FLT_SOURCE_HIGH	    /* Selected Fault source is active High  */
            |  PWM2_HL_FLTDAT_LATCH);   /* Selected Fault source forces PWM2H and PWM2L pins to FLTDAT values (Latched Mode) */

    _PWM_MODULE_ENABLE();
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

