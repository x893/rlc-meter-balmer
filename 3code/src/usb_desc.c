// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "usb_lib.h"
#include "usb_desc.h"

/* USB Standard Device Descriptor */
const uint8_t RLC_DeviceDescriptor[RLC_SIZ_DEVICE_DESC] =
{
	0x12,                       /*bLength */
	USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
	0x00,                       /*bcdUSB */
	0x02,
	0x00,                       /*bDeviceClass*/
	0x00,                       /*bDeviceSubClass*/
	0x00,                       /*bDeviceProtocol*/
	0x40,                       /*bMaxPacketSize 64*/
	0xC0,
	0x16,						/* idVendor = 0x16C0 */
	0xDC,
	0x05,						/* idProduct = 0x05DC */
	0x00,                       /*bcdDevice rel. 2.00*/
	0x02,
	1,                          /*Index of string descriptor describing manufacturer */
	2,							/*Index of string descriptor describing product*/
	3,                          /*Index of string descriptor describing the device serial number */
	0x01                        /*bNumConfigurations*/
};


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t RLC_ConfigDescriptor[RLC_SIZ_CONFIG_DESC] =
{
	0x09,		/* bLength: Configuration Descriptor size */
	USB_CONFIGURATION_DESCRIPTOR_TYPE,	/* bDescriptorType: Configuration */
	RLC_SIZ_CONFIG_DESC,				/* wTotalLength: Bytes returned */
	0x00,
	0x01,		/*bNumInterfaces: 1 interface*/
	0x01,		/*bConfigurationValue: Configuration value*/
	0x00,		/*iConfiguration: Index of string descriptor describing the configuration*/
	0xE0,		/*bmAttributes: bus powered */
	0x32,		/*MaxPower 100 mA: this current is used for detecting Vbus*/

	/************** Descriptor of Joystick Mouse interface ****************/
	// 09

	0x09,		//bLength: Interface Descriptor size
	USB_INTERFACE_DESCRIPTOR_TYPE,//bDescriptorType: Interface descriptor type
	0x00,		//bInterfaceNumber: Number of Interface
	0x00,		//bAlternateSetting: Alternate setting
	0x02,		//bNumEndpoints
	0xFF,		//Vendor specific //0x00,//unknown interface // 0x03,         //bInterfaceClass: HID
	0x00,		//not used if 0xFF        //bInterfaceSubClass : 1=BOOT, 0=no boot
	0x00,		//not used if 0xFF        //nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse
	0,            //iInterface: Index of string descriptor
	/******************** Descriptor of Joystick Mouse HID ********************/
	/* 18 */
	/*
	0x09,		//bLength: HID Descriptor size
	HID_DESCRIPTOR_TYPE, //bDescriptorType: HID
	0x00,		//bcdHID: HID Class Spec release number
	0x01,
	0x00,		//bCountryCode: Hardware target country
	0x01,		//bNumDescriptors: Number of HID class descriptors to follow
	0x22,		//bDescriptorType
	RLC_SIZ_REPORT_DESC,//wItemLength: Total length of Report descriptor
	0x00,
	*/
	/* 27 */
	// Endpoint 1 IN descriptor
	0x07,		/*bLength: Endpoint Descriptor size*/
	USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/
	0x81,		/*bEndpointAddress: Endpoint Address (IN)*/
	0x02,		/* bmAttributes: Bulk */
	VIRTUAL_COM_PORT_DATA_SIZE,	/* wMaxPacketSize: */
	0x00,
	0x00,		/* bInterval */
	/* 34 */
	// Endpoint 1 OUT descriptor
	0x07,		/*bLength: Endpoint Descriptor size*/
	USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

	0x01,		/*bEndpointAddress: Endpoint Address (OUT)*/
	0x02,		/* bmAttributes: Bulk */
	VIRTUAL_COM_PORT_DATA_SIZE,	/* wMaxPacketSize: */
	0x00,
	0x00,		/* bInterval */
	/* 41 */
};

const uint8_t RLC_ReportDescriptor[RLC_SIZ_REPORT_DESC] =
{
	0x05,          /*Usage Page(Generic Desktop)*/
	0x01,
	0x09,          /*Usage(Mouse)*/
	0x02,
	0xA1,          /*Collection(Logical)*/
	0x01,
	0x09,          /*Usage(Pointer)*/
	0x01,
	/* 8 */
	0xA1,          /*Collection(Linked)*/
	0x00,
	0x05,          /*Usage Page(Buttons)*/
	0x09,
	0x19,          /*Usage Minimum(1)*/
	0x01,
	0x29,          /*Usage Maximum(3)*/
	0x03,
	/* 16 */
	0x15,          /*Logical Minimum(0)*/
	0x00,
	0x25,          /*Logical Maximum(1)*/
	0x01,
	0x95,          /*Report Count(3)*/
	0x03,
	0x75,          /*Report Size(1)*/
	0x01,
	/* 24 */
	0x81,          /*Input(Variable)*/
	0x02,
	0x95,          /*Report Count(1)*/
	0x01,
	0x75,          /*Report Size(5)*/
	0x05,
	0x81,          /*Input(Constant,Array)*/
	0x01,
	/* 32 */
	0x05,          /*Usage Page(Generic Desktop)*/
	0x01,
	0x09,          /*Usage(X axis)*/
	0x30,
	0x09,          /*Usage(Y axis)*/
	0x31,
	0x09,          /*Usage(Wheel)*/
	0x38,
	/* 40 */
	0x15,          /*Logical Minimum(-127)*/
	0x81,
	0x25,          /*Logical Maximum(127)*/
	0x7F,
	0x75,          /*Report Size(8)*/
	0x08,
	0x95,          /*Report Count(3)*/
	0x03,
	/* 48 */
	0x81,          /*Input(Variable, Relative)*/
	0x06,
	0xC0,          /*End Collection*/
	0x09,
	0x3c,
	0x05,
	0xff,
	0x09,
	/* 56 */
	0x01,
	0x15,
	0x00,
	0x25,
	0x01,
	0x75,
	0x01,
	0x95,
	/* 64 */
	0x02,
	0xb1,
	0x22,
	0x75,
	0x06,
	0x95,
	0x01,
	0xb1,
	/* 72 */
	0x01,
	0xc0
}; /* RLC_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t RLC_StringLangID[RLC_SIZ_STRING_LANGID] =
{
	RLC_SIZ_STRING_LANGID,
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
	0x04
}; /* LangID = 0x0409: U.S. English */

const uint8_t RLC_StringVendor[RLC_SIZ_STRING_VENDOR] =
{
	RLC_SIZ_STRING_VENDOR,		/* Size of Vendor string */
	USB_STRING_DESCRIPTOR_TYPE,	/* bDescriptorType*/
								/* Manufacturer: "STMicroelectronics" */
	'B', 0, 'A', 0, 'L', 0, 'M', 0, 'E', 0, 'R', 0,	'@', 0, 'I', 0,	'N', 0, 'B', 0, 'O', 0, 'X', 0, '.', 0, 'R', 0, 'U', 0, ' ', 0, 'H', 0, 'M', 0
};

const uint8_t RLC_StringProduct[RLC_SIZ_STRING_PRODUCT] =
{
	RLC_SIZ_STRING_PRODUCT,		/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,	/* bDescriptorType */
	'R', 0, 'L', 0, 'C', 0, ' ', 0, 'B', 0, 'a', 0, 'l', 0, 'm', 0, 'e', 0, 'r', 0, ' ', 0, '3', 0, '0', 0, '3', 0
};
uint8_t RLC_StringSerial[RLC_SIZ_STRING_SERIAL] =
{
	RLC_SIZ_STRING_SERIAL,		/* bLength */
	USB_STRING_DESCRIPTOR_TYPE,	/* bDescriptorType */
	'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, '1', 0, '0', 0
};
