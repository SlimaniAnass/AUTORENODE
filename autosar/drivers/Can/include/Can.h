#ifndef CAN_H
#define CAN_H

#include "Std_Types.h"
#include "ComStack_Types.h"
#include "Can_GeneralTypes.h"

/* Version info */
#define CAN_VENDOR_ID           60u
#define CAN_MODULE_ID           80u
#define CAN_SW_MAJOR_VERSION    1u
#define CAN_SW_MINOR_VERSION    0u
#define CAN_SW_PATCH_VERSION    0u

/* Controller states — AUTOSAR SWS_CAN_00259 */
typedef enum {
    CAN_CS_UNINIT  = 0x00,
    CAN_CS_STARTED = 0x01,
    CAN_CS_STOPPED = 0x02,
    CAN_CS_SLEEP   = 0x03
} Can_ControllerStateType;

/* Return type — AUTOSAR SWS_CAN_00039 */
typedef enum {
    CAN_OK      = 0x00,
    CAN_NOT_OK  = 0x01,
    CAN_BUSY    = 0x02
} Can_ReturnType;

/* State transitions — AUTOSAR SWS_CAN_00017 */
typedef enum {
    CAN_T_START  = 0x01,
    CAN_T_STOP   = 0x02,
    CAN_T_SLEEP  = 0x03,
    CAN_T_WAKEUP = 0x04
} Can_StateTransitionType;

/* Hardware handle type */
typedef uint16 Can_HwHandleType;

/* CAN ID type */
typedef uint32 Can_IdType;

/* PDU type — ce que Can_Write reçoit */
typedef struct {
    PduIdType    swPduHandle;
    uint8        length;
    Can_IdType   id;
    uint8*       sdu;
} Can_PduType;

/* Hardware object type — pour CanIf_RxIndication */
typedef struct {
    Can_IdType       CanId;
    Can_HwHandleType Hoh;
    uint8            ControllerId;
} Can_HwType;

/* Forward declaration config type */
typedef struct Can_ConfigType_s Can_ConfigType;

/* ═══════════════════════════════════════════════
 * AUTOSAR SWS_CAN APIs — ce que tu implémentes
 * ═══════════════════════════════════════════════ */

/* SWS_CAN_00223 */
void Can_Init(const Can_ConfigType* Config);

/* SWS_CAN_00186 */
void Can_DeInit(void);

/* SWS_CAN_00230 */
void Can_GetVersionInfo(Std_VersionInfoType* versioninfo);

/* SWS_CAN_00261 */
Std_ReturnType Can_SetControllerMode(
    uint8 Controller,
    Can_ControllerStateType Transition
);

/* SWS_CAN_00229 */
void Can_GetControllerMode(
    uint8 Controller,
    Can_ControllerStateType* ControllerModePtr
);

/* SWS_CAN_00233 — fonction principale TX */
Can_ReturnType Can_Write(
    Can_HwHandleType Hth,
    const Can_PduType* PduInfo
);

/* SWS_CAN_00225 — polling TX */
void Can_MainFunction_Write(void);

/* SWS_CAN_00226 — polling RX */
void Can_MainFunction_Read(void);

/* SWS_CAN_00227 — polling Bus-Off */
void Can_MainFunction_BusOff(void);

/* SWS_CAN_00228 — polling Wakeup */
void Can_MainFunction_Wakeup(void);

/* SWS_CAN_00350 — error state */
void Can_MainFunction_Error(void);

#endif /* CAN_H */
