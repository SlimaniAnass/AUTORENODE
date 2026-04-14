/**
 * test_can_init.c
 * Test minimal : initialise le driver et vérifie l'état
 */
#include "Can.h"
#include "Can_Cfg.h"

/* Stub CanIf callbacks */
void CanIf_TxConfirmation(PduIdType id) { (void)id; }
void CanIf_RxIndication(Can_HwHandleType hrh, Can_IdType id,
                         uint8 dlc, const uint8* data) {
    (void)hrh; (void)id; (void)dlc; (void)data;
}
void CanIf_ControllerBusOff(uint8 ctrl) { (void)ctrl; }

int main(void) {
    Can_ControllerStateType mode;

    /* Test 1 : Init */
    Can_Init(&Can_DefaultConfig);

    /* Test 2 : GetControllerMode → doit être STOPPED après Init */
    Can_GetControllerMode(CAN_CONTROLLER_0, &mode);

    /* Test 3 : SetControllerMode → START */
    Std_ReturnType ret = Can_SetControllerMode(CAN_CONTROLLER_0,
                                               CAN_CS_STARTED);

    /* Test 4 : Write un frame */
    uint8 data[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    Can_PduType pdu = {
        .swPduHandle = 0,
        .length      = 4,
        .id          = 0x123,
        .sdu         = data
    };
    Can_ReturnType wr = Can_Write(CAN_HTH_0, &pdu);

     // while(1); 
    return 0;
}
