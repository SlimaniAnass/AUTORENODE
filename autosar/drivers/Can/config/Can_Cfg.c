#include "Can_Cfg.h"

static const Can_ControllerConfigType Can_ControllerConfigs[] = {
    {
        .ControllerId   = CAN_CONTROLLER_0,
        .Baudrate       = CAN_BAUDRATE_500K,
        .LoopbackMode   = CAN_LOOPBACK_ENABLED,
        .NumTxMailboxes = CAN_NUM_TX_MAILBOXES,
        .NumRxMailboxes = CAN_NUM_RX_MAILBOXES,
    }
};

const Can_ConfigType Can_DefaultConfig = {
    .NumControllers   = 1u,
    .ControllerConfig = Can_ControllerConfigs,
};
