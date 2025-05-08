#include "tinyusb_ecm.h"

#include "tinyusb/device/usbd.h"
#include "tinyusb/device/usbd_pvt.h"

#include "tinyusb/tusb_option.h"
#include "tinyusb/class/cdc/cdc.h"

#include "fiber.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+
typedef struct
{
  uint8_t num; // Index number of Management Interface, +1 for Data Interface
  uint8_t act; // Alternate setting of Data Interface. 0 : inactive, 1 : active

  uint8_t ep_notif;
  uint8_t ep_in;
  uint8_t ep_out;

  // Endpoint descriptor use to open/close when receving SetInterface
  // TODO since configuration descriptor may not be long-lived memory, we should
  // keep a copy of endpoint attribute instead
  uint8_t const * ecm_desc_epdata;

} ecm_interface_t;

typedef struct {
  tusb_control_request_t header;
  uint32_t downlink, uplink;
} ecm_notify_t;

//--------------------------------------------------------------------+
// INTERNAL OBJECT & FUNCTION DECLARATION
//--------------------------------------------------------------------+
static ecm_interface_t ecm_itf;
static ecm_notify_t ecm_notify;

void ecm_notify_connect(bool state)
{
	ecm_notify.header = (tusb_control_request_t) {
		.bmRequestType = 0xA1,
		.bRequest = CDC_NOTIF_NETWORK_CONNECTION,
		.wValue = state,
		.wIndex = ecm_itf.num,
		.wLength = 0
	};
	if (usbd_edpt_busy(TUD_OPT_RHPORT, ecm_itf.ep_notif)) return;
	usbd_edpt_xfer(TUD_OPT_RHPORT, ecm_itf.ep_notif, (uint8_t*) &ecm_notify, sizeof(tusb_control_request_t));
}

void ecm_notify_speed(void)
{
	ecm_notify = (ecm_notify_t) {
		.header = {
			.bmRequestType = 0xA1,
			.bRequest = CDC_NOTIF_CONNECTION_SPEED_CHANGE,
			.wValue = 0,
			.wIndex = ecm_itf.num,
			.wLength = 8
		},
		.downlink = 2940000,
		.uplink = 2940000
	};
	if (usbd_edpt_busy(TUD_OPT_RHPORT, ecm_itf.ep_notif)) return;
	usbd_edpt_xfer(TUD_OPT_RHPORT, ecm_itf.ep_notif, (uint8_t*) &ecm_notify, sizeof(ecm_notify_t));
}

#include <malloc.h>

uint8_t* network_packets_get(void) { return malloc(CFG_TUD_NET_MTU); }
void network_packets_put(uint8_t* packet) { return free(packet); }

uint8_t* packets_get(void) __attribute__ ((weak, alias("network_packets_get")));
void packets_put(uint8_t* packet) __attribute__ ((weak, alias("network_packets_put")));

static uint8_t * ecm_xmit_ptr;
static int16_t ecm_xmit_len = -1;
static int16_t ecm_recv_len = -1;

void network_transmit_yield(uint8_t* packet, uint16_t length)
{
	while (ecm_xmit_len != 0 || !tud_ready()) fiber_yield();
	ecm_xmit_ptr = packet;
	ecm_xmit_len = length;
	usbd_edpt_xfer(TUD_OPT_RHPORT, ecm_itf.ep_in, packet, ecm_xmit_len);
	// transaction finished in ecm_xfer_cb
}

uint8_t* network_receive_yield(uint16_t* length)
{
	uint8_t* packet;
	while (ecm_recv_len != 0 || !tud_ready()) fiber_yield();
	ecm_recv_len = -1;
	packet = packets_get();
	usbd_edpt_xfer(TUD_OPT_RHPORT, ecm_itf.ep_out, packet, CFG_TUD_NET_MTU);
	while (ecm_recv_len <= 0) fiber_yield();
	*length = ecm_recv_len;
	ecm_recv_len = 0;
	return packet;
}

/*
void tud_suspend_cb(bool remote_wakeup_en)
{
	(void) remote_wakeup_en;
	if (ecm_xmit_len > 0) packets_put(ecm_xmit_ptr);
	ecm_xmit_len = -1;
}
void tud_resume_cb(void) { REG_PORT_OUTSET0 = PORT_PA19; }
*/

//--------------------------------------------------------------------+
// USBD Driver API
//--------------------------------------------------------------------+
void ecm_init(void)
{
	tu_memclr(&ecm_itf, sizeof(ecm_itf));
}

bool ecm_deinit(void)
{
	return true;
}

void ecm_reset(uint8_t rhport)
{
	(void) rhport;
	ecm_init();
}

uint16_t ecm_open(uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len)
{
	TU_VERIFY (
	TUSB_CLASS_CDC                           == itf_desc->bInterfaceClass &&
	CDC_COMM_SUBCLASS_ETHERNET_CONTROL_MODEL == itf_desc->bInterfaceSubClass &&
	0x00                                     == itf_desc->bInterfaceProtocol, 0);

	// confirm interface hasn't already been allocated
	TU_ASSERT (0 == ecm_itf.ep_notif, 0);

	//------------- Management Interface -------------//
	ecm_itf.num = itf_desc->bInterfaceNumber;

	uint16_t drv_len = sizeof(tusb_desc_interface_t);
	uint8_t const * p_desc = tu_desc_next(itf_desc);

	// Communication Functional Descriptors
	while (TUSB_DESC_CS_INTERFACE == tu_desc_type(p_desc) && drv_len <= max_len) {
		drv_len += tu_desc_len (p_desc);
		p_desc   = tu_desc_next(p_desc);
	}

	// notification endpoint (if any)
	if (TUSB_DESC_ENDPOINT == tu_desc_type(p_desc)) {
		TU_ASSERT(usbd_edpt_open(rhport, (tusb_desc_endpoint_t const *) p_desc), 0);
		ecm_itf.ep_notif = ((tusb_desc_endpoint_t const *) p_desc)->bEndpointAddress;
		drv_len += tu_desc_len (p_desc);
		p_desc   = tu_desc_next(p_desc);
	}

	//------------- Data Interface -------------//
	// CDC-ECM data interface can be in two states
	// - 0 : zero endpoints while inactive (default)
	// - 1 : IN & OUT endpoints for active networking
	TU_ASSERT(TUSB_DESC_INTERFACE == tu_desc_type(p_desc), 0);

	do {
		tusb_desc_interface_t const * data_itf_desc = (tusb_desc_interface_t const *) p_desc;
		TU_ASSERT(TUSB_CLASS_CDC_DATA == data_itf_desc->bInterfaceClass, 0);
		drv_len += tu_desc_len (p_desc);
		p_desc   = tu_desc_next(p_desc);
	} while ( (TUSB_DESC_INTERFACE == tu_desc_type(p_desc)) && (drv_len <= max_len) );

	// Pair of endpoints
	TU_ASSERT(TUSB_DESC_ENDPOINT == tu_desc_type(p_desc), 0);

	// ECM is in-active by default, save the endpoint attribute
	// for opening later when received setInterface
	ecm_itf.ecm_desc_epdata = p_desc;

	return drv_len += 2*sizeof(tusb_desc_endpoint_t);
}

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool ecm_control_xfer_cb (uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
	if (stage != CONTROL_STAGE_SETUP) return true;
	switch (request->bmRequestType_bit.type) {
	case TUSB_REQ_TYPE_STANDARD:
		switch (request->bRequest) {
		case TUSB_REQ_GET_INTERFACE: {
	        uint8_t const req_itfnum = (uint8_t) request->wIndex;
	        TU_VERIFY(ecm_itf.num+1 == req_itfnum);
	        tud_control_xfer(rhport, request, &ecm_itf.act, 1);
	    } break;
		case TUSB_REQ_SET_INTERFACE: {
			uint8_t const req_itfnum = (uint8_t) request->wIndex;
			uint8_t const req_alt    = (uint8_t) request->wValue;
			// Only valid for Data Interface with Alternate is either 0 or 1
			TU_VERIFY(ecm_itf.num+1 == req_itfnum && req_alt < 2);
			// ACM-ECM only: request to enable/disable network activities
			if ((ecm_itf.act = req_alt))
			{
	    	  // TODO since we don't actually close endpoint
	          // hack here to not re-open it
	          if ( ecm_itf.ep_in == 0 && ecm_itf.ep_out == 0 )
	          {
	            TU_ASSERT(ecm_itf.ecm_desc_epdata);
	            TU_ASSERT(usbd_open_edpt_pair(rhport, ecm_itf.ecm_desc_epdata, 2, TUSB_XFER_BULK, &ecm_itf.ep_out, &ecm_itf.ep_in) );
				// signal ready
	            ecm_recv_len = 0;
	            ecm_xmit_len = 0;
	            // TODO should have opposite callback for application to disable network
	            tud_network_init_cb();
	            ecm_notify_connect(true);
	          }
	        } else {
	          // close the endpoint pair TODO pretend that we did, this should cause
			  // no harm since host won't try to communicate with the endpoints again
	          // ecm_itf.ep_in = ecm_itf.ep_out = 0
	        }
	        tud_control_status(rhport, request);
		} break;
		default: return false; // unsupported request
	} break;
	case TUSB_REQ_TYPE_CLASS:
		TU_VERIFY (ecm_itf.num == request->wIndex);
		switch (request->bRequest) {
		case CDC_REQUEST_SET_ETHERNET_PACKET_FILTER:
			tud_control_xfer(rhport, request, NULL, 0); break;
		case CDC_REQUEST_GET_ETHERNET_STATISTIC:
			tud_control_xfer(rhport, request, NULL, 0); break;
		} break;
	default: return false; // unsupported request
	}
	return true;
}

bool ecm_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t length)
{
	(void) result;

	if (ep_addr == ecm_itf.ep_notif && sizeof(ecm_notify.header) == length) {
		ecm_notify_speed();
	}
	else if (ep_addr == ecm_itf.ep_out) { // packet received
		ecm_recv_len = length;
	}
	else if (ep_addr == ecm_itf.ep_in)  { // packet transmit
		if ( length && (0 == (length % CFG_TUD_NET_SIZE)) ) {
			usbd_edpt_xfer(rhport, ecm_itf.ep_in, NULL, 0); // ZLP needed
		}
		else { // transmission finished
			ecm_xmit_len = 0;
			packets_put(ecm_xmit_ptr);
		}
	}
	return true;
}
