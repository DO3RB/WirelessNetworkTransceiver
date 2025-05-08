#include "tinyusb/device/usbd.h"
#include "tinyusb/device/usbd_pvt.h"

#include "samd21g18a/tinyusb_acm.h"
#include "samd21g18a/tinyusb_ecm.h"

//--------------------------------------------------------------------+
// Device Driver Override
//--------------------------------------------------------------------+

usbd_class_driver_t const tinyusb_driver[] = {
	TINYUSB_ACM_DRIVER,
	TINYUSB_ECM_DRIVER
};

usbd_class_driver_t const * usbd_app_driver_get_cb (uint8_t * count)
{
	*count = TU_ARRAY_SIZE(tinyusb_driver);
	return tinyusb_driver;
}

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

enum { // String Descriptor Index
	NUM_STR_LANGUAGE = 0,
	NUM_STR_CREATOR,
	NUM_STR_PRODUCT,
	NUM_STR_SERIAL,
	NUM_STR_DESC_ACM,
	NUM_STR_DESC_ECM,
	NUM_STR_MAC,
	NUM_STR_TOTAL
};

tusb_desc_device_t const tinyusb_desc_device =
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

	.iManufacturer      = NUM_STR_CREATOR,
	.iProduct           = NUM_STR_PRODUCT,
	.iSerialNumber      = NUM_STR_SERIAL,

	.bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb ()
{
	return (uint8_t const *) &tinyusb_desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#define NUM_EPT_ACM_NTF  0x81
#define NUM_EPT_ACM_OUT  0x02
#define NUM_EPT_ACM_IN   0x82

#define NUM_EPT_ECM_NTF  0x83
#define NUM_EPT_ECM_OUT  0x04
#define NUM_EPT_ECM_IN   0x84

#define LEN_CONFIG_TOTAL (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_CDC_ECM_DESC_LEN)

enum {
	NUM_ITF_ACM = 0,
	NUM_ITF_ACM_DATA,
	NUM_ITF_ECM,
	NUM_ITF_ECM_DATA,
	NUM_ITF_TOTAL
};

uint8_t const tinyusb_desc_config[] =
{
	TUD_CONFIG_DESCRIPTOR (
		1,                  // config number
		NUM_ITF_TOTAL,      // interface count
		NUM_STR_PRODUCT,    // string index
		LEN_CONFIG_TOTAL,   // total bytes
		0x00,               // attribute
		200                 // current in mA
	),
	TUD_CDC_DESCRIPTOR (    // 1st CDC ACM
		NUM_ITF_ACM,        // interface number
		NUM_STR_DESC_ACM,   // string index
		NUM_EPT_ACM_NTF,    // notification EP address
		8,                  // notification EP size
		NUM_EPT_ACM_OUT,    // address EP data out
		NUM_EPT_ACM_IN,     // address EP data in
		64                  //    size EP data
	),
	TUD_CDC_ECM_DESC_STATS (// 2nd CDC ECM
		NUM_ITF_ECM,        // interface number
		NUM_STR_DESC_ECM,   // string index
		NUM_STR_MAC,        // MAC address string index
		0x00F0,             // bmEthernetStatistics
		NUM_EPT_ECM_NTF,    // notification EP address
		64,                 // notification EP size
		NUM_EPT_ECM_OUT,    // address EP data out
		NUM_EPT_ECM_IN,     // address EP data in
		CFG_TUD_NET_SIZE,   //    size EP data
		CFG_TUD_NET_MTU     // max segment size
	)
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
	(void) index;
	return tinyusb_desc_config;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

#define USB16(X) _Generic((X)+0, uint16_t *: &( \
	(const struct{uint16_t N; uint16_t S[sizeof(X)/2-1];}) \
	{(TUSB_DESC_STRING<<8)|sizeof(X),(X)}).N)

// USB defines UTF16-LE aka UCS-2 wide string literals

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunterminated-string-initialization"

uint16_t const * tinyusb_desc_string[] =
{
	[NUM_STR_LANGUAGE] = USB16(u"\x0409"), // English
	[NUM_STR_CREATOR]  = USB16(u"elektronenpumpe.de"),
	[NUM_STR_PRODUCT]  = USB16(u"RFM12BP"),
	[NUM_STR_DESC_ACM] = USB16(u"Serial"),
	[NUM_STR_DESC_ECM] = USB16(u"Network"),
	[NUM_STR_SERIAL] = (const uint16_t[33]) {(TUSB_DESC_STRING<<8)|66,0},
	[NUM_STR_MAC]    = (const uint16_t[13]) {(TUSB_DESC_STRING<<8)|26,0},
};
#pragma GCC diagnostic pop

uint16_t const * tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
	(void) langid;
	return (index < NUM_STR_TOTAL) ? tinyusb_desc_string[index] : NULL;
}

#include "storage.h"

__attribute__((constructor)) void tinyusb_flash ()
{
	uint16_t string[32];
	storage_serial_utf16(string);
	storage_flash((void*)&tinyusb_desc_string[NUM_STR_SERIAL][1], string, 64);
	storage_ethmac_utf16(string);
	storage_flash((void*)&tinyusb_desc_string[NUM_STR_MAC][1], string, 24);
	storage_constructor_delist(&tinyusb_flash);
}
