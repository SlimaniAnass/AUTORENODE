/*
 * File: my_autosar_swc.c
 *
 * Code generated for Simulink model 'my_autosar_swc'.
 *
 * Model version                  : 7.0
 * Simulink Coder version         : 9.8 (R2022b) 13-May-2022
 * C/C++ source code generated on : Thu Apr 23 16:50:16 2026
 *
 * Target selection: autosar.tlc
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#include "my_autosar_swc.h"
#include "rtwtypes.h"

/* PublicStructure Variables for Internal Data */
ARID_DEF rtARID_DEF;                   /* '<Root>/Integrator' */

/* Model step function for TID0 */
void my_autosar_swc_Step(void)         /* Sample time: [1.0s, 0.0s] */
{
  float64 rtb_RateTransition;
  float64 rtb_Sum_n;

  /* RateTransition: '<Root>/RateTransition' */
  rtb_RateTransition = Rte_IrvIRead_my_autosar_swc_Step_IRV1();

  /* Outputs for Atomic SubSystem: '<S2>/SS2' */
  /* Sum: '<S4>/Sum' incorporates:
   *  Gain: '<S4>/Gain'
   *  Inport: '<Root>/In1_1s'
   */
  rtb_Sum_n = 2.0 * rtb_RateTransition +
    Rte_IRead_my_autosar_swc_Step_In1_1s_In1_1s();

  /* End of Outputs for SubSystem: '<S2>/SS2' */

  /* Outputs for Atomic SubSystem: '<S2>/SS1' */
  /* Outport: '<Root>/Out1' incorporates:
   *  Gain: '<S3>/Gain1'
   *  Gain: '<S3>/Gain2'
   *  Inport: '<Root>/In1_1s'
   *  Sum: '<S2>/Sum'
   *  Sum: '<S3>/Sum'
   */
  Rte_IWrite_my_autosar_swc_Step_Out1_Out1(5.0 *
    (Rte_IRead_my_autosar_swc_Step_In1_1s_In1_1s() + 3.0 * rtb_RateTransition) +
    rtb_Sum_n);

  /* End of Outputs for SubSystem: '<S2>/SS1' */

  /* Outport: '<Root>/Out2' */
  Rte_IWrite_my_autosar_swc_Step_Out2_Out2(rtb_Sum_n);
}

/* Model step function for TID1 */
void my_autosar_swc_Step1(void)        /* Sample time: [2.0s, 0.0s] */
{
  /* RateTransition: '<Root>/RateTransition' incorporates:
   *  DiscreteIntegrator: '<Root>/Integrator'
   */
  Rte_IrvIWrite_my_autosar_swc_Step1_IRV1(rtARID_DEF.Integrator_DSTATE);

  /* Update for DiscreteIntegrator: '<Root>/Integrator' incorporates:
   *  Inport: '<Root>/In2_2s'
   */
  rtARID_DEF.Integrator_DSTATE += 2.0 *
    Rte_IRead_my_autosar_swc_Step1_In2_2s_In2_2s();
}

/* Model initialize function */
void my_autosar_swc_Init(void)
{
  /* Outputs for Atomic SubSystem: '<Root>/Initialize Function' */
  /* StateWriter: '<S1>/State Writer' incorporates:
   *  Constant: '<S1>/Constant'
   */
  rtARID_DEF.Integrator_DSTATE = 1.0;

  /* End of Outputs for SubSystem: '<Root>/Initialize Function' */
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
