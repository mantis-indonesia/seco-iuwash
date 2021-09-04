void cekRTC(){
  Serial.println(F("Cek RTC"));
  Wire.begin();
  Wire.beginTransmission(rtc_addr);
  indeksA = Wire.endTransmission();
  if (indeksA == 0) {
    Serial.println(F("RTC ada"));
  }
  if (indeksA != 0) {
    Serial.println(F("RTC Error"));
    digitalWrite(13, HIGH);
    while (1);
  }
  Wire.end();
  setSyncProvider(RTC.get);
}

void ambilWaktu(){
  Waktu = RTC.get();
  tahun = year(Waktu);
  bulan = month(Waktu);
  hari = day(Waktu);
  jam = hour(Waktu);
  menit = minute(Waktu);
  detik = second(Waktu);
  
  if (tahun >= 2080 || tahun < 2021){
  waktu();
  Waktu = RTC.get();
  }

  alarmTime = Waktu + ALARM_INTERVAL;

  sprintf(sTime,"%d-%02d-%02dT%02d:%02d:%02d.000Z",tahun,bulan,hari,jam,menit,detik);
}

void cekwaktu() { // tidur hingga waktu pengambilan data yang disuruh
  Waktu = RTC.get();
  printDateTime(Waktu) ;
  menit = minute(Waktu);
  Serial.print(F("menit sekarang = "));
  Serial.println(menit);
  Serial.print("Tidur hingga ");
  for (byte i = 0; i < pembagiWaktu; i++) {
    if (menit >= 60 - interval) {
      Serial.print(60 - menit);
      menit = 60 - menit;
      break;
    }
    if (menit >= i * interval && menit < (i + 1) * interval) {
      Serial.print((i + 1) * interval - menit);
      menit = (i + 1) * interval - menit;
      break;
    }
  }
  Serial.println(" menit");
  Serial.flush();

  alarmTime = Waktu + menit * 60;  // calculate the alarm time
  Serial.print(hour(alarmTime));
  Serial.print(":");
  Serial.print(minute(alarmTime));
  Serial.print(":");
  Serial.println("0");

  // set the alarm
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, minute(alarmTime), hour(alarmTime), 0);
  // clear the alarm flag
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, true); // Enable alarm 1 interrupt A1IE
}

void printDateTime(time_t t) {
  Serial.print(day(t));
  Serial.print("/");
  Serial.print(month(t));
  Serial.print("/");
  Serial.print(year(t));
  Serial.print(" ");
  Serial.print(hour(t));
  Serial.print(":");
  Serial.print(minute(t));
  Serial.print(":");
  Serial.println(second(t));
  Serial.flush();
}

void clearRTC() {
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);
}
