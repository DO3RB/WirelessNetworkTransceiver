#pragma once

#ifndef TINYUSB_ACM_H
#define TINYUSB_ACM_H

#include <stdint.h>
#include <stdbool.h>

#include "tinyusb/class/cdc/cdc.h"

#ifdef __cplusplus
 extern "C" {
#endif

//--------------------------------------------------------------------+
// Application API
//--------------------------------------------------------------------+

bool     acm_opened (void); // check whether terminal connected
uint32_t acm_read   (uint8_t * buffer, uint32_t bufsize);
uint32_t acm_write  (uint8_t * buffer, uint32_t count);

//--------------------------------------------------------------------+
// Application Callback API (weak is optional)
//--------------------------------------------------------------------+

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
TU_ATTR_WEAK void acm_line_state_cb(bool dtr, bool rts);

// Invoked when line coding is change via SET_LINE_CODING
TU_ATTR_WEAK void acm_line_coding_cb(cdc_line_coding_t const* p_line_coding);

// Invoked when received send break
TU_ATTR_WEAK void acm_send_break_cb(uint16_t duration_ms);

//--------------------------------------------------------------------+
// INTERNAL USBD-CLASS DRIVER API
//--------------------------------------------------------------------+

void     acm_init            (void);
bool     acm_deinit          (void);
void     acm_reset           (uint8_t rhport);
uint16_t acm_open            (uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len);
bool     acm_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request);
bool     acm_xfer_cb         (uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------------------------+
// DEVICE DRIVER
//--------------------------------------------------------------------+

#if CFG_TUSB_DEBUG >= CFG_TUD_LOG_LEVEL
	#define DRIVER_NAME(_name) _name
#else
	#define DRIVER_NAME(_name) NULL
#endif

#define TINYUSB_ACM_DRIVER {\
	.name            = DRIVER_NAME("ACM"),\
	.init            = acm_init,\
	.deinit          = acm_deinit,\
	.reset           = acm_reset,\
	.open            = acm_open,\
	.control_xfer_cb = acm_control_xfer_cb,\
	.xfer_cb         = acm_xfer_cb,\
	.sof             = NULL}

#endif // TINYUSB_ACM_H
