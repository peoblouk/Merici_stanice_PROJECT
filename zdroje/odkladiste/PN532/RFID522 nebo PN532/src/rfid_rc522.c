/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 01-02-2023 - 19:48:00
 * @modify date 22-03-2023 - 15:46:16
 * @desc [Mifare MFRC522 RFID Card reader library]
 */

#include "rfid_rc522.h"

/////////////////////////////////////////////////////////////////////
//! Funkce na inicializaci RFID čtečky
void RC522_Init(void)
{
	// Clock konfigurace
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE); // Nahrada za MFRC522_CS_RCC
	RC522_InitPins();

	// SPI konfigurace
	SPI_DeInit();
	CS_L;
	SPI_Init(SPI_FIRSTBIT_MSB, SPI_BAUDRATEPRESCALER_4, SPI_MODE_MASTER,
			 SPI_CLOCKPOLARITY_HIGH, SPI_CLOCKPHASE_2EDGE, SPI_DATADIRECTION_2LINES_FULLDUPLEX,
			 SPI_NSS_SOFT, 0x07);
	SPI_Cmd(ENABLE);

	RC522_Reset();

	RC522_WriteRegister(MFRC522_REG_T_MODE, 0x8D);		// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	RC522_WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E); // Předdělička
	RC522_WriteRegister(MFRC522_REG_T_RELOAD_L, 30);	// Reload registru
	RC522_WriteRegister(MFRC522_REG_T_RELOAD_H, 0);

	/* 48dB gain */
	RC522_WriteRegister(MFRC522_REG_RF_CFG, 0x70);
	RC522_WriteRegister(MFRC522_REG_TX_AUTO, 0x40); // Defaultně 100% modulace - oddělený register ModGsPReg
	RC522_WriteRegister(MFRC522_REG_MODE, 0x3D);
	RC522_AntennaOn();			 // Zapni anténu
	SPI_SendData(MFRC522_DUMMY); // TODO Nevím úplně proč poslat dva DUMMY bity
	SPI_SendData(MFRC522_DUMMY);
}
/////////////////////////////////////////////////////////////////////
//! Funkce pro kontrolu ID karty
RC522_Status_t RC522_Check(uint8_t *id)
{
	RC522_Status_t status;
	// Find cards, return card type
	status = RC522_Request(PICC_REQIDL, id);
	if (status == MI_OK)
	{
		// Card detected
		// Anti-collision, return card serial number 4 bytes
		status = RC522_Anticoll(id);
	}
	else
	{
		status = MI_ERR;
	}
	RC522_Halt(); // Command card into hibernation

	return status;
}
/////////////////////////////////////////////////////////////////////
//! Funkce pro porovnání dvou ID karet
RC522_Status_t RC522_Compare(uint8_t *CardID, uint8_t *CompareID)
{
	uint8_t i;
	for (i = 0; i < 5; i++)
	{
		if (CardID[i] != CompareID[i])
		{
			return MI_ERR;
		}
	}
	return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//! Funkce na inicializaaci pinů pro práci s SPI
void RC522_InitPins(void)
{
	// Enable clock
	CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE);

	// Enable external pullup
	GPIO_ExternalPullUpConfig(SPI_PORT, (GPIO_Pin_TypeDef)(SPI_MISO | SPI_MOSI | SPI_SCK), ENABLE);

	// CS pin
	GPIO_Init(CHIP_SELECT_PORT, CHIP_SELECT_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW); // Inicializace CS
	GPIO_Init(SPI_PORT, SPI_RST, GPIO_MODE_OUT_PP_HIGH_SLOW);				  // Inicializace RST

	// Reset RFID čtečky
	GPIO_WriteHigh(SPI_PORT, SPI_RST);
	GPIO_WriteLow(SPI_PORT, SPI_RST);
	CS_H; // Konec komunikace
}
/////////////////////////////////////////////////////////////////////
//! Funkce na zápis do registru MFRC522
void RC522_WriteRegister(uint8_t addr, uint8_t val)
{
	CS_L;							   // Začátek komunikace
	SPI_SendData((addr << 1) & 0x7E);  // Zvolení registru
	SPI_SendData(val);				   // Posílání dat
	GPIO_WriteHigh(GPIOD, GPIO_PIN_4); //! TEST
	while ((SPI_GetFlagStatus(SPI_SR_TXE)) == RESET)
		; // Čekám, než obdržím něco do bufferu

	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
		;

	CS_H; // Konec komunikace
}
/////////////////////////////////////////////////////////////////////
//! Funkce na čtení z registru MFRC522
char temp = 0;
uint8_t RC522_ReadRegister(uint8_t addr)
{
	uint8_t val = 0;						   // Lokální proměnná pro vracení hodnoty, kterou jsme si přečetli z registru
	CS_L;									   // Začátek komunikace
	SPI_SendData(((addr << 1) & 0x7E) | 0x80); // Pošli data
											   // SPI_SendData(MFRC522_DUMMY);			   // Pošli data
	while ((SPI_GetFlagStatus(SPI_SR_TXE)) == 0)
		; // Čekám, než obdržím něco do bufferu

	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
		;
	temp = SPI_ReceiveData(); // Ulož data do proměnné
	SPI_SendData(MFRC522_DUMMY);

	//? Opakuji
	while ((SPI_GetFlagStatus(SPI_SR_TXE)) == 0)
		; // Čekám, než obdržím něco do bufferu
	while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET)
		;
	val = SPI_ReceiveData(); // Ulož data do proměnné
	CS_H;					 // Ukončení komunikace
	return val;
}
/////////////////////////////////////////////////////////////////////
//! Funkce na nastavení masky
void RC522_SetBitMask(uint8_t reg, uint8_t mask)
{
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) | mask); // Zapiš do registru
}
/////////////////////////////////////////////////////////////////////
void RC522_ClearBitMask(uint8_t reg, uint8_t mask)
{
	RC522_WriteRegister(reg, RC522_ReadRegister(reg) & (~mask));
}
/////////////////////////////////////////////////////////////////////
//! Funkce na zapnutí antény MFRC522
void RC522_AntennaOn(void)
{
	uint8_t temp;

	temp = RC522_ReadRegister(MFRC522_REG_TX_CONTROL);
	if (!(temp & 0x03))
	{
		RC522_SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
	}
}
/////////////////////////////////////////////////////////////////////
//! Funkce na vypnutí antény MFRC522
void RC522_AntennaOff(void)
{
	RC522_ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}
/////////////////////////////////////////////////////////////////////
//! Funkce na reset registru
void RC522_Reset(void)
{
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}
/////////////////////////////////////////////////////////////////////
// TODO zjistit k čemu tato funkce je
RC522_Status_t RC522_Request(uint8_t reqMode, uint8_t *TagType)
{
	RC522_Status_t status;
	uint16_t backBits; // The received data bits

	RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07); // TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = RC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10))
	{
		status = MI_ERR;
	}

	return status;
}
/////////////////////////////////////////////////////////////////////
//! Funkce na zápis na kartu
RC522_Status_t RC522_ToCard(uint8_t command, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint16_t *backLen)
{
	RC522_Status_t status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;
	uint8_t lastBits;
	uint8_t n;
	uint16_t i;

	switch (command)
	{
	case PCD_AUTHENT:
	{
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	}
	case PCD_TRANSCEIVE:
	{
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	}
	default:
		break;
	}

	RC522_WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	RC522_ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	RC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i < sendLen; i++)
	{
		RC522_WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
	}

	// Execute the command
	RC522_WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE)
	{
		RC522_SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=1,transmission of data starts
	}

	// Waiting to receive data to complete
	i = 2000; // i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	do
	{
		// CommIrqReg[7..0]
		// Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = RC522_ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	} while ((i != 0) && !(n & 0x01) && !(n & waitIRq));

	RC522_ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80); // StartSend=0

	if (i != 0)
	{
		if (!(RC522_ReadRegister(MFRC522_REG_ERROR) & 0x1B))
		{
			status = MI_OK;
			if (n & irqEn & 0x01)
			{
				status = MI_NOTAGERR;
			}

			if (command == PCD_TRANSCEIVE)
			{
				n = RC522_ReadRegister(MFRC522_REG_FIFO_LEVEL);
				lastBits = RC522_ReadRegister(MFRC522_REG_CONTROL) & 0x07;
				if (lastBits)
				{
					*backLen = (n - 1) * 8 + lastBits;
				}
				else
				{
					*backLen = n * 8;
				}

				if (n == 0)
				{
					n = 1;
				}
				if (n > MFRC522_MAX_LEN)
				{
					n = MFRC522_MAX_LEN;
				}

				// Reading the received data in FIFO
				for (i = 0; i < n; i++)
				{
					backData[i] = RC522_ReadRegister(MFRC522_REG_FIFO_DATA);
				}
			}
		}
		else
		{
			status = MI_ERR;
		}
	}

	return status;
}
/////////////////////////////////////////////////////////////////////
RC522_Status_t RC522_Anticoll(uint8_t *serNum)
{
	RC522_Status_t status;
	uint8_t i;
	uint8_t serNumCheck = 0;
	uint16_t unLen;

	RC522_WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00); // TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = RC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK)
	{
		// Check card serial number
		for (i = 0; i < 4; i++)
		{
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{
			status = MI_ERR;
		}
	}
	return status;
}
/////////////////////////////////////////////////////////////////////
//! Funkce na detekci CRC kodu
void RC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
	uint8_t i, n;

	RC522_ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);	// CRCIrq = 0
	RC522_SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80); // Clear the FIFO pointer
	// Write_MFRC522(CommandReg, PCD_IDLE);

	// Writing data to the FIFO
	for (i = 0; i < len; i++)
	{
		RC522_WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata + i));
	}
	RC522_WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	// Wait CRC calculation is complete
	i = 0xFF;
	do
	{
		n = RC522_ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i != 0) && !(n & 0x04)); // CRCIrq = 1

	// Read CRC calculation result
	pOutData[0] = RC522_ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = RC522_ReadRegister(MFRC522_REG_CRC_RESULT_M);
}
/////////////////////////////////////////////////////////////////////
//! Funkce pro výběr tagu
uint8_t RC522_SelectTag(uint8_t *serNum)
{
	uint8_t i;
	RC522_Status_t status;
	uint8_t size;
	uint16_t recvBits;
	uint8_t buffer[9];

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++)
	{
		buffer[i + 2] = *(serNum + i);
	}
	RC522_CalculateCRC(buffer, 7, &buffer[7]); //??
	status = RC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18))
	{
		size = buffer[0];
	}
	else
	{
		size = 0;
	}

	return size;
}
/////////////////////////////////////////////////////////////////////
//! Funkce zkoušku hesla
RC522_Status_t RC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
	RC522_Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[12];

	// Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++)
	{
		buff[i + 2] = *(Sectorkey + i);
	}
	for (i = 0; i < 4; i++)
	{
		buff[i + 8] = *(serNum + i);
	}
	status = RC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

	if ((status != MI_OK) || (!(RC522_ReadRegister(MFRC522_REG_STATUS2) & 0x08)))
	{
		status = MI_ERR;
	}

	return status;
}
/////////////////////////////////////////////////////////////////////
//! Funkce na čtení po zadání úspěšně hesla
RC522_Status_t RC522_Read(uint8_t blockAddr, uint8_t *recvData)
{
	RC522_Status_t status;
	uint16_t unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	RC522_CalculateCRC(recvData, 2, &recvData[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90))
	{
		status = MI_ERR;
	}

	return status;
}

//! Funkce na zápis na tag po zadání úspěšně hesla
RC522_Status_t RC522_Write(uint8_t blockAddr, uint8_t *writeData)
{
	RC522_Status_t status;
	uint16_t recvBits;
	uint8_t i;
	uint8_t buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	RC522_CalculateCRC(buff, 2, &buff[2]);
	status = RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
	{
		status = MI_ERR;
	}

	if (status == MI_OK)
	{
		// Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++)
		{
			buff[i] = *(writeData + i);
		}
		RC522_CalculateCRC(buff, 16, &buff[16]);
		status = RC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
		{
			status = MI_ERR;
		}
	}

	return status;
}
/////////////////////////////////////////////////////////////////////
//! Funkce na přerušení RFID čtečky
void RC522_Halt(void)
{
	uint16_t unLen;
	uint8_t buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	RC522_CalculateCRC(buff, 2, &buff[2]);

	RC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}
