#ifndef TINYUSB_ECM_H
#define TINYUSB_ECM_H

#include <stdint.h>
#include <stdbool.h>

#include "tinyusb/common/tusb_types.h"

/* declared here, NOT in usb_descriptors.c, so that the driver can intelligently ZLP as needed */
#define CFG_TUD_NET_SIZE (TUD_OPT_HIGH_SPEED ? 512 : 64)

/* Maximum Transmission Unit (in bytes) of the network, including Ethernet header */
#ifndef CFG_TUD_NET_MTU
#define CFG_TUD_NET_MTU 1516 // 1514
#endif

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------+
// CDC-ECM Descriptor Template
//--------------------------------------------------------------------+

// Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
#define TUD_CDC_ECM_DESC_STATS(_itfnum, _desc_stridx, _mac_stridx, _stats_cap, _ep_notif, _ep_notif_size, _epout, _epin, _epsize, _maxsegmentsize) \
  /* Interface Association */\
  8, TUSB_DESC_INTERFACE_ASSOCIATION, _itfnum, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ETHERNET_CONTROL_MODEL, 0, 0,\
  /* CDC Control Interface */\
  9, TUSB_DESC_INTERFACE, _itfnum, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ETHERNET_CONTROL_MODEL, 0, _desc_stridx,\
  /* CDC-ECM Header */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),\
  /* CDC-ECM Union */\
  5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, _itfnum, (uint8_t)((_itfnum) + 1),\
  /* CDC-ECM Functional Descriptor */\
  13, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ETHERNET_NETWORKING, _mac_stridx, U32_TO_U8S_LE(_stats_cap), U16_TO_U8S_LE(_maxsegmentsize), U16_TO_U8S_LE(0), 0,\
  /* Endpoint Notification */\
  7, TUSB_DESC_ENDPOINT, _ep_notif, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(_ep_notif_size), 1,\
  /* CDC Data Interface (default inactive) */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 0, 0, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
  /* CDC Data Interface (alternative active) */\
  9, TUSB_DESC_INTERFACE, (uint8_t)((_itfnum)+1), 1, 2, TUSB_CLASS_CDC_DATA, 0, 0, 0,\
  /* Endpoint In */\
  7, TUSB_DESC_ENDPOINT, _epin, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0,\
  /* Endpoint Out */\
  7, TUSB_DESC_ENDPOINT, _epout, TUSB_XFER_BULK, U16_TO_U8S_LE(_epsize), 0

//--------------------------------------------------------------------+
// Application API and Callback
//--------------------------------------------------------------------+

void network_transmit_yield (uint8_t * packet, uint16_t length);
uint8_t* network_receive_yield (uint16_t * length);

// client must provide this: initialize any network state back to the beginning
void tud_network_init_cb(void);

//--------------------------------------------------------------------+
// INTERNAL USBD-CLASS DRIVER API
//--------------------------------------------------------------------+
void     ecm_init            (void);
bool     ecm_deinit          (void);
void     ecm_reset           (uint8_t rhport);
uint16_t ecm_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
bool     ecm_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool     ecm_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);
void     ecm_report          (uint8_t *buf, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // TINYUSB_ECM_H
