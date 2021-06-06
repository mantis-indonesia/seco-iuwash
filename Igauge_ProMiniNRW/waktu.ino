void cekwaktu() { // tidur hingga waktu pengambilan data yang disuruh
  t = RTC.get();
  printDateTime(t) ;
  menit = minute(t);
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

  alarmTime = t + menit * 60;  // calculate the alarm time
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



String strTwoDigit(float nilai) {
  String result = String(nilai, 2);
  String angka, digit;
  indeksB = 0;
  indeksA = result.indexOf(".", indeksB + 1);
  digit = result.substring(indeksA + 1, result.length());
  angka = result.substring(0, indeksA);
  if (result.length() == 4) { //4.12
    angka = "00" + angka + digit;
  }
  if (result.length() == 5) { //51.23
    angka = "0" + angka + digit;
  }
  if (result.length() > 5) { //123.45
    angka = angka + digit;
  }
  return angka;
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
