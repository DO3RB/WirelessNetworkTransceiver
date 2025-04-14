#include "tinyusb/device/usbd.h"
#include "tinyusb/device/usbd_pvt.h"

#include "tinyusb_acm.h"
#include "tinyusb_ecm.h"

//--------------------------------------------------------------------+
// Device Driver Override
//--------------------------------------------------------------------+

#if CFG_TUSB_DEBUG >= CFG_TUD_LOG_LEVEL
  #define DRIVER_NAME(_name)  _name
#else
  #define DRIVER_NAME(_name)  NULL
#endif

usbd_class_driver_t const tinyusb_drivers[] =
{
  {
	.name             = DRIVER_NAME("ACM"),
	.init             = acm_init,
	.deinit           = acm_deinit,
	.reset            = acm_reset,
	.open             = acm_open,
	.control_xfer_cb  = acm_control_xfer_cb,
	.xfer_cb          = acm_xfer_cb,
	.sof              = NULL
  },
  {
	.name             = DRIVER_NAME("ECM"),
	.init             = ecm_init,
	.deinit           = ecm_deinit,
	.reset            = ecm_reset,
	.open             = ecm_open,
	.control_xfer_cb  = ecm_control_xfer_cb,
	.xfer_cb          = ecm_xfer_cb,
	.sof              = NULL
  }
};

usbd_class_driver_t const * usbd_app_driver_get_cb(uint8_t* driver_count) {
    *driver_count = TU_ARRAY_SIZE(tinyusb_drivers);
    return tinyusb_drivers;
}

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

enum // String Descriptor Index
{
  STRID_LANGID = 0,
  STRID_CREATOR,
  STRID_PRODUCT,
  STRID_DESC_ACM,
  STRID_DESC_ECM,
  STRID_SERIAL,
  STRID_MAC,
  STRID_TOTAL
};

enum
{
  ITF_NUM_ACM = 0,
  ITF_NUM_ACM_DATA,
  ITF_NUM_ECM,
  ITF_NUM_ECM_DATA,
  ITF_NUM_TOTAL
};

tusb_desc_device_t const desc_device =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    // Use Interface Association Descriptor (IAD) device class
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0x0403, // FTDI
    .idProduct          = 0x7AE7, // 7AE0 - 7AE7
    .bcdDevice          = 0x0101,

    .iManufacturer      = STRID_CREATOR,
    .iProduct           = STRID_PRODUCT,
    .iSerialNumber      = STRID_SERIAL,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  return (uint8_t const *) &desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_CDC_ECM_DESC_LEN)

#define EPNUM_CDC_NOTIF   0x81
#define EPNUM_CDC_OUT     0x02
#define EPNUM_CDC_IN      0x82

#define EPNUM_ECM_NOTIF   0x83
#define EPNUM_ECM_OUT     0x04
#define EPNUM_ECM_IN      0x84

uint8_t const desc_configuration[] =
{
	TUD_CONFIG_DESCRIPTOR (
		1,					// config number
		ITF_NUM_TOTAL,		// interface count
		STRID_PRODUCT,		// string index
		CONFIG_TOTAL_LEN,	// total bytes
		0x00,				// attribute
		200					// current in mA
	),
	TUD_CDC_DESCRIPTOR (	// 1st CDC ACM
		ITF_NUM_ACM,		// interface number
		STRID_DESC_ACM,		// string index
		EPNUM_CDC_NOTIF,	// notification EP address
		8,					// notification EP size
		EPNUM_CDC_OUT,		// address EP data out
		EPNUM_CDC_IN,		// address EP data in
		64					//    size EP data
	),
	TUD_CDC_ECM_DESC_STATS (// 2nd CDC ECM
		ITF_NUM_ECM,		// interface number
		STRID_DESC_ECM,		// string index
		STRID_MAC,			// MAC address string index
		0x00F0,				// bmEthernetStatistics
		EPNUM_ECM_NOTIF,	// notification EP address
		64,					// notification EP size
		EPNUM_ECM_OUT,		// address EP data out
		EPNUM_ECM_IN,		// address EP data in
		CFG_TUD_NET_SIZE,	//    size EP data
		CFG_TUD_NET_MTU		// max segment size
	)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
	(void) index;
	return desc_configuration;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

static char const * string_desc_arr [] =
{
  [STRID_LANGID]	= (const char[]) { 0x09, 0x04 }, // English (0x0409)
  [STRID_CREATOR]	= "elektronenpumpe.de",
  [STRID_PRODUCT]	= "RFM12BP",
  [STRID_DESC_ACM]	= "Serial",
  [STRID_DESC_ECM]	= "Network"
};

#include "storage.h"

static uint16_t _desc_str[33];

uint16_t const * tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;
  uint_fast8_t count = 0;
  switch (index) {
    case STRID_LANGID:
      memcpy(&_desc_str[1], string_desc_arr[STRID_LANGID], 2);
      count = 1;
      break;
    case STRID_CREATOR:
    case STRID_PRODUCT:
    case STRID_DESC_ACM:
    case STRID_DESC_ECM: {
      char const * const str = string_desc_arr[index];
      // Cap at max char
      if ((count = strlen(str)) > 31) count = 31;
      // Convert ASCII string into UTF-16
      for (uint_fast8_t n = 0; n < count; n++) { _desc_str[1+n] = str[n]; }
      break; }
    case STRID_SERIAL:
	  storage_serial_utf16(&_desc_str[1]);
      count = 32;
      break;
    case STRID_MAC:
	  storage_ethmac_utf16(&_desc_str[1]);
      count = 12;
      break;
    default:
      return NULL;
  }
  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*count + 2);
  return _desc_str;
}
