//handling peripheral
#include <Wire.h>

// from  http://heliosoph.mit-links.info/arduino-powered-by-capacitor-reducing-consumption/
//defines for DIDR0 setting
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// create variables to restore the SPI & ADC register values after shutdown
byte keep_ADCSRA;
byte keep_SPCR;

void indikator() {
  pinMode(13, OUTPUT);
  for (byte ind = 0; ind < 2; ind++) {
    digitalWrite(13, HIGH);
    LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
    digitalWrite(13, LOW);
    LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
  }
}

void timerOn() {
  power_timer0_enable();// Timer 0   >> millis() will stop working
}

void off() {
  power_adc_disable(); // ADC converter
  power_spi_disable(); // SPI
  power_usart0_disable(); // Serial (USART)
  power_timer0_disable();// Timer 0   >> millis() will stop working
  power_timer1_disable();// Timer 1   >> analogWrite() will stop working.
  power_timer2_disable();// Timer 2   >> tone() will stop working.
  power_twi_disable(); // TWI (I2C)
}

void on() {
  power_adc_enable();
  power_spi_enable(); // SPI
  power_usart0_enable(); // Serial (USART)
  power_timer0_enable();// Timer 0   >> millis() working
  power_timer1_enable();// Timer 1   >> analogWrite() working.
  power_timer2_enable();// Timer 2   >> tone() working.
  power_twi_enable(); // TWI (I2C)
}

void s_on() {
  power_usart0_enable();
  power_timer0_enable();
}

void s_off() {
  power_usart0_disable();
}

void i_En(byte alamat) {
  power_twi_enable();
  power_timer0_enable();
  Wire.begin(alamat);
}

void i_Dis() {
  power_twi_disable();
  power_timer0_disable();
}

void spiEn(byte SSpin) {
  power_timer0_enable();
  pinMode(SSpin, OUTPUT);
  digitalWrite(SSpin, HIGH);
  power_spi_enable();
  SPCR = keep_SPCR;
  delay(100);
}

void spiDis(byte SSpin) {
  delay(100);
  SPCR = 0;
  power_spi_disable();
  pinMode(SSpin, OUTPUT);
  digitalWrite(SSpin, LOW);
  LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
}
