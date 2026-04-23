#ifndef S32K144_H
#define S32K144_H

#include "Std_Types.h"

/* ═══════════════════════════════════════════
 * S32K144 — FlexCAN base addresses
 * Reference: S32K144 Reference Manual Rev 3
 * ═══════════════════════════════════════════ */

#define CAN0_BASE 0x40024000UL
#define CAN1_BASE 0x40025000UL
#define CAN2_BASE 0x4002B000UL

/* ── FlexCAN register offsets ── */
#define FLEXCAN_MCR_OFFSET 0x0000U      /* Module Configuration Register */
#define FLEXCAN_CTRL1_OFFSET 0x0004U    /* Control 1 Register */
#define FLEXCAN_TIMER_OFFSET 0x0008U    /* Free Running Timer */
#define FLEXCAN_ECR_OFFSET 0x001CU      /* Error Counter */
#define FLEXCAN_ESR1_OFFSET 0x0020U     /* Error and Status 1 */
#define FLEXCAN_IMASK1_OFFSET 0x0028U   /* Interrupt Masks 1 */
#define FLEXCAN_IFLAG1_OFFSET 0x0030U   /* Interrupt Flags 1 */
#define FLEXCAN_CTRL2_OFFSET 0x0034U    /* Control 2 Register */
#define FLEXCAN_ESR2_OFFSET 0x0038U     /* Error and Status 2 */
#define FLEXCAN_RXFGMASK_OFFSET 0x0048U /* Rx FIFO Global Mask */
#define FLEXCAN_RXFIR_OFFSET 0x004CU    /* Rx FIFO Information */
#define FLEXCAN_MB_OFFSET 0x0080U       /* Message Buffers start */

/* ── MCR bits ── */
#define FLEXCAN_MCR_MDIS (1UL << 31)        /* Module Disable */
#define FLEXCAN_MCR_FRZ (1UL << 30)         /* Freeze Enable */
#define FLEXCAN_MCR_RFEN (1UL << 29)        /* Rx FIFO Enable */
#define FLEXCAN_MCR_HALT (1UL << 28)        /* Halt FlexCAN */
#define FLEXCAN_MCR_NOTRDY (1UL << 27)      /* FlexCAN Not Ready */
#define FLEXCAN_MCR_SOFTRST (1UL << 25)     /* Soft Reset */
#define FLEXCAN_MCR_FRZACK (1UL << 24)      /* Freeze Mode Acknowledge */
#define FLEXCAN_MCR_SUPV (1UL << 23)        /* Supervisor Mode */
#define FLEXCAN_MCR_LPMACK (1UL << 20)      /* Low Power Mode Acknowledge */
#define FLEXCAN_MCR_WRNEN (1UL << 21)       /* Warning Interrupt Enable */
#define FLEXCAN_MCR_MAXMB(n) ((n) & 0x7FUL) /* Max Message Buffers */
#define FLEXCAN_MCR_LOOPBACK (1UL << 17)    /* Loop Back Mode */

/* ── CTRL1 bits ── */
#define FLEXCAN_CTRL1_BOFFMSK (1UL << 15) /* Bus Off Mask */
#define FLEXCAN_CTRL1_ERRMSK (1UL << 14)  /* Error Mask */
#define FLEXCAN_CTRL1_LPB (1UL << 12)     /* Loop Back Mode */
#define FLEXCAN_CTRL1_TWRNMSK (1UL << 11) /* Tx Warning Interrupt Mask */
#define FLEXCAN_CTRL1_RWRNMSK (1UL << 10) /* Rx Warning Interrupt Mask */
#define FLEXCAN_CTRL1_SMP (1UL << 7)      /* CAN Bit Sampling */
#define FLEXCAN_CTRL1_BOFFREC (1UL << 6)  /* Bus Off Recovery */
#define FLEXCAN_CTRL1_TSYN (1UL << 5)     /* Timer Sync */
#define FLEXCAN_CTRL1_LBUF (1UL << 4)     /* Lowest Buffer TX First */
#define FLEXCAN_CTRL1_LOM (1UL << 3)      /* Listen-Only Mode */

/* Timing bits dans CTRL1 */
#define FLEXCAN_CTRL1_PRESDIV(x) (((x) & 0xFFUL) << 24)
#define FLEXCAN_CTRL1_RJW(x) (((x) & 0x3UL) << 22)
#define FLEXCAN_CTRL1_PSEG1(x) (((x) & 0x7UL) << 19)
#define FLEXCAN_CTRL1_PSEG2(x) (((x) & 0x7UL) << 16)
#define FLEXCAN_CTRL1_PROPSEG(x) (((x) & 0x7UL) << 0)

/* ── ESR1 bits ── */
#define FLEXCAN_ESR1_BOFFINT (1UL << 2) /* Bus Off Interrupt */
#define FLEXCAN_ESR1_ERRINT (1UL << 1)  /* Error Interrupt */
#define FLEXCAN_ESR1_WAKINT (1UL << 0)  /* Wake-Up Interrupt */
#define FLEXCAN_ESR1_FLTCONF (3UL << 4) /* Fault Confinement State */
#define FLEXCAN_ESR1_BUSOFF (1UL << 2)  /* Bus-Off State */

/* ── Message Buffer structure ── */
typedef struct {
  volatile uint32 CS;     /* Control and Status */
  volatile uint32 ID;     /* Message ID */
  volatile uint8 DATA[8]; /* Data bytes */
} FlexCAN_MB_Type;

/* ── MB CS bits ── */
#define MB_CS_CODE(x) (((x) & 0xFUL) << 24)
#define MB_CS_CODE_RX_EMPTY 0x4UL          /* RX: empty, ready to receive */
#define MB_CS_CODE_RX_FULL 0x2UL           /* RX: full */
#define MB_CS_CODE_TX_DATA 0xCUL           /* TX: data frame */
#define MB_CS_CODE_TX_INACTIVE 0x8UL       /* TX: inactive */
#define MB_CS_IDE (1UL << 21)              /* ID Extended */
#define MB_CS_RTR (1UL << 20)              /* Remote TX Request */
#define MB_CS_DLC(x) (((x) & 0xFUL) << 16) /* Data Length Code */
#define MB_CS_TIMESTAMP (0xFFFFUL)         /* Timestamp mask */

/* ── FlexCAN peripheral accessor ── */
typedef struct {
  volatile uint32 MCR;
  volatile uint32 CTRL1;
  volatile uint32 TIMER;
  volatile uint32 RESERVED1;
  volatile uint32 RXMGMASK;
  volatile uint32 RX14MASK;
  volatile uint32 RX15MASK;
  volatile uint32 ECR;
  volatile uint32 ESR1;
  volatile uint32 IMASK2;
  volatile uint32 IMASK1;
  volatile uint32 IFLAG2;
  volatile uint32 IFLAG1;
  volatile uint32 CTRL2;
  volatile uint32 ESR2;
  volatile uint32 RESERVED2[2];
  volatile uint32 CRCR;
  volatile uint32 RXFGMASK;
  volatile uint32 RXFIR;
  volatile uint32 RESERVED3[24];
  FlexCAN_MB_Type MB[16]; /* 16 Message Buffers */
  volatile uint32 RESERVED4[448];
  volatile uint32 RXIMR[16]; /* Individual Rx Mask Registers */
} FlexCAN_Type;

#define CAN0 ((FlexCAN_Type *)CAN0_BASE)
#define CAN1 ((FlexCAN_Type *)CAN1_BASE)
#define CAN2 ((FlexCAN_Type *)CAN2_BASE)

/* ── PCC (Peripheral Clock Control) ── */
#define PCC_BASE 0x40065000UL
#define PCC_FlexCAN0 (*(volatile uint32 *)(PCC_BASE + 0x090U))
#define PCC_FlexCAN1 (*(volatile uint32 *)(PCC_BASE + 0x094U))
#define PCC_FlexCAN2 (*(volatile uint32 *)(PCC_BASE + 0x0ACU))
#define PCC_CGC (1UL << 30) /* Clock Gate Control */

#endif /* S32K144_H */
