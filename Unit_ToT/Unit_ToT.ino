/* I-GAUGE Unit ToT 2021
    Kegiatan IUWASH - SECO
    BEKERJA SAMA DENGAN PDAM DEPOK

*/

//LIBRARY
#include <avr/power.h>
#include <avr/sleep.h>
#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC
#include <LowPower.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>

//custom library
#include "config.h"

#define gsm Serial1
//parameter waktu
time_t Waktu, alarmTime;
byte pembagiWaktu = 60 / interval;
const time_t ALARM_INTERVAL(interval * 60);  // alarm interval in seconds


void setup() {
  //initialisation
  Serial.begin(baud);
  gsm.begin(baud);  // SIM900A
  pinMode(13, OUTPUT);
  pinMode(wakeupPin, INPUT_PULLUP); // Set interrupt pin

  //WELCOME SCREEN
  Serial.println(F("\r\nGSM IGAUGE unit Training 5V 2021"));
  Serial.print(F("Unit = "));
  Serial.println(ID);
  Serial.flush();

  //CEK RTC
  cekRTC();
  indikator();

  //cek micro SD Card
  sdInit();
  indikator();
  cekfile();
  indikator();

  //waiting 15 second for GSM warm up
  for (indeksA = 1; indeksA <= indeksB; indeksA++) {
    Serial.print(indeksA);
    Serial.println(F(" detik"));
    Serial.flush();
    indikator();
  }

  Serial.println(F("Cek AT Command"));
  ceksim(); // Cek GSM modul apakah terhubung
  Serial.println(F("Registrasi SIM"));
  regsim(); // registrasi GSM modul ke network

  //atur waktu interupsi
  Serial.println(F("Atur Waktu Interupsi.\r\n I2C set di 400K")) ;
  Wire.setClock(400000) ;
  waktu(); // ambil data waktu dari RTC network

  //setSyncProvider(RTC.get);   // the function to get the time from the RTC
  GSMsleep(); //GSM modul tidur

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
    digitalWrite(led, HIGH);
    reads1 = analogRead(pres); //pressure
    reads = reads + reads1;
    delay(300);
    digitalWrite(led, LOW);
    delay(700);
  }

  
  //KONVERSI TEKANAN

  Serial.print(F("TEGANGAN SENSOR = "));
  Serial.println(tekanan);
  tekanan = (3.00 * tekanan - 1.5) + float(offset);
  if (tekanan < 0) tekanan = 0;

  //debug value
  Serial.print(F("TEKANAN = "));
  Serial.println(tekanan, 2);
  Serial.flush();

  if (GSMerror == 0) {
    sendServer();
  }
  else kode=999;

  simpanData();

  //TURN OFF GSM MODULE
  GSMsleep();

  //atur waktu tidur hingga waktu pengambilan data berikutnya
  
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

void indikator() {
  pinMode(led, OUTPUT);
  for (byte ind = 0; ind < 2; ind++) {
    digitalWrite(led, HIGH);
    LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
    digitalWrite(led, LOW);
    LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
  }
}
