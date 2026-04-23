#ifndef CAN_CFG_H
#define CAN_CFG_H

#include "Can.h"

/* Nombre de contrôleurs CAN sur le S32K144 */
#define CAN_CONTROLLER_CNT       3u   /* CAN0, CAN1, CAN2 */

/* IDs des contrôleurs */
#define CAN_CONTROLLER_0         0u
#define CAN_CONTROLLER_1         1u
#define CAN_CONTROLLER_2         2u

/* Contrôleur utilisé par défaut pour le projet */
#define CAN_DEFAULT_CONTROLLER   CAN_CONTROLLER_0

/* Nombre de mailboxes TX/RX */
#define CAN_NUM_TX_MAILBOXES     4u
#define CAN_NUM_RX_MAILBOXES     4u

/* Baudrate — 500 kbit/s */
#define CAN_BAUDRATE_500K        500000u

/* Mode loopback pour tests sans hardware */
#define CAN_LOOPBACK_ENABLED     STD_ON

/* Hardware Transmit Handle (HTH) */
#define CAN_HTH_0                0u

/* Hardware Receive Handle (HRH) */
#define CAN_HRH_0                0u

/* Configuration d'un contrôleur CAN */
typedef struct {
    uint8   ControllerId;
    uint32  Baudrate;
    boolean LoopbackMode;
    uint8   NumTxMailboxes;
    uint8   NumRxMailboxes;
} Can_ControllerConfigType;

/* Configuration globale du driver */
struct Can_ConfigType_s {
    uint8                        NumControllers;
    const Can_ControllerConfigType* ControllerConfig;
};

/* Config par défaut — définie dans Can_Cfg.c */
extern const Can_ConfigType Can_DefaultConfig;

#endif /* CAN_CFG_H */
