/*=====================================================================================================================
 * 
 * Repository path:     $HeadURL$
 * Last committed:      $Revision$
 * Last changed by:     $Author$
 * Last changed date:   $Date$
 * ID:                  $Id$
 *
 *===================================================================================================================*/
#ifndef __MEASURE_H
#define __MEASURE_H

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file                
 *===================================================================================================================*/
#include "basedef.h"
#include "vadc.h"
#include "pfcparam.h"
#include "smpsparam.h"

/*=====================================================================================================================
 * Constant data                                                              
 *===================================================================================================================*/
#define cFastMeasureTaskPeriodUs        250UL

/*
** PFC measurement constants. All max values are referred to max ADC reference voltage (3.3V)
*/
#define cPFC_Vac_Max_V                  443.3
#define cPFC_Vdc_Max_V                  460.223
#define cPFC_Iac_Max_A                  32.410
#define cPFC_IT1_Max_A                  20.0
#define cPFC_IT2_Max_A                  20.0

/*
** SMPS measurement constants. All max values are referred to max ADC reference voltage (3.3V)
*/
#define cSMPS_VoutMax_V                 66.0
#define cSMPS_I_HBridge_A               10.341


/*
** Temperature measurement coefficients
** Sensor used - MCP9700
*/
#define cTempSensorOffsetVoltagemV      500UL   // voltage offset at 0deg
#define cTempSensorTemperatureCoeff     10UL

/*
** Trip measure constants
*/
#define cPFC_Vac_RMS_Trip_Min_V         90.0
#define cPFC_Vac_RMS_Trip_Max_V         240.0
#define cPFC_VdcLink_Trip_Min_V         126.0
#define cPFC_VdcLink_Trip_Max_V         415.0
#define cPFC_Iac_RMS_Trip_A             8.0
#define cPFC_IT1_RMS_Trip_A             4.0
#define cPFC_IT2_RMS_Trip_A             4.0
#define cSMPS_Vout_Trip_V               50.0
#define cSMPS_IH_Trip_A                 7.0

// Trip units in fractional format
#define cqPFC_Vac_Peak_Trip_Min_V       _Q15((cPFC_Vac_RMS_Trip_Min_V * SQRT2) / cPFC_Vac_Max_V)
#define cqPFC_Vac_Peak_Trip_Max_V       _Q15((cPFC_Vac_RMS_Trip_Max_V * SQRT2) / cPFC_Vac_Max_V)
#define cqPFC_Iac_Peak_Trip_A           _Q15((cPFC_Iac_RMS_Trip_A * SQRT2) / cPFC_Iac_Max_A)
#define cqPFC_IT1_Peak_Trip_A           _Q15((cPFC_IT1_RMS_Trip_A * SQRT2) / cPFC_IT1_Max_A)
#define cqPFC_IT2_Peak_Trip_A           _Q15((cPFC_IT2_RMS_Trip_A * SQRT2) / cPFC_IT2_Max_A)
#define cqPFC_VdcLink_Trip_Min_V        _Q15(cPFC_VdcLink_Trip_Min_V / cPFC_Vdc_Max_V)
#define cqPFC_VdcLink_Trip_Max_V        _Q15(cPFC_VdcLink_Trip_Max_V / cPFC_Vdc_Max_V)
#define cqSMPS_Vout_Trip_V              _Q15(cSMPS_Vout_Trip_V / cSMPS_VoutMax_V)
#define cqSMPS_IH_Trip_A                _Q15(cSMPS_IH_Trip_A / cSMPS_I_HBridge_A)


/*=====================================================================================================================
 * Exported type                                                             
 *===================================================================================================================*/
struct sMes
{
/*
** Measured values in fractional format
*/
    Q15 u16PfcVdc;

/*
** Measured real values
*/
    // PFC measurements
    U16 u16PfcVac_RMS_x10;
    U16 u16PfcVdc_x10;
    U16 u16PfcIac_x10;
    U16 u16PfcIT1_x10;
    U16 u16PfcIT2_x10;
    // SMPS ZVT measurements
    U16 u16SmpsVout_x10;
    U16 u16SmpsI_POS_x10;
    U16 u16SmpsI_NEG_x10;
    // Temperature feedback
    U16 u16TempFbk_x10;
};

/*=====================================================================================================================
 * Exported data                                                              
 *===================================================================================================================*/
extern struct sMes Mes;

// Freemaster TSA params
// extern U16 ADC_SMPS_IH_1_FBK;
// extern U16 ADC_SMPS_V_FBK;
// extern U16 ADC_SMPS_IH_2_FBK;
// extern U16 ADC_PFC_AC_VOLT_FBK;
// extern U16 ADC_PFC_AC_CURRENT_FBK;
// extern U16 ADC_PFC_T1_CURRENT_FBK;
// extern U16 ADC_PFC_T2_CURRENT_FBK;
// extern U16 ADC_PFC_DC_LINK_FBK;

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Macros                                                            
 *===================================================================================================================*/
#define _MeasureInit()          MeasureInit()
/*=====================================================================================================================
 * Exported functions                     				                        
 *===================================================================================================================*/
extern void MeasureInit(void);
extern void MeasureTimerCallback(void);
extern void FastMeasureTask(void);
extern void SlowMeasureTask(void);

#endif /* __MEASURE_H */
