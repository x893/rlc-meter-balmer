// balmer@inbox.ru RLC Meter 303
// 2013-2014

#include "stm32f30x.h"
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_pwr.h"
#include "hw_config.h"

__IO uint32_t bDeviceState = UNCONNECTED;	/* USB device status */
__IO bool fSuspendEnabled = true;			/* true when suspend is possible */

struct
{
	__IO RESUME_STATE eState;
	__IO uint8_t bESOFcnt;
} ResumeS;

/**
  * @brief  PowerOn
  * @param  None.
  * @retval USB_SUCCESS.
  */
RESULT PowerOn(void)
{
	uint16_t wRegVal;

	USB_Cable_Config(ENABLE);	/*** cable plugged-in ? ***/
	wRegVal = CNTR_FRES;		/*** CNTR_PWDN = 0 ***/
	_SetCNTR(wRegVal);

	wInterrupt_Mask = 0;		/*** CNTR_FRES = 0 ***/
	_SetCNTR(wInterrupt_Mask);
	
	_SetISTR(0);				/*** Clear pending interrupts ***/
								/*** Set interrupt mask ***/
	wInterrupt_Mask = CNTR_RESETM | CNTR_SUSPM | CNTR_WKUPM;
	_SetCNTR(wInterrupt_Mask);

	return USB_SUCCESS;
}

/**
  * @brief  handles switch-off conditions
  * @param  None.
  * @retval USB_SUCCESS.
  */
RESULT PowerOff()
{
	_SetCNTR(CNTR_FRES);			/* disable all interrupts and force USB reset */
	_SetISTR(0);					/* clear interrupt status register */
	USB_Cable_Config(DISABLE);		/* Disable the Pull-Up*/
	_SetCNTR(CNTR_FRES + CNTR_PDWN);/* switch-off device */

	return USB_SUCCESS;
}

/**
  * @brief  Sets suspend mode operating conditions
  * @param  None.
  * @retval USB_SUCCESS.
  */
void Suspend(void)
{
	uint16_t wCNTR;
	/* suspend preparation */
	/* ... */

	/* macrocell enters suspend mode */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_FSUSP;
	_SetCNTR(wCNTR);

	/* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
	/* power reduction */
	/* ... on connected devices */


	/* force low-power mode in the macrocell */
	wCNTR = _GetCNTR();
	wCNTR |= CNTR_LPMODE;
	_SetCNTR(wCNTR);

	/* switch-off the clocks */
	/* ... */
	Enter_LowPowerMode();

}

/**
  * @brief  Handles wake-up restoring normal operations
  * @param  None.
  * @retval USB_SUCCESS.
  */
void Resume_Init(void)
{
	uint16_t wCNTR;

	/* ------------------ ONLY WITH BUS-POWERED DEVICES ---------------------- */
	/* restart the clocks */
	/* ...  */

	/* CNTR_LPMODE = 0 */
	wCNTR = _GetCNTR();
	wCNTR &= (~CNTR_LPMODE);
	_SetCNTR(wCNTR);


	/* restore full power */
	/* ... on connected devices */
	Leave_LowPowerMode();

	/* reset FSUSP bit */
	_SetCNTR(IMR_MSK);

	/* reverse suspend preparation */
	/* ... */

}

/**
  * @brief  This is the state machine handling resume operations and timing sequence.
  *         The control is based on the Resume structure variables and on the ESOF
  *         interrupt calling this subroutine without changing machine state.
  * @param  eResumeSetVal: a state machine value (RESUME_STATE) RESUME_ESOF doesn't
  *                        change ResumeS.eState allowing decrementing of the ESOF
  *                        counter in different states.
  * @retval None.
  */
void Resume(RESUME_STATE eResumeSetVal)
{
	uint16_t wCNTR;

	if (eResumeSetVal != RESUME_ESOF)
		ResumeS.eState = eResumeSetVal;

	switch (ResumeS.eState)
	{
	case RESUME_EXTERNAL:
		Resume_Init();
		ResumeS.eState = RESUME_OFF;
		break;
	case RESUME_INTERNAL:
		Resume_Init();
		ResumeS.eState = RESUME_START;
		break;
	case RESUME_LATER:
		ResumeS.bESOFcnt = 2;
		ResumeS.eState = RESUME_WAIT;
		break;
	case RESUME_WAIT:
		ResumeS.bESOFcnt--;
		if (ResumeS.bESOFcnt == 0)
			ResumeS.eState = RESUME_START;
		break;
	case RESUME_START:

		wCNTR = _GetCNTR();
		wCNTR |= CNTR_RESUME;
		_SetCNTR(wCNTR);
		ResumeS.eState = RESUME_ON;
		ResumeS.bESOFcnt = 10;
		break;
	case RESUME_ON:
		ResumeS.bESOFcnt--;
		if (ResumeS.bESOFcnt == 0)
		{
			wCNTR = _GetCNTR();
			wCNTR &= (~CNTR_RESUME);
			_SetCNTR(wCNTR);
			ResumeS.eState = RESUME_OFF;
		}
		break;
	case RESUME_OFF:
	case RESUME_ESOF:
	default:
		ResumeS.eState = RESUME_OFF;
		break;
	}
}
