void indikator() {
  pinMode(Bled, OUTPUT);
  for (byte ind = 0; ind < 2; ind++) {
    digitalWrite(Bled, HIGH);
    delay(200);
//    LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
    digitalWrite(Bled, LOW);
    delay(200);
//    LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
  }
}

void inisialisasi() {
  Serial.begin(baud);
  Serial.println(F("\r\nInisialisasi serial GSM"));
  Serial.flush();
  gsm.begin(baud);  // SIM900A
  pinMode(13, OUTPUT);
  pinMode(wakeupPin, INPUT_PULLUP); // Set interrupt pin

}
