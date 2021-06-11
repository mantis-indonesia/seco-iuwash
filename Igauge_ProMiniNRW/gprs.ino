boolean gprsComm() {
  //Attach from GPRS Service
#ifdef debug
  Serial.println(F("\r\n\r\n- AKTIFKAN GPRS -")); //SERVICE
#endif
  gprsAttach(3000);

  // Cek GPRS Registration
  char karakterOutput = '.';
  gprsReg(karakterOutput, 5000);
  if (karakterOutput != '1') return 0;

#ifdef debug
  Serial.println(F("\r\n- TUTUP TCP IP -"));
#endif
  gprsShut(200);

  // SET SINGLE IP CONNECTION
#ifdef debug
  Serial.println(F("\r\n\r\n- ATUR CIPMUX -")); //MENJADI SINGLE IP CONNECTION
#endif
  gprsMux(200);

  //CEK IP STATUS IN INITIAL MODE
#ifdef debug
  Serial.println(F("\r\n\r\n- CEK IP STATUS-"));
#endif
  gprsStatus(200);

  //CONNECTION TYPE = gprs ; ATUR APN, USER, DAN PASSWORD
#ifdef debug
  Serial.println(F("\r\n\r\n- SET APN -")); //, USER, DAN PASSWORD
#endif
  gprsCSTT(200);

  //CEK IP STATUS IN INITIAL MODE
#ifdef debug
  Serial.print(F("\r\n\r\n- CEK IP STATUS-\r\n"));
#endif
  gprsStatus(200);

  //Bring Up Wireless Connection with GPRS
#ifdef debug
  Serial.println(F("\r\n\r\n- BUKA TCP IP -"));
#endif
  gprsWirelessConnect(karakterOutput, 85000);
  if ( karakterOutput == 'E') {
    Serial.println(F("- ERROR -"));
    return 0;
  }
  Serial.println(F("- CONNECTED -"));

  //GPRS IP
  Serial.println(F("\r\n- IP ADDRESS -"));
  gprsIP(200);

  return 1;
}

char cekSerial(unsigned long times) {
  boolean indeks = 0;
  unsigned long mulai = millis();
  while (millis() - mulai < times && indeks == 0) {
    while (gsm.available() > 0) {
      karakter = gsm.read();

#ifdef debug
      if (isAscii(karakter)) Serial.write(karakter);
#endif

      if (karakter == 'O' || karakter == 'E') {
        indeks = 1;
        break;
      } // exit from if
    } // exit from gsm available
  } // exit from millis()

  readSerial(10);
  return karakter;
}

void gprsAttach(unsigned int waktu) {
  // ATTACH gprs SERVICE
  gsm.println(F("AT+CGATT=1"));
  readSerial(waktu);
}

void gprsOff(unsigned int waktu) {
  // ATTACH gprs SERVICE
  gsm.println(F("AT+CGATT=0"));
  readSerial(waktu);
}

void gprsReg(char &Karakter, unsigned int jeda) {
  // ATTACH gprs SERVICE

  unsigned long waktu = millis ();
  while (millis () - waktu <= jeda) {
    karakter = '+';
    gsm.println(F("AT+CGREG?"));
    unsigned long waktuAwal = millis();
    while (millis() - waktuAwal <= 1000 && karakter != 'K') {
      karakter = gsm.read();
      switch (karakter) {
        case '0' ... '9':
          Karakter = karakter;
#ifdef debug
          if(isAscii(Karakter)) Serial.print(Karakter);
#endif
          break;

        default:
#ifdef debug
          if (isAscii(karakter)) Serial.print(karakter);
#endif
          break;
      }
    }

#ifdef debug
    Serial.print(F("\r\nCGREG="));
    Serial.println(Karakter);
#endif

    if (Karakter != '1') {
      gsm.println(F("AT+CGREG=1"));
      readSerial(200);
    }
    else break;
  }

}

void gprsShut(unsigned int waktu) {
  //DEAKTIVASI gprsPDP CONTEXT DAN TUTUP BEARER
  gsm.println(F("AT+CIPSHUT"));
  readSerial(waktu);
}

void gprsMux(int waktu) {
  // ATTACH gprs SERVICE
  gsm.println(F("AT+CIPMUX=0"));
  readSerial(waktu);
}

void gprsStatus(unsigned int waktu) {
  // ATTACH gprs SERVICE
  byte i = 0;
  unsigned long mulai = millis();
  gsm.println(F("AT+CIPSTATUS"));
  while (millis() - mulai <= waktu && i < 4) {
    while (gsm.available() > 0) {
      karakter = gsm.read();
      Serial.write(karakter);
      if (karakter == '\n') i = i + 1;
    }
  }
  readSerial(20);
}

void gprsCSTT(int waktu) {
  //APN
  gsm.print(F("AT+CSTT=\""));
  gsm.print(apn);

  //USER - ISI DI ANTARA \" DAN \"
  gsm.println(F("\",\"\",\"\""));

  //PASSWORD - ISI DI ANTARA \" DAN \"
//  gsm.println(F(""));
  readSerial(waktu);
}

char gprsWirelessConnect(char &karakter, unsigned long waktu) {
  gsm.println(F("AT+CIICR"));
  char g = cekSerial(waktu);

  // debug
    #ifdef debug
    Serial.print(F("\r\nKarakter dari CIICR : "));
    Serial.print(g);
    if (g == 'O')  Serial.println(F("K"));
    else {
    Serial.println(F("RROR"));
    }
    #endif
    
  //PARSING OPEN BEARER UNTUK CEK OK ATAU ERROR
  return g;
}

void gprsIP(int waktu) {
  gsm.println(F("AT+CIFSR"));
  readSerial(waktu);
}
