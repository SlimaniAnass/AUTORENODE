#ifndef CAN_GENERAL_TYPES_H
#define CAN_GENERAL_TYPES_H

#include "Std_Types.h"

/* Error states — AUTOSAR */
typedef enum {
    CAN_ERRORSTATE_ACTIVE  = 0x00,
    CAN_ERRORSTATE_PASSIVE = 0x01,
    CAN_ERRORSTATE_BUSOFF  = 0x02
} Can_ErrorStateType;

/* Error types */
typedef enum {
    CAN_ERROR_BIT_MONITORING1 = 0x01,
    CAN_ERROR_BIT_MONITORING0 = 0x02,
    CAN_ERROR_BIT             = 0x03,
    CAN_ERROR_CHECK_ACK_FAILED= 0x04,
    CAN_ERROR_ACK_DELIMITER   = 0x05,
    CAN_ERROR_ARBITRATION_LOST= 0x06,
    CAN_ERROR_OVERLOAD        = 0x07
} Can_ErrorType;

#endif /* CAN_GENERAL_TYPES_H */
