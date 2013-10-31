// balmer@inbox.ru 2013 ESR Demo
#include "sound.h"
#include "voltage.h"
#include "usb_desc.h"

//cyclic buffer 
uint16_t cyclic_capacity;
uint16_t cyclic_size;
uint16_t cyclic_pos;
uint16_t* cyclic_buffer;

bool sound_command = false;
uint16_t sound_sum_samples = 1;
uint32_t sound_sum_value;
uint16_t sound_sum_count;


void CyclicInit(void)
{
	cyclic_capacity = ADC_BUFFER_SIZE;
	cyclic_size = 0;
	cyclic_buffer = adc_buffer;
	cyclic_pos = 0;
}

bool CyclicIsEmpty(void)
{
	return cyclic_size==0;
}

bool CyclicIsFull(void)
{
	return cyclic_size==cyclic_capacity;
}

//add to head
void CyclicAdd(uint16_t data)
{
	assert_param(cyclic_size<cyclic_capacity);
    
	cyclic_buffer[cyclic_pos] = data;
	cyclic_pos++;
	cyclic_size++;
	if(cyclic_pos==cyclic_capacity)
		cyclic_pos = 0;
}

//remove from tail
uint16_t CyclicPop(void)
{
	assert_param(cyclic_size>0);
	uint16_t end_pos = cyclic_pos+(cyclic_capacity-cyclic_size);
	if(end_pos>=cyclic_capacity)
		end_pos -= cyclic_capacity;
    
	cyclic_size--;
	return cyclic_buffer[end_pos];
}

void SoundADC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);//48/8 = 6 MHz

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	// Configure PA.0 (ADC Channel10) as analog input -------------------------
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);   

	// ADC1 configuration ------------------------------------------------------
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	// ADC1 regular channel10 configuration
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_Cmd(ADC1, ENABLE);

	// Enable ADC1 reset calibaration register
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register 
	while(ADC_GetResetCalibrationStatus(ADC1)); // Start ADC1 calibaration
	ADC_StartCalibration(ADC1); // Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC1));

  /* Configure and enable ADC interrupt */
#if defined (STM32F10X_LD_VL) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD_VL)
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;
#else
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
#endif
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC1_2_IRQHandler(void)
{
	uint16_t value = ADC_GetConversionValue(ADC1);
	sound_sum_value += value;
	sound_sum_count++;
	if(sound_sum_count<sound_sum_samples)
		return;
	value = (sound_sum_value+(sound_sum_count>>1))/sound_sum_count;
	sound_sum_value = 0;
	sound_sum_count = 0;

	if(CyclicIsFull())
		return;
	CyclicAdd(value);
}


void SoundInit(uint16_t sum_samples)
{
	sound_sum_samples = sum_samples;
	sound_sum_count = 0;
	sound_sum_value = 0;
	CyclicInit();
	SoundADC_Configuration();
}

const uint16_t usbMaxSamples = VIRTUAL_COM_PORT_DATA_SIZE;
void SoundFillUsb()
{
	uint16_t i;
	for(i=0; i<usbMaxSamples; i++)
	{
		if(CyclicIsEmpty())
			break;
		//USBAdd16(CyclicPop());
		USBAdd8(CyclicPop()>>4);
	}
}
