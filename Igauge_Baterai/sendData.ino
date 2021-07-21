boolean sendServer() {
  //HTTP initilization
  Serial.println(F("\r\n- MULAI TCP IP -"));
  boolean nilai = TCPstart(5000, 3);
  if (nilai == 0) return 0;

  //SET HTTP PARAMETERS VALUE
  Serial.println(F("\r\n- KIRIM DATA -"));

  nilai = TCPsend();
  if (nilai == 0) Serial.println("TCP SEND ERROR");

  //HTTP TERMINATE
  Serial.println(F("\r\n- TUTUP TCP IP -"));
  TCPclose(200);
  gprsShut(300);

  while (Serial.available()) Serial.read();

  if (kode != 200) return 0;
  else return 1;
}


// LOW CODE FOR TCP IP
// --------------------------------------------------
// FUNCTION FOR TCP IP
boolean TCPstart(unsigned long jeda, byte ulangan) {
  char g;
  boolean hasilTCP = 0;
  for (byte angka = 1; angka <= ulangan; angka++) {
    Serial.print(F(" - TCP ke - "));
    Serial.println(angka);
    gsm.print(F("AT+CIPSTART=\"TCP\",\""));

    //ALAMAT HOST SERVER
    gsm.print(F("54.255.55.184"));
    gsm.println(F("\",\"4000\""));
    readSerial(200); // harus ada untuk menunggu CONNECT OK

    unsigned long waktu = millis();
    g = '\0';
    while (millis() - waktu < jeda) {
      g = gsm.read();
      if (isAscii(g))  Serial.print(g);
      if (g == 'S' || g == 'C' || g == 'A' || g == 'E') break;
    }

    readSerial(15);
    Serial.println(g);
    if (g == 'C' || g == 'A') {
      hasilTCP = 1;
      break;
    }
    if (g == 'E' || g == 'S') {
      hasilTCP = 0;
    }
  }
#ifdef debug
  Serial.print(F("\r\nHasil TCP Start="));
  Serial.println(hasilTCP);
#endif

  if (hasilTCP == 0) readSerial(2000);
  return hasilTCP;
}

boolean TCPsend() {
  //siapkan variabel kode HTTP
  kode = 0;
  char karakter = '.';

  gsm.println(F("AT+CIPSEND"));
  //  gsm.println(output.length()+output1.length() - 1);
  unsigned long waktu = millis();
  while (millis() - waktu < 500 && karakter != '>') {
    while (gsm.available() > 0 && karakter != '>') {
      karakter = gsm.read();
      Serial.write(karakter);
    }
  }
  if (karakter != '>') return 0;

  json = "";
  delay(10);

  json = "\"s1\": " + String (tekanan, 2)+ ",\r\n\"s2\": 0,\r\n\"s3\": 0,\r\n";
//  json = "\"s1\": 5,\r\n\"s2\": 0,\r\n\"s3\": 0,\r\n";// uji coba
  json.concat("\"b\": ");
  json.concat(String(volt, 2));
  json.concat(",\r\n\"signature\": \"\",\r\n");
  json.concat("\"device_code\": \"");
  json.concat(String(ID));
  json.concat("\",\r\n");
  json.concat("\"sTime\": \"");
  json.concat(String(sTime));
  json.concat("\",\r\n");
  json.concat("\"lat\": \"");
  json.concat(String(Lat, 6));
  json.concat("\",\r\n");
  json.concat("\"lon\": \"");
  json.concat(String(Long, 6));
  
  delay(100);
#ifdef debug
  Serial.print("Data JSON=");
  Serial.println(json);
  Serial.println(" ");
  Serial.flush();
#endif
  delay(100);
  
  gsm.print("POST /graphql HTTP/1.1\r\n");
  gsm.print("Host: 54.255.55.184:4000\r\n");
  gsm.print("Content-Type: application/json\r\n");
  gsm.print("Content-Length: ");
  gsm.print(119 + json.length() + 105);
  gsm.print("\r\n\r\n");
  gsm.print( "{\r\n\"operationName\": \"insert_data\",\r\n\"variables\": {\r\n\"data\": {\r\n\"email\": \"admin@gmail.com\",\r\n\"password\": \"admin!2357\",\r\n");
  gsm.print(json);
  gsm.print("\"\r\n}\r\n},\r\n");
  gsm.print("\"query\": \"mutation insert_data($data: DataCreateInput!) {insert_one_data(data: $data) {id}}\"\r\n}");
  gsm.flush();
  //  readSerial(150);
  readSerial(1000);

  //  readSerial(5000);

  char character = '\0';
  unsigned long waktuAwal = millis();
  // SEND OK
  waktuAwal = millis();
  boolean nilai = 0;
  Serial.write(13);
  Serial.write(10);
  gsm.write(26);
  while (millis() - waktuAwal < 15000 && nilai == 0) {
    nilai = gsm.find("SEND OK");
  }
  //  if (nilai == 0) return 0;


  Serial.println(F("\r\n- TUNGGU KODE HTTP -"));
  //HTTP 1.1
  waktuAwal = millis();
  while (millis() - waktuAwal < 10000 && character != ' ') { // && gsm.available()>0
    character = gsm.read();
#ifdef debug
    if (isAscii(character)) Serial.write(character);
#endif
  }

  if (character != ' ') return 0;

  //HTTP RESPONSE
  character == '0';

  waktuAwal = millis();
  while (millis() - waktuAwal < 10000 && character != 'O') {
    character = gsm.read();

    switch (character) {
      case '0' ... '9':
#ifdef debug
        Serial.print(character);
#endif

        kode *= 10;
        kode += character - '0';
        break;
    }

  } // while serial available
  if (character != 'O') return 0;

  Serial.print(" O");
  readSerial(500);


#ifdef debug
  Serial.print(F("\r\n\r\nKODE="));
  Serial.println(kode);
#endif

  return 1;

}

void TCPclose(unsigned int waktu) {
  gsm.println(F("AT+CIPCLOSE"));
  readSerial(waktu);
}
