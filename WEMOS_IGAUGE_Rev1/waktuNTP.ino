void initRTC() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1) {
      display.getTextBounds(F("ERROR!! CONTACT CS."), 0, 0, &posx, &posy, &w, &h);
      display.setCursor((128 - w) / 2, 40);
      display.println(F("ERROR!! CONTACT CS."));
      display.display();
    }
  }

  for (i = 1; i < 4; i++) {
    ESPtime = rtc.now();
    filename = String(ESPtime.month()) + '/' + String(ESPtime.day()) + '/' + String(ESPtime.year()) + ' ' + String(ESPtime.hour()) + ':' + String(ESPtime.minute()) + ':' + String(ESPtime.second());
    Serial.println(filename);
    display.getTextBounds(F("yyyy/mm/dd hh:mm:ss"), 0, 0, &posx, &posy, &w, &h);
    display.setCursor((128 - w) / 2, 40);
    display.print(ESPtime.year());
    display.print("/");
    lcd2digits(ESPtime.month());
    display.print("/");
    lcd2digits(ESPtime.day());
    display.print(" ");
    lcd2digits(ESPtime.hour());
    display.print(":");
    lcd2digits(ESPtime.minute());
    display.print(":");
    lcd2digits(ESPtime.second());
    display.display();
    delay(1000);
  }
}

void displaydate() {
  ESPtime = rtc.now();
  tahun = ESPtime.year();
  bulan = ESPtime.month();
  hari = ESPtime.day();
  jam = ESPtime.hour();
  menit = ESPtime.minute();
  detik = ESPtime.second();
  
  display.getTextBounds(F("yyyy/mm/dd hh:mm"), 0, 0, &posx, &posy, &w, &h);
  display.setTextColor(WHITE, BLACK);
  display.setCursor((128 - w), 0);
  display.print(ESPtime.year()); display.print("/");
  lcd2digits(ESPtime.month()); display.print("/");
  lcd2digits(ESPtime.day()); display.print(" ");
  lcd2digits(ESPtime.hour()); display.print(":");
  lcd2digits(ESPtime.minute());
  display.display();
 
  sprintf(sTime, "%d-%02d-%02dT%02d:%02d:%02d.000Z", tahun, bulan, hari, jam, menit, detik);
//  Serial.println(sTime);
}

void ambilWaktu() {
  for (i = 1; i <= 3; i++) {
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    delay(1000);
  }

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();
  jam = timeClient.getHours();
  menit = timeClient.getMinutes();
  detik = timeClient.getSeconds();
  //Get a time structure
  struct tm *ptm = gmtime ((time_t *)&epochTime);
  hari = ptm->tm_mday;
  bulan = ptm->tm_mon + 1;
  tahun = ptm->tm_year + 1900;

  //Print complete date:
  String currentDate = String(tahun) + "-" + String(bulan) + "-" + String(hari);
  Serial.print("Tanggal: ");
  Serial.println(currentDate);
  Serial.println("");

  sprintf(sTime, "%d-%02d-%02dT%02d:%02d:%02d.000Z", tahun, bulan, hari, jam, menit, detik);
  Serial.println(sTime);
  
  //  rtc.adjust(DateTime(tahun, bulan, hari, jam, menit, detik));
  if (tahun >= 2021)  rtc.adjust(DateTime(tahun, bulan, hari, jam, menit, detik));

}
