/* I-GAUGE Pro Mini 3v3
  Modifikasi Instalasi kedua Kegiatan IUWASH 2019
*/

//LIBRARY
#include <SoftwareSerial.h>
SoftwareSerial gsm(6, 7); // RX, TX

#include <TimeLib.h>
#include <TimeAlarms.h>

AlarmId id;

//custom library
#include "config.h"


void setup() {
  //initialisation
  inisialisasi();

  //pinMode
  Serial.println(F("Atur pinMode"));
  for (indeksA = 2; indeksA < 14; indeksA++) {
    if (indeksA == 6) indeksA = 9;
    pinMode(indeksA, OUTPUT);
    digitalWrite(indeksA, LOW);
  }
  pinMode(connectPres, INPUT);
  digitalWrite(connectPres, HIGH);

  // disable unused pin function
  Serial.println(F("Matikan semua fungsi"));
  Serial.flush();

  //CHECK IF PRESSURE SENSOR CONNECTED
  indeksA = digitalRead(connectPres);
  Serial.print(F("Sensor tekanan = "));
  Serial.println(indeksA);
  Serial.flush();
  /*
    if (indeksA == 1) {
    s_on();
    Serial.println(F("Sensor tekanan tidak terhubung!!!!"));
    Serial.flush();
    s_off();
    digitalWrite(Rled, HIGH);
    digitalWrite(13, HIGH);
    while (1) {}
    }
  */
  //nyala kan LED kalau sensor terhubung
  Serial.println(F("Sensor tekanan terhubung"));
  Serial.flush();
  indikator(Bled);

  Serial.println(F("Inisialisasi serial GSM"));
  Serial.flush();
  indikator(Bled);


  //waiting 15 second for GSM warm up
  for (indeksA = 1; indeksA <= indeksB; indeksA++) {
    Serial.print(indeksA);
    Serial.println(F(" detik"));
    Serial.flush();
    indikator(Gled);
  }

  Serial.println(F("Cek AT Command"));
  ceksim(); // Cek GSM modul apakah terhubung
  indikator(Gled);
  //Pilih Band
  Serial.println(F("Pilih Band"));
  gsm.println(F("AT+CBAND=\"ALL_BAND\""));
  readSerial(15000);

  Serial.println(F("Registrasi SIM"));
  regsim(); // registrasi GSM modul ke network
  indikator(Gled);

  waktu();
  kerja();
  Alarm.timerRepeat(interval * 60, kerja);           // timer for every 15 seconds
}

void bersihdata() {
  reads = 0; reads1 = 0; tekanan = 0; volt = 0;
}

void loop() {
  Alarm.delay(0);
}

void kerja() {
  Serial.println(F("\r\nMulai Ambil Data"));
  Serial.flush();
  bersihdata();

  waktu(); //cek waktu dan tanggal dari network


  //AMBIL DATA SENSOR TEKANAN
  Serial.println(F("\r\nMulai Ambil Data"));
  Serial.flush();
  ambilData();

  if (GSMerror == 0) {
    sendServer();
  }

  if (tekanan > batasAtas || tekanan < batasBawah) kirimSMS();
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

void ambilData() {
  //nilai ADC tekanan selama 5 detik
  for (indeksA = 0; indeksA < burst; indeksA++) {
    digitalWrite(Gled, HIGH);
    reads1 = analogRead(pres); //pressure
    if (reads1 < 0) {
      reads = reads + 0;
    }
    else {
      reads = reads + reads1;
    }
    Alarm.delay(300);
    digitalWrite(Gled, LOW);
    Alarm.delay(700);
  }
  //KONVERSI ADC KE TEKANAN
  tekanan = ((float)reads / (float)burst) / 1024.00 * 3.3 * 14700 / 10000; // nilai voltase dari nilai DN | nilai 1.5152 itu konversi sistem 3.3v menuju 5v
  Serial.print("nilai voltase tekanan asli = ");
  Serial.println(tekanan);
  tekanan = (3.00 * tekanan - 1.50) + float(offset);
  Serial.print("nilai tekanan ditambahkan offset = ");
  Serial.println(tekanan);
  if (tekanan < 0) tekanan = 0;

  //nilai ADC tegangan
  reads1 = analogRead(tegangan);
  volt = (float)reads1 / 1024.00 * 3.3 * 14700 / 10000; // nilai voltase dari nilai DN * 147.00 / 100.00


  //debug value
  Serial.print(F("TEKANAN = "));
  Serial.println(tekanan, 2);
  Serial.print(F("VOLTASE = "));
  Serial.println(volt, 2);
  Serial.flush();
}
