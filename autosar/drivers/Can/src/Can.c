/**
 * @file    Can.c
 * @brief   AUTOSAR CAN Driver — SWS_CAN R4.3
 *          Target: NXP S32K144 FlexCAN
 * @author  Slimani Anass  —  Driver CAN AUTOSAR Compliant
 */

#include "Can.h"
#include "Can_Cfg.h"
#include "Can_GeneralTypes.h"
#include "S32K144.h"

/* CanIf callbacks — déclarés dans CanIf_Cbk.h */
extern void CanIf_TxConfirmation(PduIdType CanTxPduId);
extern void CanIf_RxIndication(Can_HwHandleType Hrh,
                                Can_IdType CanId,
                                uint8 CanDlc,
                                const uint8* CanSduPtr);
extern void CanIf_ControllerBusOff(uint8 ControllerId);

/* ══════════════════════════════════════════
 * Internal state
 * ══════════════════════════════════════════ */
typedef struct {
    Can_ControllerStateType state;
    const Can_ConfigType*   config;
    boolean                 initialized;
} Can_ControllerInternalType;

static Can_ControllerInternalType
    Can_Controllers[CAN_CONTROLLER_CNT];

/* Map controller ID → FlexCAN peripheral */
static FlexCAN_Type* const Can_HwBase[CAN_CONTROLLER_CNT] = {
    CAN0, CAN1, CAN2
};

/* ══════════════════════════════════════════
 * Internal helpers
 * ══════════════════════════════════════════ */

static void Can_EnableClock(uint8 Controller) {
    switch(Controller) {
        case 0: PCC_FlexCAN0 |= PCC_CGC; break;
        case 1: PCC_FlexCAN1 |= PCC_CGC; break;
        case 2: PCC_FlexCAN2 |= PCC_CGC; break;
        default: break;
    }
}

static void Can_EnterFreezeMode(FlexCAN_Type* can) {
    can->MCR |= (FLEXCAN_MCR_FRZ | FLEXCAN_MCR_HALT);
    /* Wait for freeze acknowledge */
    while(!(can->MCR & FLEXCAN_MCR_FRZACK));
}

static void Can_ExitFreezeMode(FlexCAN_Type* can) {
    can->MCR &= ~(FLEXCAN_MCR_FRZ | FLEXCAN_MCR_HALT);
    /* Wait until not frozen */
    while(can->MCR & FLEXCAN_MCR_FRZACK);
}

/**
 * Configure baud rate for 500 kbit/s @ 8 MHz CAN clock
 * Tq = 1/8MHz = 125ns
 * Bit time = 16 Tq → 500 kbit/s
 * PRESDIV=0 (div by 1), PROPSEG=3, PSEG1=4, PSEG2=4, RJW=1
 */
static void Can_ConfigureBaudrate(FlexCAN_Type* can, uint32 baudrate) {
    (void)baudrate; /* Only 500k supported for now */
    can->CTRL1 = FLEXCAN_CTRL1_PRESDIV(0)
               | FLEXCAN_CTRL1_RJW(1)
               | FLEXCAN_CTRL1_PSEG1(4)
               | FLEXCAN_CTRL1_PSEG2(4)
               | FLEXCAN_CTRL1_PROPSEG(3);
}

static void Can_InitMailboxes(FlexCAN_Type* can,
                               const Can_ControllerConfigType* cfg) {
    uint8 i;

    /* Disable all MBs first */
    for(i = 0; i < 16u; i++) {
        can->MB[i].CS = MB_CS_CODE(MB_CS_CODE_TX_INACTIVE);
        can->MB[i].ID = 0u;
    }

    /* Configure TX mailboxes */
    for(i = 0; i < cfg->NumTxMailboxes; i++) {
        can->MB[i].CS = MB_CS_CODE(MB_CS_CODE_TX_INACTIVE);
    }

    /* Configure RX mailboxes */
    for(i = 0; i < cfg->NumRxMailboxes; i++) {
        uint8 rxMb = cfg->NumTxMailboxes + i;
        can->MB[rxMb].CS = MB_CS_CODE(MB_CS_CODE_RX_EMPTY);
        can->MB[rxMb].ID = 0u;  /* Accept all */
        can->RXIMR[rxMb] = 0u; /* No filtering */
    }
}

/* ══════════════════════════════════════════
 * AUTOSAR API Implementation
 * ══════════════════════════════════════════ */

/**
 * Can_Init — SWS_CAN_00223
 * Initialise le driver CAN et le contrôleur.
 */
void Can_Init(const Can_ConfigType* Config) {
    uint8 i;

    if(Config == NULL_PTR) {
        return; /* DET error in full AUTOSAR */
    }

    for(i = 0; i < Config->NumControllers; i++) {
        const Can_ControllerConfigType* ctrlCfg =
            &Config->ControllerConfig[i];
        FlexCAN_Type* can = Can_HwBase[ctrlCfg->ControllerId];

        /* 1. Enable peripheral clock */
        Can_EnableClock(ctrlCfg->ControllerId);

        /* 2. Disable module to allow configuration */
        can->MCR |= FLEXCAN_MCR_MDIS;
        while(!(can->MCR & FLEXCAN_MCR_LPMACK));

        /* 3. Enable module and enter freeze mode */
        can->MCR &= ~FLEXCAN_MCR_MDIS;
        while(can->MCR & FLEXCAN_MCR_LPMACK);
        Can_EnterFreezeMode(can);

        /* 4. Soft reset */
        can->MCR |= FLEXCAN_MCR_SOFTRST;
        while(can->MCR & FLEXCAN_MCR_SOFTRST);

        /* 5. Configure MCR */
        can->MCR = FLEXCAN_MCR_FRZ
                 | FLEXCAN_MCR_HALT
                 | FLEXCAN_MCR_WRNEN
                 | FLEXCAN_MCR_MAXMB(15u);

        /* 6. Loopback mode si activé (test sans hardware) */
        if(ctrlCfg->LoopbackMode == STD_ON) {
            can->CTRL1 |= FLEXCAN_CTRL1_LPB;
        }

        /* 7. Configure baudrate */
        Can_ConfigureBaudrate(can, ctrlCfg->Baudrate);

        /* 8. Configure mailboxes */
        Can_InitMailboxes(can, ctrlCfg);

        /* 9. Accept all frames (global mask = 0) */
        can->RXMGMASK  = 0u;
        can->RX14MASK  = 0u;
        can->RX15MASK  = 0u;

        /* 10. Exit freeze mode */
        Can_ExitFreezeMode(can);

        /* Update internal state */
        Can_Controllers[ctrlCfg->ControllerId].state       = CAN_CS_STOPPED;
        Can_Controllers[ctrlCfg->ControllerId].config      = Config;
        Can_Controllers[ctrlCfg->ControllerId].initialized = TRUE;
    }
}

/**
 * Can_DeInit — SWS_CAN_00186
 */
void Can_DeInit(void) {
    uint8 i;
    for(i = 0; i < CAN_CONTROLLER_CNT; i++) {
        if(Can_Controllers[i].initialized == TRUE) {
            FlexCAN_Type* can = Can_HwBase[i];
            can->MCR |= FLEXCAN_MCR_MDIS;
            Can_Controllers[i].state       = CAN_CS_UNINIT;
            Can_Controllers[i].initialized = FALSE;
        }
    }
}

/**
 * Can_SetControllerMode — SWS_CAN_00261
 */
Std_ReturnType Can_SetControllerMode(uint8 Controller,
                                      Can_ControllerStateType Transition) {
    FlexCAN_Type* can;

    if(Controller >= CAN_CONTROLLER_CNT) {
        return E_NOT_OK;
    }
    if(Can_Controllers[Controller].initialized == FALSE) {
        return E_NOT_OK;
    }

    can = Can_HwBase[Controller];

    switch(Transition) {
        case CAN_CS_STARTED:
            Can_ExitFreezeMode(can);
            Can_Controllers[Controller].state = CAN_CS_STARTED;
            break;

        case CAN_CS_STOPPED:
            Can_EnterFreezeMode(can);
            Can_Controllers[Controller].state = CAN_CS_STOPPED;
            break;

        case CAN_CS_SLEEP:
            can->MCR |= FLEXCAN_MCR_MDIS;
            while(!(can->MCR & FLEXCAN_MCR_LPMACK));
            Can_Controllers[Controller].state = CAN_CS_SLEEP;
            break;

        default:
            return E_NOT_OK;
    }

    return E_OK;
}

/**
 * Can_GetControllerMode — SWS_CAN_00229
 */
void Can_GetControllerMode(uint8 Controller,
                            Can_ControllerStateType* ControllerModePtr) {
    if((Controller < CAN_CONTROLLER_CNT) &&
       (ControllerModePtr != NULL_PTR)) {
        *ControllerModePtr = Can_Controllers[Controller].state;
    }
}

/**
 * Can_Write — SWS_CAN_00233
 * Envoie un PDU CAN via le mailbox TX.
 */
Can_ReturnType Can_Write(Can_HwHandleType Hth,
                          const Can_PduType* PduInfo) {
    FlexCAN_Type* can;
    uint8 txMb;
    uint8 i;

    if(PduInfo == NULL_PTR) {
        return CAN_NOT_OK;
    }

    /* Pour ce projet : Hth = ControllerId */
    if(Hth >= CAN_CONTROLLER_CNT) {
        return CAN_NOT_OK;
    }

    if(Can_Controllers[Hth].state != CAN_CS_STARTED) {
        return CAN_NOT_OK;
    }

    can  = Can_HwBase[Hth];
    txMb = 0u; /* TX mailbox 0 */

    /* Check mailbox available */
    uint8 code = (uint8)((can->MB[txMb].CS >> 24u) & 0xFu);
    if(code != (uint8)MB_CS_CODE_TX_INACTIVE) {
        return CAN_BUSY;
    }

    /* Load ID */
    can->MB[txMb].ID = (PduInfo->id & 0x7FFu) << 18u; /* Standard 11-bit */

    /* Load data */
    for(i = 0u; i < PduInfo->length && i < 8u; i++) {
        can->MB[txMb].DATA[i] = PduInfo->sdu[i];
    }

    /* Set CS — activate TX */
    can->MB[txMb].CS = MB_CS_CODE(MB_CS_CODE_TX_DATA)
                     | MB_CS_DLC(PduInfo->length);

    return CAN_OK;
}

/**
 * Can_MainFunction_Write — SWS_CAN_00225
 * Appelée cycliquement par le scheduler AUTOSAR.
 * Vérifie si TX est terminé et notifie CanIf.
 */
void Can_MainFunction_Write(void) {
    uint8 ctrl;

    for(ctrl = 0u; ctrl < CAN_CONTROLLER_CNT; ctrl++) {
        if(Can_Controllers[ctrl].state != CAN_CS_STARTED) {
            continue;
        }

        FlexCAN_Type* can = Can_HwBase[ctrl];

        /* Check TX flag for mailbox 0 */
        if(can->IFLAG1 & (1u << 0u)) {
            /* Clear flag */
            can->IFLAG1 = (1u << 0u);
            /* Notify CanIf — TX confirmed */
            CanIf_TxConfirmation(0u);
        }
    }
}

/**
 * Can_MainFunction_Read — SWS_CAN_00226
 * Appelée cycliquement. Lit les mailboxes RX et notifie CanIf.
 */
void Can_MainFunction_Read(void) {
    uint8 ctrl;

    for(ctrl = 0u; ctrl < CAN_CONTROLLER_CNT; ctrl++) {
        if(Can_Controllers[ctrl].state != CAN_CS_STARTED) {
            continue;
        }

        const Can_ControllerConfigType* cfg =
            &Can_Controllers[ctrl].config->ControllerConfig[ctrl];
        FlexCAN_Type* can = Can_HwBase[ctrl];
        uint8 rxMbStart   = cfg->NumTxMailboxes;
        uint8 rxMbEnd     = rxMbStart + cfg->NumRxMailboxes;
        uint8 mb;

        for(mb = rxMbStart; mb < rxMbEnd; mb++) {
            /* Check if mailbox has data */
            uint8 code = (uint8)((can->MB[mb].CS >> 24u) & 0xFu);
            if(code == (uint8)MB_CS_CODE_RX_FULL) {
                uint8  dlc   = (uint8)((can->MB[mb].CS >> 16u) & 0xFu);
                uint32 id    = (can->MB[mb].ID >> 18u) & 0x7FFu;
                uint8  data[8u];
                uint8  i;

                for(i = 0u; i < dlc && i < 8u; i++) {
                    data[i] = can->MB[mb].DATA[i];
                }

                /* Lock release — read timer to unlock MB */
                (void)can->TIMER;

                /* Set MB back to empty */
                can->MB[mb].CS = MB_CS_CODE(MB_CS_CODE_RX_EMPTY);

                /* Notify CanIf */
                CanIf_RxIndication((Can_HwHandleType)mb,
                                   (Can_IdType)id,
                                   dlc,
                                   data);
            }
        }
    }
}

/**
 * Can_MainFunction_BusOff — SWS_CAN_00227
 */
void Can_MainFunction_BusOff(void) {
    uint8 ctrl;

    for(ctrl = 0u; ctrl < CAN_CONTROLLER_CNT; ctrl++) {
        if(Can_Controllers[ctrl].state != CAN_CS_STARTED) {
            continue;
        }

        FlexCAN_Type* can = Can_HwBase[ctrl];

        if(can->ESR1 & FLEXCAN_ESR1_BOFFINT) {
            /* Clear Bus-Off flag */
            can->ESR1 = FLEXCAN_ESR1_BOFFINT;

            /* Transition to STOPPED — AUTOSAR SWS_CAN_00272 */
            Can_Controllers[ctrl].state = CAN_CS_STOPPED;

            /* Notify CanIf */
            CanIf_ControllerBusOff(ctrl);
        }
    }
}

/**
 * Can_MainFunction_Wakeup — SWS_CAN_00228
 */
void Can_MainFunction_Wakeup(void) {
    /* Not implemented in this version */
}

/**
 * Can_MainFunction_Error — SWS_CAN_00350
 */
void Can_MainFunction_Error(void) {
    /* Not implemented in this version */
}

/**
 * Can_GetVersionInfo — SWS_CAN_00230
 */
void Can_GetVersionInfo(Std_VersionInfoType* versioninfo) {
    if(versioninfo != NULL_PTR) {
        versioninfo->vendorID         = CAN_VENDOR_ID;
        versioninfo->moduleID         = CAN_MODULE_ID;
        versioninfo->sw_major_version = CAN_SW_MAJOR_VERSION;
        versioninfo->sw_minor_version = CAN_SW_MINOR_VERSION;
        versioninfo->sw_patch_version = CAN_SW_PATCH_VERSION;
    }
}
