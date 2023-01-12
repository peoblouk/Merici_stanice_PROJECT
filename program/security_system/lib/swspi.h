// Tyto makra upravujte podle pot�eby 
#define DIN_GPIO 	GPIOB
#define DIN_PIN 	PIN_5
#define CS_GPIO 	GPIOB
#define CS_PIN 		PIN_4
#define CLK_GPIO 	GPIOB
#define CLK_PIN 	PIN_3

void swspi_init(void);
void swspi_tx16(uint16_t data);
