#include "tinyusb_acm.h"

#include "tinyusb/device/usbd.h"
#include "tinyusb/device/usbd_pvt.h"

// Level where CFG_TUSB_DEBUG must be at least for this driver is logged
#ifndef CFG_TUD_CDC_LOG_LEVEL
	#define CFG_TUD_CDC_LOG_LEVEL CFG_TUD_LOG_LEVEL
#endif

#define TU_LOG_DRV(...) TU_LOG(CFG_TUD_CDC_LOG_LEVEL, __VA_ARGS__)

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF
//--------------------------------------------------------------------+
enum
{
  BULK_PACKET_SIZE = (TUD_OPT_HIGH_SPEED ? 512 : 64)
};

typedef struct
{
  uint8_t itf_num;
  uint8_t ep_notif;
  uint8_t ep_in;
  uint8_t ep_out;

  // Bit 0:  DTR (Data Terminal Ready), Bit 1: RTS (Request to Send)
  uint8_t line_state;

  /*------------- From this point, data is not cleared by bus reset -------------*/
  TU_ATTR_ALIGNED(4) cdc_line_coding_t line_coding;

  uint8_t* rx_dst;
  uint8_t* tx_src;
  uint32_t rx_len;
  int32_t  tx_len;

  // Endpoint Transfer buffer
  alignas(4) uint8_t epout_buf[BULK_PACKET_SIZE];
  alignas(4) uint8_t epin_buf [BULK_PACKET_SIZE];

} acm_interface_t;

#define ITF_MEM_RESET_SIZE offsetof(acm_interface_t, line_coding)

//--------------------------------------------------------------------+
// INTERNAL OBJECT & FUNCTION DECLARATION
//--------------------------------------------------------------------+
CFG_TUD_MEM_SECTION tu_static acm_interface_t acm_itf;

//--------------------------------------------------------------------+
// APPLICATION API
//--------------------------------------------------------------------+
bool acm_opened(void)
{
	// consider active DTR (bit 0) as connected
	return tud_ready() && tu_bit_test(acm_itf.line_state, 0);
}

uint32_t acm_read(uint8_t * buffer, uint32_t count)
{
	if (acm_itf.rx_dst == NULL && count > 0) {
		acm_itf.rx_dst = buffer;
		acm_itf.rx_len = count;
		return 0;
	}
	if (acm_itf.rx_dst+acm_itf.rx_len == buffer+count) { // mutex
		uint32_t bytes = count - acm_itf.rx_len;
		if (bytes > 0) acm_itf.rx_dst = NULL;
		if (bytes < count) buffer[bytes] = '\0';
		return bytes;
	}
	return 0; // ssize_t -1 EBUSY
}

uint32_t acm_write(uint8_t * buffer, uint32_t count)
{
	if (acm_itf.tx_src == NULL && count > 0 && acm_opened() && usbd_edpt_claim(0, acm_itf.ep_in)) {
		acm_itf.tx_src = buffer;
		acm_itf.tx_len = count;
		if ((intptr_t) buffer % 4 == 0 && (intptr_t) buffer >= 0x20000000)
			// shortpath if buffer is aligned and in memory
			usbd_edpt_xfer(0, acm_itf.ep_in, buffer, count);
		else {
			uint32_t bytes = TU_MIN(acm_itf.tx_len, BULK_PACKET_SIZE);
			memcpy(acm_itf.epin_buf, acm_itf.tx_src, bytes);
			usbd_edpt_xfer(0, acm_itf.ep_in, acm_itf.epin_buf, bytes);
		}
		return 0;
	}
	if (acm_itf.tx_src + acm_itf.tx_len == buffer + count) {
		if (acm_opened() && acm_itf.tx_len > 0) return count - acm_itf.tx_len;
		else {
			acm_itf.tx_src = NULL;
			return count;
		}
	}
	return acm_opened() ? 0 : count;
}

//--------------------------------------------------------------------+
// USBD Driver API
//--------------------------------------------------------------------+
void acm_init(void)
{
	tu_memclr(&acm_itf, sizeof(acm_itf));

	// default line coding is : stop bit = 1, parity = none, data bits = 8
	acm_itf.line_coding.bit_rate  = 115200;
	acm_itf.line_coding.stop_bits = 0;
	acm_itf.line_coding.parity    = 0;
	acm_itf.line_coding.data_bits = 8;
}

bool acm_deinit(void)
{
	return true;
}

void acm_reset(uint8_t rhport)
{
	(void) rhport;
	tu_memclr(&acm_itf, ITF_MEM_RESET_SIZE);
}

uint16_t acm_open(uint8_t rhport, tusb_desc_interface_t const * itf_desc, uint16_t max_len)
{
  // Only support ACM subclass
  TU_VERIFY( TUSB_CLASS_CDC                           == itf_desc->bInterfaceClass &&
             CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL == itf_desc->bInterfaceSubClass, 0);

  //------------- Control Interface -------------//
  acm_itf.itf_num = itf_desc->bInterfaceNumber;

  uint16_t drv_len = sizeof(tusb_desc_interface_t);
  uint8_t const * p_desc = tu_desc_next( itf_desc );

  // Communication Functional Descriptors
  while ( TUSB_DESC_CS_INTERFACE == tu_desc_type(p_desc) && drv_len <= max_len )
  {
    drv_len += tu_desc_len(p_desc);
    p_desc   = tu_desc_next(p_desc);
  }

  if ( TUSB_DESC_ENDPOINT == tu_desc_type(p_desc) )
  {
    // notification endpoint
    tusb_desc_endpoint_t const * desc_ep = (tusb_desc_endpoint_t const *) p_desc;

    TU_ASSERT( usbd_edpt_open(rhport, desc_ep), 0 );
    acm_itf.ep_notif = desc_ep->bEndpointAddress;

    drv_len += tu_desc_len(p_desc);
    p_desc   = tu_desc_next(p_desc);
  }

  //------------- Data Interface (if any) -------------//
  if ( (TUSB_DESC_INTERFACE == tu_desc_type(p_desc)) &&
       (TUSB_CLASS_CDC_DATA == ((tusb_desc_interface_t const *) p_desc)->bInterfaceClass) )
  {
    // next two endpoint descriptors
    drv_len += tu_desc_len(p_desc);
    p_desc   = tu_desc_next(p_desc);

    // Open endpoint pair
    TU_ASSERT( usbd_open_edpt_pair(rhport, p_desc, 2, TUSB_XFER_BULK, &acm_itf.ep_out, &acm_itf.ep_in), 0 );

    drv_len += 2*sizeof(tusb_desc_endpoint_t);
  }

  // start listening
  usbd_edpt_claim(rhport, acm_itf.ep_out);
  usbd_edpt_xfer (rhport, acm_itf.ep_out, acm_itf.epout_buf, BULK_PACKET_SIZE);

  return drv_len;
}

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool acm_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
  // Handle class request only
  TU_VERIFY(request->bmRequestType_bit.type == TUSB_REQ_TYPE_CLASS);

  switch ( request->bRequest )
  {
    case CDC_REQUEST_SET_LINE_CODING:
      if (stage == CONTROL_STAGE_SETUP)
      {
        TU_LOG_DRV("  Set Line Coding\r\n");
        tud_control_xfer(rhport, request, &acm_itf.line_coding, sizeof(cdc_line_coding_t));
      }
      else if ( stage == CONTROL_STAGE_ACK)
      {
        if ( acm_line_coding_cb ) acm_line_coding_cb(&acm_itf.line_coding);
      }
    break;

    case CDC_REQUEST_GET_LINE_CODING:
      if (stage == CONTROL_STAGE_SETUP)
      {
        TU_LOG_DRV("  Get Line Coding\r\n");
        tud_control_xfer(rhport, request, &acm_itf.line_coding, sizeof(cdc_line_coding_t));
      }
    break;

    case CDC_REQUEST_SET_CONTROL_LINE_STATE:
      if (stage == CONTROL_STAGE_SETUP)
      {
        tud_control_status(rhport, request);
      }
      else if (stage == CONTROL_STAGE_ACK)
      {
        // CDC PSTN v1.2 section 6.3.12
        // Bit 0: Indicates if DTE is present or not.
        //        This signal corresponds to V.24 signal 108/2 and RS-232 signal DTR (Data Terminal Ready)
        // Bit 1: Carrier control for half-duplex modems.
        //        This signal corresponds to V.24 signal 105 and RS-232 signal RTS (Request to Send)
		bool dtr = tu_bit_test(request->wValue, 0);
		bool rts = tu_bit_test(request->wValue, 1);

		TU_LOG_DRV("  Set Control Line State: DTR = %d, RTS = %d\r\n", dtr, rts);

		// invoke callback and release transmission lock
		if ( tud_ready() && request->wValue != acm_itf.line_state ) {
			if (dtr == 0) {
				acm_itf.tx_src += acm_itf.tx_len;
				acm_itf.tx_len = 0;
				usbd_edpt_release(rhport, acm_itf.ep_in);
			}
			if (acm_line_state_cb) acm_line_state_cb(dtr, rts);
		}

        acm_itf.line_state = (uint8_t) request->wValue;
      }
    break;
    case CDC_REQUEST_SEND_BREAK:
      if (stage == CONTROL_STAGE_SETUP)
      {
        tud_control_status(rhport, request);
      }
      else if (stage == CONTROL_STAGE_ACK)
      {
        TU_LOG_DRV("  Send Break\r\n");
        if ( acm_send_break_cb ) acm_send_break_cb(request->wValue);
      }
    break;

    default: return false; // stall unsupported request
  }

  return true;
}

bool acm_xfer_cb(uint8_t rhport, uint8_t ep_addr, xfer_result_t result, uint32_t xferred_bytes)
{
	(void) result;

	// device input coming from host output
	if (ep_addr == acm_itf.ep_out) {
		if (acm_itf.rx_dst != NULL) {
			uint32_t bytes = TU_MIN(acm_itf.rx_len, xferred_bytes);
			memcpy(acm_itf.rx_dst, acm_itf.epout_buf, bytes);
			acm_itf.rx_dst += bytes;
			acm_itf.rx_len -= bytes;
		}
  		usbd_edpt_claim(rhport, acm_itf.ep_out);
  		usbd_edpt_xfer (rhport, acm_itf.ep_out, acm_itf.epout_buf, BULK_PACKET_SIZE);
	}

	// device output going to host input
	if (ep_addr == acm_itf.ep_in && acm_itf.tx_src != NULL) {
		acm_itf.tx_src += xferred_bytes;
		acm_itf.tx_len -= xferred_bytes;
		if (acm_itf.tx_len > 0) {
			uint32_t bytes  = TU_MIN(acm_itf.tx_len, BULK_PACKET_SIZE);
			memcpy(acm_itf.epin_buf, acm_itf.tx_src, bytes);
			usbd_edpt_claim(rhport, acm_itf.ep_in);
			usbd_edpt_xfer (rhport, acm_itf.ep_in, acm_itf.epin_buf, bytes);
		} else if (xferred_bytes == BULK_PACKET_SIZE) {
			usbd_edpt_claim(rhport, acm_itf.ep_in);
			usbd_edpt_xfer (rhport, acm_itf.ep_in, NULL, 0); // send ZLP
		}
	}

	// nothing to do with notif endpoint for now

	return true;
}

#include "samd21g18a/fiber.h"

_ssize_t _write_r (struct _reent* impure, int fd, const void* buf, size_t count) {
	(void) impure;
	(void) fd;
	while (acm_write((uint8_t*) buf, count) < count) fiber_yield();
	return count;
}

_ssize_t _read_r (struct _reent* impure, int fd, void* buf, size_t count) {
	(void) impure;
	(void) fd;
	int n = 0;
	if (count > 0) while ((n = acm_read(buf, count)) < 1) fiber_yield();
	return n;
}
