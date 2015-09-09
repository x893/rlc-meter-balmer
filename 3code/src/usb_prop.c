// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "hw_config.h"

uint32_t ProtocolValue;

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
{
	EP_NUM,
	1
};

DEVICE_PROP Device_Property =
{
	RLC_Init,
	RLC_Reset,
	RLC_Status_In,
	RLC_Status_Out,
	RLC_Data_Setup,
	RLC_NoData_Setup,
	RLC_Get_Interface_Setting,
	RLC_GetDeviceDescriptor,
	RLC_GetConfigDescriptor,
	RLC_GetStringDescriptor,
	0,
	0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
	RLC_GetConfiguration,
	RLC_SetConfiguration,
	RLC_GetInterface,
	RLC_SetInterface,
	RLC_GetStatus,
	RLC_ClearFeature,
	RLC_SetEndPointFeature,
	RLC_SetDeviceFeature,
	RLC_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
	(uint8_t*)RLC_DeviceDescriptor,
	RLC_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
	(uint8_t*)RLC_ConfigDescriptor,
	RLC_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR RLC_Report_Descriptor =
{
	(uint8_t *)RLC_ReportDescriptor,
	RLC_SIZ_REPORT_DESC
};

ONE_DESCRIPTOR Mouse_Hid_Descriptor =
{
	(uint8_t*)RLC_ConfigDescriptor + RLC_OFF_HID_DESC,
	RLC_SIZ_HID_DESC
};

ONE_DESCRIPTOR String_Descriptor[4] =
{
	{ (uint8_t*)RLC_StringLangID, RLC_SIZ_STRING_LANGID },
	{ (uint8_t*)RLC_StringVendor, RLC_SIZ_STRING_VENDOR },
	{ (uint8_t*)RLC_StringProduct, RLC_SIZ_STRING_PRODUCT },
	{ (uint8_t*)RLC_StringSerial, RLC_SIZ_STRING_SERIAL }
};

/**
  * @brief  RLC_Init
  * @param  None
  * @retval None
  */
void RLC_Init(void)
{
	Get_SerialNum();	/* Update the serial number string descriptor with the data from the unique	ID*/
	pInformation->Current_Configuration = 0;
	
	PowerOn();			/* Connect the device */
	USB_SIL_Init();		/* Perform basic device initialization operations */

	bDeviceState = UNCONNECTED;
}

/**
  * @brief  Joystick Mouse reset routine.
  * @param  None
  * @retval None
  */
void RLC_Reset(void)
{
	/* Set RLC_DEVICE as not configured */
	pInformation->Current_Configuration = 0;
	pInformation->Current_Interface = 0;	/*the default Interface*/

	/* Current Feature initialization */
	pInformation->Current_Feature = RLC_ConfigDescriptor[7];

	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_BULK);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPRxAddr(ENDP1, ENDP1_RXADDR);
	SetEPRxCount(ENDP1, VIRTUAL_COM_PORT_DATA_SIZE);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	SetEPRxStatus(ENDP1, EP_RX_VALID);

	/* Set this device to response on default address */
	SetDeviceAddress(0);

	bDeviceState = ATTACHED;
}

/**
  * @brief  Update the device state to configured.
  * @param  None
  * @retval None
  */
void RLC_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &Device_Info;

	if (pInfo->Current_Configuration != 0)
	{	/* Device configured */
		bDeviceState = CONFIGURED;
	}
}

/**
  * @brief  Update the device state to addressed.
  * @param  None
  * @retval None
  */
void RLC_SetDeviceAddress(void)
{
	bDeviceState = ADDRESSED;
}

/**
  * @brief  Joystick status IN routine.
  * @param  None
  * @retval None
  */
void RLC_Status_In(void)
{}

/**
  * @brief  Joystick status OUT routine.
  * @param  None
  * @retval None
  */
void RLC_Status_Out(void)
{}

/**
  * @brief  Handle the data class specific requests.
  * @param  RequestNo: Request Nb.
  * @retval USB_UNSUPPORT or USB_SUCCESS.
  */
RESULT RLC_Data_Setup(uint8_t RequestNo)
{
	uint8_t *(*CopyRoutine)(uint16_t);
	CopyRoutine = NULL;

/*
	if ((RequestNo == GET_DESCRIPTOR)
	&& (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT))
	&& (pInformation->USBwIndex0 == 0))
	{
		if (pInformation->USBwValue1 == REPORT_DESCRIPTOR)
		{
			CopyRoutine = RLC_GetReportDescriptor;
		}
		else if (pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
		{
			CopyRoutine = RLC_GetHIDDescriptor;
		}
	}
	else if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
	&& RequestNo == GET_PROTOCOL)
	{
		CopyRoutine = RLC_GetProtocolValue;
	}
*/

	if (CopyRoutine == NULL)
		return USB_UNSUPPORT;

	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}

/**
  * @brief  Handle the no data class specific requests
  * @param  RequestNo: Request Nb.
  * @retval USB_UNSUPPORT or USB_SUCCESS.
  */
RESULT RLC_NoData_Setup(uint8_t RequestNo)
{
	if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
	&& (RequestNo == SET_PROTOCOL)
		)
		return RLC_SetProtocol();
	return USB_UNSUPPORT;
}

/**
  * @brief  Gets the device descriptor.
  * @param  Length: Length.
  * @retval The address of the device descriptor.
  */
uint8_t *RLC_GetDeviceDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/**
  * @brief  Gets the configuration descriptor.
  * @param  Length: Length.
  * @retval The address of the configuration descriptor.
  */
uint8_t *RLC_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/**
  * @brief  Gets the string descriptors according to the needed index
  * @param  Length: Length.
  * @retval The address of the string descriptors.
  */
uint8_t *RLC_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	if (wValue0 > 4)
		return NULL;
	return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
}

/**
  * @brief  Gets the HID report descriptor.
  * @param  Length: Length.
  * @retval The address of the configuration descriptor.
  */
uint8_t *RLC_GetReportDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &RLC_Report_Descriptor);
}

/**
  * @brief  Gets the HID descriptor.
  * @param  Length: Length.
  * @retval The address of the configuration descriptor.
  */
uint8_t *RLC_GetHIDDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Mouse_Hid_Descriptor);
}

/**
  * @brief  tests the interface and the alternate setting according to the supported one.
  * @param  Interface: interface number.
  * @param  AlternateSetting : Alternate Setting number.
  * @retval USB_SUCCESS or USB_UNSUPPORT.
  */
RESULT RLC_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if (AlternateSetting > 0)
		return USB_UNSUPPORT;
	else if (Interface > 0)
		return USB_UNSUPPORT;

	return USB_SUCCESS;
}

/**
  * @brief  Joystick Set Protocol request routine.
  * @param  None.
  * @retval USB_SUCCESS
  */
RESULT RLC_SetProtocol(void)
{
	ProtocolValue = pInformation->USBwValue0;
	return USB_SUCCESS;
}

/**
  * @brief  Get the protocol value
  * @param  Length: length
  * @retval address of the protocol value.
  */
uint8_t *RLC_GetProtocolValue(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = 1;
		return NULL;
	}
	return (uint8_t *)(&ProtocolValue);
}
