void initSD() {
  pinMode(cs, OUTPUT);
  digitalWrite(cs, HIGH);
  delay(200);

  if (!SD.begin(cs)) {
    while (1) {
      Serial.println(F("SD init error!!!"));
      display.getTextBounds(F("SD Card Error!!!"), 0, 0, &posx, &posy, &w, &h);
      display.setCursor((128 - w) / 2, 40);
      display.println(F("SD Card Error!!!"));
      display.display();
      delay(5000);
    }
  }
}

void simpandata() {
//  SdFile::dateTimeCallback(dateTime);
  filename = "";
  filename = String(tahun);
  if (bulan < 10) {
    filename += "0" + String(bulan);
  }
  if (bulan >= 10) {
    filename += String(bulan);
  }
  if (hari < 10) {
    filename += "0" + String(hari);
  }
  if (hari >= 10) {
    filename += String(hari);
  }
  filename += ".txt";

  file = SD.open(filename, FILE_READ);
  a = file.available();
  file.close();
  // set date time callback function
//  SdFile::dateTimeCallback(dateTime);
  file = SD.open(filename, FILE_WRITE);

  if (a == 0) {
    Serial.println(filename);
    file.print(F("Date (YYYY-MM-DD HH:MM:SS),"));
    file.print(F("ID,Pressure (BAR),Temperature (°C), Humidity (RH),"));
    file.println(F("Voltage (V),Burst interval (SECOND),Data Interval (MINUTE),Network Code,Response")); //
    // SdFile::dateTimeCallback(dateTime);
  }

  json = "";
  json = String(tahun) + '/';
  if (bulan < 10) {
    json += "0" + String(bulan);
  }
  if (bulan >= 10) {
    json += String(bulan);
  }
  json += "/";
  if (hari < 10) {
    json += "0" + String(hari);
  }
  if (hari >= 10) {
    json += String(hari);
  }
  json += " ";
  if (jam < 10) {
    json += "0" + String(jam);
  }
  if (jam >= 10) {
    json += String(jam);
  }
  json += ":";
  if (menit < 10) {
    json += "0" + String(menit);
  }
  if (menit >= 10) {
    json += String(menit);
  }
  json += ":";
  if (detik < 10) {
    json += "0" + String(detik);
  }
  if (detik >= 10) {
    json += String(detik);
  }
  json += "," + String(ID) + "," + String(tekanan, 2) + "," + String(temp, 2);
  json += "," +  String(humid, 2) + "," + String(volt, 2) + "," + String(burst) + "," + String(interval);

  Serial.println(json);
  file.print(json);
  delay(100);
  file.close();
}
/*
void dateTime(uint16_t* date, uint16_t* time) {
  ESPtime = rtc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(ESPtime.year(), ESPtime.month(), ESPtime.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(ESPtime.hour(), ESPtime.minute(), ESPtime.second());
}
*/
void hapusmenu(byte h, byte h1) {
  display.fillRect(0, h, 128, h1, BLACK); //clear display
  display.display();
  display.setTextColor(WHITE, BLACK);
}

void lcd2digits(int number) {
  if (number >= 0 && number < 10) {
    display.write('0');
  }
  display.print(number);
}

void configs() {
  i = 0;
  file = SD.open("config.txt", FILE_READ);
  if (file) {
    while (file.available()) {
      sdcard[i++] = file.read();
    }
  }
  else  {

    while (1) {
      Serial.println(F("ERROR READING!!!"));
      display.getTextBounds(F("ERROR READING!!!"), 0, 0, &posx, &posy, &w, &h);
      display.setCursor((128 - w) / 2, 40);
      display.println(F("ERROR READING!!!"));
      display.display();
      delay(10000);
    }
  }
  file.close();

  filename = String(sdcard);
  for ( a = 0; a < sizeof(sdcard); a++) {
    sdcard[a] = (char)0;
  }
  Serial.println(filename);
  display.getTextBounds(F("SUCCESS!!!"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 40);
  display.println(F("SUCCESS!!!"));
  display.display();
  Serial.println(F("GET CONFIG SUCCESS!!!"));

  b = 0;

  //NAMA STASIUN
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  hotspot = filename.substring(a + 2, b);

  //INTERVAL
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  interval = filename.substring(a + 1, b).toInt();

  //BURST
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  burst = filename.substring(a + 1, b).toInt();

  //OFFSET
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  json = filename.substring(a + 2, b);
  offset = json.toFloat();

  //LATITUDE
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  json = filename.substring(a + 2, b);
  flat = json.toFloat();

  //LONGITUDE
  a = filename.indexOf("=", b + 1);
  b = filename.indexOf("\r", a + 1);
  json = filename.substring(a + 2, b);
  flon = json.toFloat();

  filename = '0';

  Serial.print(F("NAMA STASIUN = "));
  Serial.println(hotspot);
  Serial.print(F("Interval data = "));
  Serial.print(interval);
  if (interval < 2)
    Serial.println(F(" minute"));
  else Serial.println(F(" minutes"));
  Serial.print(F("Burst Interval = "));
  Serial.print(burst);
  if (interval < 2)
    Serial.println(F(" second"));
  else Serial.println(F(" seconds"));
  Serial.print(F("OFFSET = "));
  Serial.println(offset, 4);
  Serial.print(F("LATITUDE = "));
  Serial.println(flat, 4);
  Serial.print(F("LONGITUDE = "));
  Serial.println(flon, 4);
  Serial.flush();
}
void bersihdata() {
  reads1 = 0; reads = 0; tekanan = 0.00; temp = 0.00; humid = 0.00; volt = 0.00; filename = ""; json = "";
}

void initDHT() {
  Serial.println(F("INIT DHT"));
  Serial.flush();
  hapusmenu(17, 64);
  dht.begin();
  display.getTextBounds(F("INIT DHT11"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 20);
  display.print(F("INIT DHT"));
  display.println(DHTTYPE);
  display.display();
  delay(1000);
  display.getTextBounds(F("DHT READY!"), 0, 0, &posx, &posy, &w, &h);
  display.setCursor((128 - w) / 2, 40);
  display.println(F("DHT READY!"));
  display.display();
  delay(2000);
}
