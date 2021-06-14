/* I-GAUGE Unit Instalasi 2021
    Kegiatan IUWASH - SECO
    BEKERJA SAMA DENGAN PDAM DEPOK

*/

//LIBRARY
#include <avr/power.h>
#include <avr/sleep.h>
#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC
#include <LowPower.h>
#include <Wire.h>
//custom library
#include "config.h"

#define promini 1

#ifdef promini
#include <SoftwareSerial.h>
SoftwareSerial gsm(6, 7); // RX, TX
#else
#define gsm Serial1
#endif

//parameter waktu
time_t Waktu, alarmTime;
byte pembagiWaktu = 60 / interval;
const time_t ALARM_INTERVAL(interval * 60);  // alarm interval in seconds


void setup() {
  //initialisation
  inisialisasi();

  //WELCOME SCREEN
  Serial.println(F("\r\nGSM IGAUGE PRO MINI 5V 2021"));
  Serial.print(F("Unit = "));
  Serial.println(ID);

  //CEK RTC
  cekRTC();

  // disable unused pin function
  Serial.println(F("Matikan semua fungsi peripheral"));
  Serial.flush();
  off();

  //CHECK IF PRESSURE SENSOR CONNECTED
  s_on();
  Serial.print(F("Sensor tekanan "));
  /*
  pinMode(connectPres, INPUT);
  indeksA = digitalRead(connectPres);
  if (indeksA == 1) {
    s_on();
    Serial.println(F("tidak terhubung!!!!"));
    Serial.flush();
    off();
    while (1) {
      digitalWrite(Bled, HIGH);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      digitalWrite(Bled, LOW);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }
  }
*/
  //nyala kan LED kalau sensor terhubung
  Serial.println(F("terhubung"));
  Serial.flush();
  s_off();
  indikator();

  s_on();
  Serial.println(F("\r\nInisialisasi serial GSM"));
  Serial.flush();


  //waiting 15 second for GSM warm up
  for (indeksA = 1; indeksA <= 2; indeksA++) {
    s_on();
    Serial.print(indeksA);
    Serial.println(F(" detik"));
    Serial.flush();
    indikator();
  }
  s_on();
  Serial.println(F("Cek AT Command"));
  ceksim(); // Cek GSM modul apakah terhubung
  //Pilih Band
  Serial.println(F("Pilih Band"));
  gsm.println(F("AT+CBAND=\"ALL_BAND\""));
  readSerial(15000);

  Serial.println(F("Registrasi SIM"));
  regsim(); // registrasi GSM modul ke network

  //atur waktu interupsi
  on();
  Serial.println(F("Atur Waktu Interupsi.\r\n I2C set di 400K")) ;
  Wire.setClock(400000) ;
  waktu(); // ambil data waktu dari RTC network
  /*
    if (timeStatus() != timeSet) {
      Serial.println(F("Tidak bisa sinkronisasi dengan RTC. Atur menggunakan GSM"));
      waktu(); // ambil data waktu dari RTC network

    }
    else  {
      Serial.println(F("Waktu RTC OK"));

    }
  */

  //setSyncProvider(RTC.get);   // the function to get the time from the RTC
  Serial.println("gsm tidur");
  GSMsleep(); //GSM modul tidur
  on();

  clearRTC();
  Serial.print(F("waktu = "));
  cekwaktu();
  kerja();
}

void loop() {
  sleepNow(); // Enter sleep
  
  if ( RTC.alarm(ALARM_1) )  {
    kerja();
  }
  
}

void kerja() {
  Serial.println("Saatnya bekerja");

  // turn on GSM & Pressure sensor
  ambilWaktu();
  Serial.println(F("Mulai Ambil Data"));
  printDateTime(Waktu) ;

  //nyalakan GSM jika mati
  if (gsm == 1) {
    Serial.println(F("Nyalakan GSM"));
    GSMreset();
  }

  //  ceksim(); // Cek GSM modul apakah terhubung
  regsim(); // registrasi GSM modul ke network
  gprsComm();

  bersihdata();

  //AMBIL DATA SENSOR TEKANAN
  Serial.println(F("ambil data sensor tekanan"));
  Serial.flush();

  //burst data 5 seconds
  for (i = 0; i < burst; i++) {
    digitalWrite(Bled, HIGH);
    reads1 = analogRead(pres); //pressure
    reads = reads + reads1;
    delay(300);
    digitalWrite(Bled, LOW);
    delay(700);
  }

  //tegangan
  reads1 = analogRead(tegangan);

  //KONVERSI TEKANAN
  tekanan = ((float)reads / (float)burst) / 1024.00 * 5.0; // nilai voltase dari nilai DN
  volt = (float)reads1 / 1024.00 * 5.0 ; // nilai voltase dari nilai DN

  Serial.print(F("TEGANGAN SENSOR = "));
  Serial.println(tekanan);
  tekanan = (3.00 * tekanan - 1.5) + float(offset);
  if (tekanan < 0) tekanan = 0;

  //debug value
  Serial.print(F("TEKANAN = "));
  Serial.println(tekanan, 2);
  Serial.print(F("VOLTASE = "));
  Serial.println(volt, 2);
  Serial.flush();

  if (GSMerror == 0) {
    sendServer();
  }

  //TURN OFF GSM MODULE
  GSMsleep();

  //atur waktu tidur hingga waktu pengambilan data berikutnya
  on();
  alarmTime = Waktu + ALARM_INTERVAL;
  // set the alarm
  Serial.print(F("Tidur hingga "));
  Serial.print(hour(alarmTime));
  Serial.print(":");
  Serial.println(minute(alarmTime));
  Serial.flush();
  // set the alarm
  RTC.setAlarm(ALM1_MATCH_HOURS, 0, minute(alarmTime), hour(alarmTime), 0);
  delay(100);
}

void bersihdata() {
  reads = 0; reads1 = 0; tekanan = 0; volt = 0;
}

void sleepNow () {
  Serial.println("Entering sleep");
  delay(100);
  noInterrupts (); // Disable interrupts before entering sleep
  attachInterrupt (digitalPinToInterrupt(wakeupPin), wake, LOW);  // Wake on falling edge of D2
  interrupts (); // Enable interrupts to ensure next instruction is executed
  delay(100);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}


void wake() {
  detachInterrupt(digitalPinToInterrupt(wakeupPin));
}
