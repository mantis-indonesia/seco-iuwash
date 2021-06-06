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
  indeksC = 0;
  result = "";
  //Functionality
  Serial.println(F("Set Functionality"));
  serial1.println(F("AT+CFUN=1"));
  bacaserial(200);
  delay(1500);

  //Pilih Band
  Serial.println(F("Pilih Band"));
  serial1.println(F("AT+CBAND=\"ALL_BAND\""));
  bacaserial(200);
  delay(1500);

  //opsi jika tidak terhubung ke network
  indeksC=0;
  while (indeksC <= 10) {
    sim();
    indeksC++;
    if (operators.length() > 0)  {
      GSMerror = 0;
      break;
    }
    if (indeksC == 10) {
      Serial.println(F("TIDAK ADA OPERATOR"));
      Serial.flush();
      GSMerror = 1;
    }
  }
}

void sim() {
  result = "";
  operators = "";
  //REGISTASI NETWORK
  Serial.println(F("Registrasi Network"));
  serial1.println(F("AT+CREG=1"));
  bacaserial(300);
  delay(1500);

  //CARI OPERATOR KARTU
  Serial.println(F("Cari operator"));
  serial1.println(F("AT+COPS?"));
  Serial.flush();
  serial1.flush();
  delay(200);
  while (serial1.available() > 0) {
    if (serial1.find("+COPS:")) {
      while (serial1.available() > 0) {
        karakter = serial1.read();
        result += karakter;
      }
    }
  }
  Serial.flush();
  serial1.flush();

  indeksA = result.indexOf('"');
  indeksB = result.indexOf('"', indeksA + 1);
  kalimat = result.substring(indeksA + 1, indeksB);
  if (kalimat == "51089") kalimat = "THREE";

  //Nama operator
  operators = kalimat;
  Serial.println(operators);
  Serial.flush();
  if (operators.length() == 0) GSMerror = 1;
  if (operators.length() > 0) GSMerror = 0;
  delay(1500);
}

void waktu() {
  on();
  //WAKTU LOKAL BERDASARKAN NETWORK GSM
  Serial.println(F("Minta waktu lokal ke network GSM"));
  serial1.println(F("AT+CLTS=1"));
  bacaserial(300);
  delay(500);
  serial1.println(F("AT+CCLK?"));
  kalimat = "";
  start = millis();
  while ((start + 200) > millis()) {
    while (serial1.available() > 0) {
      karakter = serial1.read();
      kalimat = kalimat + karakter;
    }
  }
  serial1.flush();
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
    t = makeTime(tm);
    RTC.set(t); 
    setTime(t);
  }

  printDateTime(t);
  Serial.flush();
  off();
}

void apn(String nama) {
  if (nama == "TELKOMSEL") {
    APN = "Telkomsel"; //internet
    USER = "wap";
    PWD = "wap123";
  }
  if (nama == "INDOSAT") {
    APN = "indosatgprs";
    USER = "indosat";
    PWD = "indosatgprs";
  }
  if (nama == "EXCELCOM" || nama == "XL") {
    APN = "internet";
    USER = "";
    PWD = "";
  }
  if (nama == "THREE" || nama == "3") {
    APN = "3data";
    USER = "3data";
    PWD = "3data";
  }
}

byte ConnectAT(String cmd, int d) {
  i = 0;
  s_on();
  power_timer0_enable();
  while (1) {
    serial1.println(cmd);
    while (serial1.available()) {
      if (serial1.find("OK"))
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

void bacaserial(int wait) {
  start = millis();
  while ((start + wait) > millis()) {
    while (serial1.available() > 0) {
      karakter = serial1.read();
      Serial.print(karakter);
    }
  }
  serial1.flush();
  Serial.flush();
}

void sendServer() {
  on();
  result = "";
  // ATTACH GPRS SERVICE
  serial1.println(F("AT+CGATT=1"));
  Serial.flush();
  serial1.flush();
  bacaserial(200);
  delay(500);

  // CHECK GPRS SERVICE APAKAH SUDAH TERKONEKSI
  serial1.println(F("AT+CGATT?"));
  delay(500);
  while (serial1.available() > 0) {
    if (serial1.find("+CGATT: ")) {
      while (serial1.available() > 0) {
        karakter = serial1.read();
        result += karakter;
      }
    }
  }

  if (result.toInt() == 1) {
    //kirim data ke server
    serial1.println(F("AT+CIPSHUT"));
    bacaserial(500);
    Serial.flush();
    serial1.flush();
  delay(200);

    serial1.println(F("AT+SAPBR=0,1"));
    bacaserial(200);
    Serial.flush();
    serial1.flush();
  delay(200);

    //ATUR APN SESUAI DENGAN PROVIDER
    apn(operators);

    //CONNECTION TYPE
    serial1.println(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\""));
    bacaserial(200);
    Serial.flush();
    serial1.flush();
    delay(200);

    //APN ID
    result = "AT+SAPBR=3,1,\"APN\",\"" + APN + "\"";
    serial1.println(result);
    Serial.flush();
    serial1.flush();
    delay(200);

    //APN USER
    result = "AT+SAPBR=3,1,\"USER\",\"" + USER + "\"";
    serial1.println(result);
    Serial.flush();
    serial1.flush();
    delay(200);

    //APN PASSWORD
    result = "AT+SAPBR=3,1,\"PWD\",\"" + PWD + "\"";
    serial1.println(result);
    serial1.flush();
    bacaserial(200);
    Serial.flush();
    serial1.flush();
    delay(200);

    //OPEN BEARER
    i = 0;
    start = millis();
    Serial.println(F("AT+SAPBR=1,1"));
    serial1.println(F("AT+SAPBR=1,1"));
    while (millis() - start < 7500) {
      while (serial1.available() > 0) {
        if (serial1.find("OK") == true) {
          i = 1;
          break;
        }
        Serial.write(serial1.read());
      }
      if (i == 1) break;
    }
    Serial.flush();
    serial1.flush();
    delay(1000);

    //TERMINATE HTTP SERVICE
    serial1.println(F("AT+HTTPTERM"));
    bacaserial(200);
    Serial.flush();
    serial1.flush();
    delay(500);

    serial1.println(F("AT+HTTPINIT"));
    bacaserial(200);
    Serial.flush();
    serial1.flush();
    delay(500);

    //SET HTTP PARAMETERS VALUE
    serial1.println(F("AT+HTTPPARA=\"CID\",1"));
    serial1.flush();
    bacaserial(200);
    Serial.flush();
    serial1.flush();
    delay(200);

    // send data measurement to server
    //SET HTTP URL
    result = "AT+HTTPPARA=\"URL\",\"" + server + url + "\"";
    // Serial.println(result);
    serial1.println(result);
    bacaserial(200);
    Serial.flush();
    serial1.flush();
    delay(200);

    //http://www.mantisid.id/api/product/osh_data_c.php?={"Data":"003,20190315213555,2456,02356,02201,01233,05,05"}
    // {"Data":"id unit,yyyymmddhhmmss,tekanan(5 digit),kelembaban(5 digit),suhu(5 digit),volt (5 digit), burst (2 digit), interval (2 digit)"}
    kalimat = "{\"Data\":\"" + String(ID) + ",";

    kalimat += String(tahun);
    if (bulan < 10) {
      kalimat += "0" + String(bulan);
    }
    if (bulan >= 10) {
      kalimat += String(bulan);
    }
    if (hari < 10) {
      kalimat += "0" + String(hari);
    }
    if (hari >= 10) {
      kalimat += String(hari);
    }
    if (jam < 10) {
      kalimat += "0" + String(jam);
    }
    if (jam >= 10) {
      kalimat += String(jam);
    }
    if (menit < 10) {
      kalimat += "0" + String(menit);
    }
    if (menit >= 10) {
      kalimat += String(menit);
    }
    if (detik < 10) {
      kalimat += "0" + String(detik);
    }
    if (detik >= 10) {
      kalimat += String(detik);
    }

    kalimat += "," + strTwoDigit(tekanan) + ",-9999,-9999,";
    kalimat += strTwoDigit(volt) + "," + String(burst) + "," ;
    kalimat += String(interval) + "\"}";


    //SET HTTP DATA FOR SENDING TO SERVER
    result = "AT+HTTPDATA=" + String(kalimat.length() + 1) + ",15000";
    // Serial.println(result);
    serial1.println(result);
    Serial.flush();
    serial1.flush();
    while (serial1.available() > 0) {
      while (serial1.find("DOWNLOAD") == false) {
      }
    }
    bacaserial(500);

    //SEND DATA
    Serial.println(F("KIRIM DATANYA"));
    Serial.println(kalimat);
    serial1.println(kalimat);
    Serial.flush();
    serial1.flush();
    bacaserial(1000);
    Serial.flush();
    serial1.flush();

    //HTTP METHOD ACTION
    start = millis();
    Serial.println(F("AT+HTTPACTION=1"));
    serial1.println(F("AT+HTTPACTION=1"));
    Serial.flush();
    serial1.flush();
    while (serial1.available() > 0) {
      while (serial1.find("OK") == false) {
        // if (serial1.find("ERROR")) {
        // goto serve;
        // }
      }
    }
    Serial.flush();
    serial1.flush();
    indeksA = '0';
    indeksB = '0';
    result = "";
    //CHECK KODE HTTPACTION
    while ((start + 30000) > millis()) {
      while (serial1.available() > 0) {
        karakter = serial1.read();
        result += karakter;
        //Serial.print(karakter);
        indeksA = result.indexOf(":");
        indeksB = result.length();
        if (indeksB - indeksA > 8) {
          Serial.println(F("keluar yuk"));
          break;
        }
      }
      if (indeksB - indeksA > 8) {
        Serial.println(F("hayuk"));
        break;
      }
    }
    Serial.flush();
    serial1.flush();


    indeksA = '0';
    indeksB = '0';
    serial1.println(F("AT+HTTPTERM"));
    bacaserial(200);
    Serial.flush();
    serial1.flush();
  delay(200);
    indikator();

    // Serial.println(F("AT+SAPBR=0,1"));
    serial1.println(F("AT+SAPBR=0,1"));
    bacaserial(200);
    indeksA = '0';
    indeksA = result.indexOf(',');
    indeksB = result.indexOf(',', indeksA + 1);
    kode = result.substring(indeksA + 1, indeksB).toInt();

    Serial.print(F("kode="));
    Serial.println(kode);
    Serial.flush();
  }
  else {
    Serial.println(F("NETWORK ERROR"));
    Serial.flush();
  }

}

void GSMreset() { //WAKE UP GSM
  Serial.println(F(" "));
  Serial.println(F("AT+CSCLK=0"));
  serial1.println(F("AT+CSCLK=0"));
  Serial.flush();
  serial1.flush();
  delay(200);
  serial1.println(F("AT+CSCLK=0"));
  serial1.flush();
  bacaserial(200);
  Serial.flush();
  serial1.flush();
  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);
}

void GSMsleep() {
  on();
  gsm = 1;
  Serial.println(F("GSM SLEEP"));
  serial1.println(F("AT+CSCLK=2"));
  serial1.flush();
  bacaserial(200);
  off();
}
