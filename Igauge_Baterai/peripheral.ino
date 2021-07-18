void indikator(byte led) {
  pinMode(led, OUTPUT);
  for (byte ind = 0; ind < 2; ind++) {
    digitalWrite(led, HIGH);
    Alarm.delay(250);
    digitalWrite(led, LOW);
    Alarm.delay(250);
  }
}

void inisialisasi() {
  Serial.begin(baud);
  Serial.flush();
  gsm.begin(baud);  // SIM900A
  //WELCOME SCREEN
  Serial.println(F("\r\nGSM IGAUGE PRO MINI 3V3 2019"));
  Serial.print("Unit = ");
  Serial.println(ID);
}
