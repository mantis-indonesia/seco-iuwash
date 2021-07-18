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
