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
#ifdef __MEASURE_C
    #error "!!! FileName ID. It should be Unique !!!"
#else
    #define __MEASURE_C
#endif

/*=====================================================================================================================
 * Included files to resolve specific definitions in this file
 *===================================================================================================================*/
#include "measure.h"
#include "vadc.h"
#include "kernelinclude.h"
#include "statemachine.h"


/*=====================================================================================================================
 * Local constants
 *===================================================================================================================*/
#define cSlowMeasureTaskPeriodUs        20E3

/*=====================================================================================================================
 * Local macros
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local types
 *===================================================================================================================*/

/*=====================================================================================================================
 * Local data
 *===================================================================================================================*/
static tOSAlarm SlowMeasureAlarm;
static U32 u32SmpsVout_filt = 0;
static U32 u32SmpsIH_NEG_filt = 0;
static U32 u32SmpsIH_POS_filt = 0;
static U32 u32PfcVac_filt = 0;
static U32 u32PfcVac_filt_sum = 0;
static U32 u32PfcDcLink_filt = 0;
static U32 u32PfcIAC_filt = 0;
static U32 u32PfcIAC_filt_sum = 0;
static U32 u32PfcIT1_filt = 0;
static U32 u32PfcIT2_filt = 0;

/*=====================================================================================================================
 * Constant Local Data
 *===================================================================================================================*/

/*=====================================================================================================================
 * Constant exported data                                                     
 *===================================================================================================================*/

/*=====================================================================================================================
 * Exported Data
 *===================================================================================================================*/
struct sMes Mes;

#if FREEMASTER_USED
// Freemaster TSA params
U16 ADC_SMPS_IH_1_FBK = 0;
U16 ADC_SMPS_V_FBK = 0;
U16 ADC_SMPS_IH_2_FBK = 0;
U16 ADC_PFC_AC_VOLT_FBK = 0;
U16 ADC_PFC_AC_CURRENT_FBK = 0;
U16 ADC_PFC_T1_CURRENT_FBK = 0;
U16 ADC_PFC_T2_CURRENT_FBK = 0;
U16 ADC_PFC_DC_LINK_FBK = 0;

// Freemaster TSA table
FMSTR_TSA_TABLE_BEGIN(MEASURE_TABLE)
    FMSTR_TSA_RO_VAR(ADC_SMPS_IH_1_FBK,         FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_SMPS_V_FBK,            FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_SMPS_IH_2_FBK,         FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_PFC_AC_VOLT_FBK,       FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_PFC_AC_CURRENT_FBK,    FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_PFC_T1_CURRENT_FBK,    FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_PFC_T2_CURRENT_FBK,    FMSTR_TSA_UINT16)
    FMSTR_TSA_RO_VAR(ADC_PFC_DC_LINK_FBK,       FMSTR_TSA_UINT16)

    FMSTR_TSA_RO_VAR(Mes.u16PfcVac_RMS_x10,     FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16PfcVdc_x10,         FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16PfcIac_x10,         FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16PfcIT1_x10,         FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16PfcIT2_x10,         FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16SmpsVout_x10,       FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16SmpsI_POS_x10,      FMSTR_TSA_USERTYPE(Mes))
    FMSTR_TSA_RO_VAR(Mes.u16SmpsI_NEG_x10,      FMSTR_TSA_USERTYPE(Mes))
FMSTR_TSA_TABLE_END()
#endif

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
 * Description: Measure module initialization
 *===================================================================================================================*/
void MeasureInit(void)
{
    SlowMeasureAlarm.TaskID = cSlowMeasureTaskId;
    OsSetAlarm(&SlowMeasureAlarm, cSlowMeasureTaskPeriodUs/cOsAlarmTickUs);
    memset(&Mes, 0, sizeof(Mes));
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void MeasureTimerCallback(void)
{
    OSActivateTask(cFastMeasureTaskId);
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void FastMeasureTask(void)
{
#if FREEMASTER_USED == 0
    U16 ADC_SMPS_IH_1_FBK;
    U16 ADC_SMPS_V_FBK;
    U16 ADC_SMPS_IH_2_FBK;
    U16 ADC_PFC_AC_VOLT_FBK;
    U16 ADC_PFC_AC_CURRENT_FBK;
    U16 ADC_PFC_T1_CURRENT_FBK;
    U16 ADC_PFC_T2_CURRENT_FBK;
    U16 ADC_PFC_DC_LINK_FBK;
#endif

    ADC_SMPS_IH_1_FBK      = _ADC_SMPS_IH_NEG_FBK;
    ADC_SMPS_V_FBK         = _ADC_SMPS_V_FBK;
    ADC_SMPS_IH_2_FBK      = _ADC_SMPS_IH_POS_FBK;
    ADC_PFC_AC_VOLT_FBK    = _ADC_PFC_AC_VOLT_FBK;
    ADC_PFC_AC_CURRENT_FBK = _ADC_PFC_AC_CURRENT_FBK;
    ADC_PFC_T1_CURRENT_FBK = _ADC_PFC_T1_CURRENT_FBK;
    ADC_PFC_T2_CURRENT_FBK = _ADC_PFC_T2_CURRENT_FBK;
    ADC_PFC_DC_LINK_FBK    = _ADC_PFC_DC_LINK_FBK;

    // PFC Vac measure
    #define  cPFC_VacFilter_us      (1000UL)
    u32PfcVac_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cPFC_VacFilter_us), (ADC_PFC_AC_VOLT_FBK - _Hi(u32PfcVac_filt)));
    u32PfcVac_filt_sum += _Hi(u32PfcVac_filt);

    // PFC DC-Link voltage measure
    #define cPFC_DC_LinkFilter_us   (5000UL)
    u32PfcDcLink_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cPFC_DC_LinkFilter_us), (ADC_PFC_DC_LINK_FBK - _Hi(u32PfcDcLink_filt)));

    // PFC AC current measure
    #define cPFC_I_AC_Filter_us     (1000UL)
    u32PfcIAC_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cPFC_I_AC_Filter_us), (ADC_PFC_AC_CURRENT_FBK - _Hi(u32PfcIAC_filt)));
    u32PfcIAC_filt_sum += _Hi(u32PfcIAC_filt);

    // PFC I_T1 current measure
    #define cPFC_I_T1_Filter_us     (20000UL)
    u32PfcIT1_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cPFC_I_T1_Filter_us), (ADC_PFC_T1_CURRENT_FBK - _Hi(u32PfcIT1_filt)));

    // PFC I_T2 current measure
    #define cPFC_I_T2_Filter_us     (20000UL)
    u32PfcIT2_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cPFC_I_T2_Filter_us), (ADC_PFC_T2_CURRENT_FBK - _Hi(u32PfcIT2_filt)));

    // SMPS Vout measure
    #define cSMPS_VoutFilter_us      (5000UL)
    u32SmpsVout_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cSMPS_VoutFilter_us), (ADC_SMPS_V_FBK - _Hi(u32SmpsVout_filt)));

    // SMPS I NEG H-Bridge measure
    #define cSMPS_IH_NEG_Filter_us   (5000UL)
    u32SmpsIH_NEG_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cSMPS_IH_NEG_Filter_us), (ADC_SMPS_IH_1_FBK - _Hi(u32SmpsIH_NEG_filt)));

    // SMPS I POS H-Bridge measure
    #define cSMPS_IH_POS_Filter_us   (5000UL)
    u32SmpsIH_POS_filt += (S32)_mulus(((65536.0 * cFastMeasureTaskPeriodUs) / cSMPS_IH_POS_Filter_us), (ADC_SMPS_IH_2_FBK - _Hi(u32SmpsIH_POS_filt)));
    
}

/*=====================================================================================================================
 * Parameters: void
 *
 * Return: void
 *
 * Description: 
 *===================================================================================================================*/
void SlowMeasureTask(void)
{
    Q15 qTmp;
/*
** PFC measurements
*/
    // PFC AC voltage feedback measure
    qTmp = _divud(u32PfcVac_filt_sum, (U16)(cSlowMeasureTaskPeriodUs/cFastMeasureTaskPeriodUs));
    u32PfcVac_filt_sum = 0;
    // Vrms = (Vavg*PI)/(2*SQRT2)
    qTmp = (Q15)((U32)_muluu(qTmp, (U16)_round((16384.0*PI)/(2*SQRT2))) >> 14);
    Mes.u16PfcVac_RMS_x10 = fmul_q15(qTmp, (U16)_round(cPFC_Vac_Max_V * 10.0));
    if(Mes.u16PfcVac_RMS_x10 < (U16)(cPFC_Vac_RMS_Trip_Min_V * 10.0))  _set_PFC_VAC_undervoltage_fault(1);
    if(Mes.u16PfcVac_RMS_x10 > (U16)(cPFC_Vac_RMS_Trip_Max_V * 10.0))  _set_PFC_VAC_overvoltage_fault(1);

    // PFC DC-Link voltage feedback
    Mes.u16PfcVdc_x10 = fmul_q15(_Hi(u32PfcDcLink_filt), (Q15)_round(cPFC_Vdc_Max_V * 10.0));
    if(Mes.u16PfcVdc_x10 < (U16)(cPFC_VdcLink_Trip_Min_V * 10.0))  _set_PFC_VDC_undervoltage_fault(1);
    if(Mes.u16PfcVdc_x10 > (U16)(cPFC_VdcLink_Trip_Max_V * 10.0))  _set_PFC_VDC_overvoltage_fault(1);

    // PFC I AC current feedback
    // Irms = (Iavg*PI)/(2*SQRT2)
    qTmp = _divud(u32PfcIAC_filt_sum, (U16)(cSlowMeasureTaskPeriodUs/cFastMeasureTaskPeriodUs));
    u32PfcIAC_filt_sum = 0;
    qTmp = (Q15)((U32)_muluu(qTmp, (U16)_round((16384.0*PI)/(2*SQRT2))) >> 14);
    Mes.u16PfcIac_x10 = fmul_q15(qTmp, (Q15)_round(cPFC_Iac_Max_A * 10.0));
    if(Mes.u16PfcIac_x10 > (U16)(cPFC_Iac_RMS_Trip_A * 10.0))  _set_PFC_IAC_overcurrent_fault(1);

    // PFC I_T1 current feedback
    Mes.u16PfcIT1_x10 = fmul_q15(_Hi(u32PfcIT1_filt), (Q15)(cPFC_IT1_Max_A * 10.0));

    // PFC I_T2 current feedback
    Mes.u16PfcIT2_x10 = fmul_q15(_Hi(u32PfcIT2_filt), (Q15)(cPFC_IT2_Max_A * 10.0));
/*
** SMPS measurements
*/
    // SMPS Vout voltage feedback
    Mes.u16SmpsVout_x10 = fmul_q15(_Hi(u32SmpsVout_filt), (Q15)_round(cSMPS_VoutMax_V * 10.0));

    // SMPS I-HBridge positive current feedback
    Mes.u16SmpsI_POS_x10 = fmul_q15(_Hi(u32SmpsIH_POS_filt), (Q15)(cSMPS_I_HBridge_A * 10.0));

    // SMPS I-HBridge negative current feedback
    Mes.u16SmpsI_NEG_x10 = fmul_q15(_Hi(u32SmpsIH_NEG_filt), (Q15)(cSMPS_I_HBridge_A * 10.0));
/*
** Temperature measurement
*/
#if cTempSensorTemperatureCoeff == 10UL
    Mes.u16TempFbk_x10 = fmul_q15(_ADC_TEMPEBK, (Q15)cAdcVoltRefmV) - (U16)cTempSensorOffsetVoltagemV;
#else
    Mes.u16TempFbk_x10 = (U16)(fmul_q15(_ADC_TEMPEBK, (Q15)(cAdcVoltRefmV * 10.0)) - (U16)cTempSensorOffsetVoltagemV) / cTempSensorTemperatureCoeff;
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
