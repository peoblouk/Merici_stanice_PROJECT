#include "spse_stm8.h"  


// blok funkc� pro STM s instanc� ADC2
#ifdef STM8S208

/* funkce provede p�evod a vr�t� v�sledek p�evodu ze zvolen�ho kan�lu.
Argument m��e b�t bu� makro: ADC2_CHANNEL_x ... kde x m��e b�t 0 a� 15 nebo ��seln� hodnota 0 a� 15 (odpov�d� ��slu kan�lu)
*/
uint16_t ADC_get(ADC2_Channel_TypeDef ADC2_Channel){
ADC2_Select_Channel(ADC2_Channel); // vybere kan�l / nastavuje analogov� multiplexer
ADC2->CR1 |= ADC2_CR1_ADON; // Start Conversion (ADON must be SET before => ADC must be enabled !)
while(!(ADC2->CSR & ADC2_CSR_EOC)); // �ek� na dokon�en� p�evodu (End Of Conversion)
ADC2->CSR &=~ADC2_CSR_EOC; // ma�e vlajku 
return ADC2_GetConversionValue(); // vrac� v�sledek
}

/* Funkce �etrn� p�epne multiplexer (n�hrada za mizern� �e�en� ze SPL knihoven)
Argument m��e b�t bu� makro: ADC2_CHANNEL_x ... kde x m��e b�t 0 a� 15 nebo ��seln� hodnota 0 a� 15 (odpov�d� ��slu kan�lu)
*/
void ADC2_Select_Channel(ADC2_Channel_TypeDef ADC2_Channel){
    uint8_t tmp = (ADC2->CSR) & (~ADC2_CSR_CH);
    tmp |= ADC2_Channel | ADC2_CSR_EOC;
    ADC2->CSR = tmp;
}

/* Nastav� zarovn�n� v�sledku p�evodu vlevo nebo vpravo
Argument m��e b�t makro ADC2_ALIGN_LEFT nebo ADC2_ALIGN_RIGHT
*/
void ADC2_AlignConfig(ADC2_Align_TypeDef ADC2_Align){
	if(ADC2_Align){
		ADC2->CR2 |= (uint8_t)(ADC2_Align);
	}else{
		ADC2->CR2 &= (uint8_t)(~ADC2_CR2_ALIGN);
	}
}

/* Funkce po�k� definovan� �as na stabilizaci ADC (vhodn� za�adit po spu�t�n� ADC k zaji�t�n� �e se prvn� p�evod neprovede p�ed stabilizac�)
*/
void ADC2_Startup_Wait(void){
	delay.us(ADC_TSTAB);
}

#endif


// blok funkc� pro STM s instanc� ADC1
#ifdef STM8S103

uint16_t ADC_get(ADC1_Channel_TypeDef ADC1_Channel){
ADC1_Select_Channel(ADC1_Channel);
ADC1->CR1 |= ADC1_CR1_ADON; // Start Conversion (ADON must be SET before => ADC must be enabled !)
while(!(ADC1->CSR & ADC1_CSR_EOC));
ADC1->CSR &=~ADC1_CSR_EOC;
return ADC1_GetConversionValue();
}

void ADC1_Select_Channel(ADC1_Channel_TypeDef ADC1_Channel){
    uint8_t tmp = (ADC1->CSR) & (~ADC1_CSR_CH);
    tmp |= ADC1_Channel | ADC1_CSR_EOC;
    ADC1->CSR = tmp;
}

void ADC1_AlignConfig(ADC1_Align_TypeDef ADC1_Align){
	if(ADC1_Align){
		ADC1->CR2 |= (uint8_t)(ADC1_Align);
	}else{
		ADC1->CR2 &= (uint8_t)(~ADC1_CR2_ALIGN);
	}
}

void ADC1_Startup_Wait(void){
	delay.us(ADC_TSTAB);
}

#endif

