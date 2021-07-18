void ceksim() { ///CONNECT AT
  for (indeksA = 0; indeksA < 20; indeksA++) {
    indeksB = ConnectAT(F("AT"), 200);
    if (indeksB == 20) {
      Serial.println(F("GSM MODULE OK!!"));
      Serial.flush();
      GSMerror = 0;
      break;
    }
    if (indeksB < 20) {
      Serial.print(indeksA);
      Serial.println(F(" GSM MODULE ERROR"));
      Serial.flush();
      GSMerror = 1;
      indikator();
    }
  delay(500);
  }
}

void regsim() {
  indeksA = 0;
  result = "";
  //Functionality
  Serial.println(F("Set Functionality"));
  gsm.println(F("AT+CFUN=1"));
  readSerial(200);
  delay(1500);

  //opsi jika tidak terhubung ke network
  indeksA=0;
  while (indeksA <= 10) {
    sim();
    indeksA++;
    if (json.length() > 0)  {
      GSMerror = 0;
      break;
    }
    if (indeksA == 10) {
      Serial.println(F("TIDAK ADA OPERATOR"));
      Serial.flush();
      GSMerror = 1;
    }
  }
}

void sim() {
  result = "";
  json = "";
  //REGISTASI NETWORK
  Serial.println(F("Registrasi Network"));
  gsm.println(F("AT+CREG=1"));
  readSerial(500);
  delay(1500);

  //CARI OPERATOR KARTU
  Serial.println(F("Cari operator"));
  gsm.println(F("AT+COPS?"));
  Serial.flush();
  gsm.flush();
  delay(200);
  while (gsm.available() > 0) {
    if (gsm.find("+COPS:")) {
      while (gsm.available() > 0) {
        karakter = gsm.read();
        result += karakter;
      }
    }
  }
  Serial.flush();
  gsm.flush();

  indeksA = result.indexOf('"');
  indeksB = result.indexOf('"', indeksA + 1);
  json = result.substring(indeksA + 1, indeksB);
  
  //Nama operator
  Serial.println(json);
  Serial.flush();
  if (json.length() == 0) GSMerror = 1;
  if (json.length() > 0) GSMerror = 0;
  delay(1500);
}

void waktu() {
//  on();
  //WAKTU LOKAL BERDASARKAN NETWORK GSM
  Serial.println(F("Minta waktu lokal ke network GSM"));
  gsm.println(F("AT+CLTS=1"));
  readSerial(500);
  delay(500);
  gsm.println(F("AT+CCLK?"));
  kalimat = "";
  start = millis();
  while ((start + 200) > millis()) {
    while (gsm.available() > 0) {
      karakter = gsm.read();
      kalimat = kalimat + karakter;
    }
  }
  gsm.flush();
  Serial.println(kalimat);
  Serial.flush();
  delay(500);
  tmElements_t tm;
  //parse string
  indeksB = 0;

  //tahun
  indeksA = kalimat.indexOf("\"", indeksB + 1);
  indeksB = kalimat.indexOf("/", indeksA + 1);
  tahun = 2000 + kalimat.substring(indeksA + 1, indeksB).toInt();

  //bulan
  indeksA = kalimat.indexOf("/", indeksB + 1);
  bulan = kalimat.substring(indeksB + 1, indeksA).toInt();

  //hari
  indeksB = kalimat.indexOf(",", indeksA + 1);
  hari = kalimat.substring(indeksA + 1, indeksB).toInt();

  //jam
  indeksA = kalimat.indexOf(":", indeksB + 1);
  jam = kalimat.substring(indeksB + 1, indeksA).toInt();

  //menit
  indeksB = kalimat.indexOf(":", indeksA + 1);
  menit = kalimat.substring(indeksA + 1, indeksB).toInt();

  //detik
  indeksA = kalimat.indexOf("+", indeksB + 1);
  detik = kalimat.substring(indeksB + 1, indeksA).toInt();

//debug
  //  Serial.println(tahun);
  //  Serial.println(bulan);
  //  Serial.println(hari);
  //  Serial.println(jam);
  //  Serial.println(menit);
  //  Serial.println(detik);

  if (tahun >= 2019) {
    tm.Year = CalendarYrToTm(tahun);
    tm.Month = bulan;
    tm.Day = hari;
    tm.Hour = jam;
    tm.Minute = menit;
    tm.Second = detik;
    Waktu = makeTime(tm);
    RTC.set(Waktu); 
    setTime(Waktu);
  }

  printDateTime(Waktu);
  Serial.flush();
//  off();
}

byte ConnectAT(String cmd, int d) {
  i = 0;
//  s_on();
//  power_timer0_enable();
  while (1) {
    gsm.println(cmd);
    while (gsm.available()) {
      if (gsm.find("OK"))
        i = 20;
    }
    delay(d);
    if (i > 5) {
      break;
    }
    i++;
  }
  return i;
}

void readSerial(unsigned int wait) {
  char karakter = -2;
  unsigned long mulai = millis();
  while (millis() - mulai <= wait)  {
    karakter = gsm.read();
#ifdef debug
    if (isAscii(karakter)) Serial.write(karakter);
#endif
  }
}

void GSMreset() { //WAKE UP GSM
  Serial.println(F(" "));
  gsm.println(F("AT+CSCLK=0"));
  Serial.flush();
  gsm.flush();
  delay(200);
  gsm.println(F("AT+CSCLK=0"));
  gsm.flush();
  readSerial(200);
  Serial.flush();
  gsm.flush();
  delay(2000);
//  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}

void GSMsleep() {
//  on();
  gsmTidur = 1;
  Serial.println(F("GSM SLEEP"));
  gsm.println(F("AT+CSCLK=2"));
  gsm.flush();
  readSerial(200);
}

void kirimSMS() {
  gsm.println(F("AT+CMGF=1"));
  gsm.flush();
  readSerial(200);
  gsm.print(F("AT+CMGS=\""));
  gsm.print(noHP);
  gsm.println(F("\""));
  gsm.flush();
  readSerial(200);

  gsm.print(F("PERINGATAN!!!\r\n Nilai tekanan unit "));
  gsm.print(ID);
  gsm.print(F(" berada "));
  if (tekanan < batasBawah){
    gsm.print(F("di bawah"));
  
  }
  if (tekanan > batasAtas){
    gsm.print(F("di atas"));
  }

  gsm.print(F(" ambang batas.\r\nNilai = "));
  gsm.print(tekanan);
  gsm.println(F(" bar"));
  gsm.flush();
  readSerial(500);
  gsm.write(26);
  readSerial(500);

}
