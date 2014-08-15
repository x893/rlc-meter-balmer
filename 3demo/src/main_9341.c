
#define TFT_PORT	GPIOA
#define TFT_SPI		SPI1

#define PIN_LED		GPIO_Pin_1
#define PIN_RST		GPIO_Pin_2
#define PIN_DC		GPIO_Pin_3
#define PIN_CS		GPIO_Pin_0
#define PIN_SCK		GPIO_Pin_5
#define PIN_MISO	GPIO_Pin_6
#define PIN_MOSI	GPIO_Pin_7

#define PIN_SOURCE_SCK	GPIO_PinSource5
#define PIN_SOURCE_MISO	GPIO_PinSource6
#define PIN_SOURCE_MOSI	GPIO_PinSource7

// Colors
#define RED			0xf800
#define GREEN		0x07e0
#define BLUE		0x001f
#define BLACK		0x0000
#define YELLOW		0xffe0
#define WHITE		0xffff
#define CYAN		0x07ff
#define BRIGHT_RED	0xf810
#define GRAY1		0x8410
#define GRAY2		0x4208

#define TFT_CS_LOW		GPIO_WriteBit(TFT_PORT, PIN_CS, Bit_RESET);
#define TFT_CS_HIGH		GPIO_WriteBit(TFT_PORT, PIN_CS, Bit_SET);
#define TFT_DC_LOW		GPIO_WriteBit(TFT_PORT, PIN_DC, Bit_RESET);
#define TFT_DC_HIGH		GPIO_WriteBit(TFT_PORT, PIN_DC, Bit_SET);
#define TFT_RST_LOW		GPIO_WriteBit(TFT_PORT, PIN_RST, Bit_RESET);
#define TFT_RST_HIGH	GPIO_WriteBit(TFT_PORT, PIN_RST, Bit_SET);

void TFT_sendByte(uint8_t data)
{
    while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_TXE)==RESET);
	SPI_SendData8(TFT_SPI, data);
}

void TFT_sendCMD(int index)
{
	while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_BSY) == SET);
    TFT_CS_HIGH;
    TFT_DC_LOW;
    TFT_CS_LOW;

    TFT_sendByte(index);
}

void TFT_sendDATA(int data)
{
	while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_BSY) == SET);
    TFT_DC_HIGH;
    TFT_sendByte(data);
}

void TFT_sendWord(int data)
{
	while(SPI_I2S_GetFlagStatus(TFT_SPI, SPI_I2S_FLAG_BSY) == SET);
    TFT_DC_HIGH;

    TFT_sendByte(data >> 8);
    TFT_sendByte(data & 0x00ff);
}

int TFT_Read_Register(int Addr, int xParameter)
{
	int data = 0;

    TFT_sendCMD(0xD9);                                                     // ext command
    TFT_sendByte(0x10+xParameter);                           // 0x11 is the first Parameter
    TFT_DC_LOW;
    TFT_CS_LOW;
    TFT_sendByte(Addr);
    TFT_DC_HIGH;
    data = SPI_ReceiveData8(TFT_SPI);
    TFT_CS_HIGH;

    return data;
}

int TFT_readID(void)
{
	int i = 0;
	int byte;
	int data[3] = {0x00, 0x00, 0x00};
	int ID[3] = {0x00, 0x93, 0x41};
	int ToF = 1;

	for(i=0; i<3; i++)
    {
		byte = TFT_Read_Register(0xD3, i+1);
        data[i] = byte;

        if(data[i] != ID[i])
        {
            ToF = 0;
        }
    }

    return ToF;
}

void TFT_init()
{
    int i;
	GPIO_InitTypeDef gpio;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	gpio.GPIO_Pin = PIN_LED | PIN_DC | PIN_RST | PIN_CS;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TFT_PORT, &gpio);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	gpio.GPIO_Pin = PIN_SCK | PIN_MOSI;
	gpio.GPIO_Mode = GPIO_Mode_AF;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(TFT_PORT, &gpio);

    gpio.GPIO_Pin = PIN_MISO;
	gpio.GPIO_Mode = GPIO_PuPd_UP;
    GPIO_Init(TFT_PORT, &gpio);

	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(TFT_PORT, PIN_SOURCE_SCK, GPIO_AF_5);
	GPIO_PinAFConfig(TFT_PORT, PIN_SOURCE_MISO, GPIO_AF_5);
	GPIO_PinAFConfig(TFT_PORT, PIN_SOURCE_MOSI, GPIO_AF_5);

    //SPI_StructInit(&spi);
	SPI_InitTypeDef spi;
	spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	spi.SPI_Mode = SPI_Mode_Master;
	spi.SPI_DataSize = SPI_DataSize_8b;
	spi.SPI_CPOL = SPI_CPOL_Low;
	spi.SPI_CPHA = SPI_CPHA_1Edge;
	spi.SPI_NSS = SPI_NSS_Soft;
	spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	spi.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(TFT_SPI, &spi);
	SPI_Cmd(TFT_SPI, ENABLE);
	SPI_NSSInternalSoftwareConfig(SPI1, SPI_NSSInternalSoft_Set);

	TFT_RST_LOW;
	for(i=0; i<0x0FFFF; i++);
	TFT_RST_HIGH;

    TFT_sendCMD(0xCB);
	TFT_sendDATA(0x39);
	TFT_sendDATA(0x2C);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x34);
	TFT_sendDATA(0x02);

	TFT_sendCMD(0xCF);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0xC1);
	TFT_sendDATA(0x30);

	TFT_sendCMD(0xE8);
	TFT_sendDATA(0x85);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x78);

	TFT_sendCMD(0xEA);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x00);

	TFT_sendCMD(0xED);
	TFT_sendDATA(0x64);
	TFT_sendDATA(0x03);
	TFT_sendDATA(0x12);
	TFT_sendDATA(0x81);

	TFT_sendCMD(0xF7);
	TFT_sendDATA(0x20);

	TFT_sendCMD(0xC0);    	//Power control
	TFT_sendDATA(0x23);   	//VRH[5:0]

	TFT_sendCMD(0xC1);    	//Power control
	TFT_sendDATA(0x10);   	//SAP[2:0];BT[3:0]

	TFT_sendCMD(0xC5);    	//VCM control
	TFT_sendDATA(0x3e);   	//Contrast
	TFT_sendDATA(0x28);

	TFT_sendCMD(0xC7);    	//VCM control2
	TFT_sendDATA(0x86);  	 //--

	TFT_sendCMD(0x36);    	// Memory Access Control
	TFT_sendDATA(0x48);  	//C8	   //48 68绔栧睆//28 E8 妯睆

	TFT_sendCMD(0x3A);
	TFT_sendDATA(0x55);

	TFT_sendCMD(0xB1);
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x18);

	TFT_sendCMD(0xB6);    	// Display Function Control
	TFT_sendDATA(0x08);
	TFT_sendDATA(0x82);
	TFT_sendDATA(0x27);

	TFT_sendCMD(0xF2);    	// 3Gamma Function Disable
	TFT_sendDATA(0x00);

	TFT_sendCMD(0x26);    	//Gamma curve selected
	TFT_sendDATA(0x01);

	TFT_sendCMD(0xE0);    	//Set Gamma
	TFT_sendDATA(0x0F);
	TFT_sendDATA(0x31);
	TFT_sendDATA(0x2B);
	TFT_sendDATA(0x0C);
	TFT_sendDATA(0x0E);
	TFT_sendDATA(0x08);
	TFT_sendDATA(0x4E);
	TFT_sendDATA(0xF1);
	TFT_sendDATA(0x37);
	TFT_sendDATA(0x07);
	TFT_sendDATA(0x10);
	TFT_sendDATA(0x03);
	TFT_sendDATA(0x0E);
	TFT_sendDATA(0x09);
	TFT_sendDATA(0x00);

	TFT_sendCMD(0xE1);    	//Set Gamma
	TFT_sendDATA(0x00);
	TFT_sendDATA(0x0E);
	TFT_sendDATA(0x14);
	TFT_sendDATA(0x03);
	TFT_sendDATA(0x11);
	TFT_sendDATA(0x07);
	TFT_sendDATA(0x31);
	TFT_sendDATA(0xC1);
	TFT_sendDATA(0x48);
	TFT_sendDATA(0x08);
	TFT_sendDATA(0x0F);
	TFT_sendDATA(0x0C);
	TFT_sendDATA(0x31);
	TFT_sendDATA(0x36);
	TFT_sendDATA(0x0F);

	TFT_sendCMD(0x11);    	//Exit Sleep
	for(i=0; i<0x00FFF; i++);

	TFT_sendCMD(0x29);    //Display on
	TFT_sendCMD(0x2c);
}

void TFT_led(int state)
{
	if (state)
		GPIO_WriteBit(TFT_PORT, PIN_LED, Bit_SET);
	else
		GPIO_WriteBit(TFT_PORT, PIN_LED, Bit_RESET);
}

void TFT_setCol(int StartCol, int EndCol)
{
	TFT_sendCMD(0x2A);                                                      // Column Command address
	TFT_sendWord(StartCol);
	TFT_sendWord(EndCol);
}

void TFT_setPage(int StartPage, int EndPage)
{
	TFT_sendCMD(0x2B);                                                      // Column Command address
	TFT_sendWord(StartPage);
	TFT_sendWord(EndPage);
}

void TFT_setXY(int poX, int poY)
{
	TFT_setCol(poX, poX);
	TFT_setPage(poY, poY);
	TFT_sendCMD(0x2c);
}

void TFT_setPixel(int poX, int poY, int color)
{
	TFT_setXY(poX, poY);
	TFT_sendWord(color);
}

int mainXXX(void)
{
	int i, j;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	TFT_led(1);
	TFT_init();
	TFT_setPixel(10, 10, RED);

	for(i=0; i<100; i++) {
		TFT_setPixel(i, 50, RED);
		TFT_setPixel(i, 51, RED);

		TFT_setPixel(50, i, GREEN);
		TFT_setPixel(51, i, GREEN);
	}

	for(i=150; i<200; i++) {
		for(j=150; j<200; j++) {
			TFT_setPixel(i, j, RED);
		}
	}

	while(1)
	{
	}
}
