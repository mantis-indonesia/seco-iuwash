void sdInit() {
  pinMode(SSpin, OUTPUT);
  digitalWrite(SSpin, HIGH);
  delay(10);

  if (!SD.begin(SSpin)) { //SD ERROR
    Serial.println(F("> MICRO SD CARD TIDAK TERDETEKSI!!!!"));
    digitalWrite(ledError, 1);
    while (1);
  }
  Serial.println(F("> MICRO SD CARD TERDETEKSI!"));
  Serial.flush();
}


void cekfile() {
  //LOCAL VARIABLE
  byte indeks = 1;
  nomor = 0;

  while (indeks) {
    nomor = nomor + 1;
    sprintf(NamaFile, "LOG%03d.txt", nomor);
    indeks = SD.exists(NamaFile);
    if (indeks == 0) {
      Serial.print(NamaFile);
      Serial.println(F(" tidak ada"));
      File file = SD.open(NamaFile, FILE_WRITE);
      file.println(F("Tanggal(YYYY/MM/DD) , Waktu(HH:MM:SS) , Tekanan (Bar) , Kode HTTP"));
      file.flush();
      file.close();
    }
  }
  Serial.flush();
}

void simpanData() {
  //  char kata[13];
#ifdef debug
  Serial.print(F("Nomor File "));
  Serial.println(nomor);
#endif

  SdFile::dateTimeCallback(dateTime);
  
  Serial.print(F("Nama File = "));
  Serial.println(NamaFile);
  
  File file = SD.open(NamaFile, FILE_WRITE);
  file.print(tahun);
  file.print("/");
  file.print(bulan);
  file.print("/");
  file.print(hari);
  file.print(" ");
  file.print(jam);
  file.print(":");
  file.print(menit);
  file.print(":");
  file.print(detik);
  file.print(",");
  file.print(tekanan);
  file.print(",");
  file.println(kode);
  file.close();
  Serial.println("- SIMPAN DATA SELESAI -");
}

void dateTime(uint16_t* date, uint16_t* time) {
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(tahun, bulan, hari);

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(jam, menit, detik);
}
