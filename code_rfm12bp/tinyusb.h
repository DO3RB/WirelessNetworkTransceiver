#pragma once

#ifndef TINYUSB_H
#define TINYUSB_H

#ifdef __cplusplus
extern "C" {}
#endif

//--------------------------------------------------------------------
// COMMON CONFIGURATION
//--------------------------------------------------------------------

#define TUD_OPT_RHPORT 0

#define CFG_TUSB_RHPORT0_MODE OPT_MODE_DEVICE
#define CFG_TUSB_MCU          OPT_MCU_SAMD21
#define CFG_TUSB_OS           OPT_OS_NONE
#define CFG_TUSB_DEBUG        0
#define CFG_TUD_ENABLED       1
#define CFG_TUD_MAX_SPEED     OPT_MODE_DEFAULT_SPEED
#define CFG_TUSB_MEM_SECTION

//--------------------------------------------------------------------
// DEVICE CONFIGURATION
//--------------------------------------------------------------------

#define CFG_TUD_ENDPOINT0_SIZE 64
#define CFG_TUD_ENDPOINT0_BUFSIZE 64

// CDC FIFO size of TX and RX
#define CFG_TUD_CDC_RX_BUFSIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)
#define CFG_TUD_CDC_TX_BUFSIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)

// Maximum Transmission Unit (in bytes) of the network, including Ethernet header
#define CFG_TUD_NET_MTU 1516 // 1514

#endif // TINYUSB_H
