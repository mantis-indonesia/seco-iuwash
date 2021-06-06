/* I-GAUGE Pro Mini 2019 5V
   Instalasi NRW
*/

//LIBRARY
#include <avr/power.h>
#include <avr/sleep.h>
#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC
#include <LowPower.h>
#include <SoftwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADS1015.h>
#include <Wire.h>
//custom library
#include "config.h"

SoftwareSerial serial1(6, 7); // RX, TX

//ADS1115
Adafruit_ADS1115 ads(ads_addr);

//GLOBAL VARIABLE
time_t t, alarmTime;
byte pembagiWaktu = 60 / interval;
const time_t ALARM_INTERVAL(interval * 60);  // alarm interval in seconds

boolean ada = 0;
int i, kode, tahun;
char karakter;
byte indeksA, indeksB, indeksC;
byte bulan, hari, jam, menit, detik;
unsigned long reads = 0; //pressure
unsigned long reads1 = 0;
unsigned long awal, akhir, start;
float tekanan, volt;
String kalimat, network, APN, USER, PWD, operators, result;

void setup() {
  //initialisation
  Serial.begin(baud);  // Serial USB
  pinMode(13, OUTPUT);
  pinMode(wakeupPin, INPUT_PULLUP);
  
  //WELCOME SCREEN
  Serial.println(F("\r\nGSM IGAUGE PRO MINI 5V 2019"));
  Serial.print(F("Unit = "));
  Serial.println(ID);

  //CEK ADS1115
  Serial.println(F("Cek ADS1115"));
  Serial.print(F("ADS1115 = "));
  Wire.begin();
  Wire.beginTransmission(ads_addr);
  indeksA = Wire.endTransmission();
  if (indeksA == 0) {
    Serial.println("ADS1115 ada");
    ada = 1;
    Serial.println(ada);
  }
  
  Serial.println(ada);
  Wire.end();

  //CEK RTC
  Serial.println(F("Cek RTC"));
  Wire.begin();
  Wire.beginTransmission(rtc_addr);
  indeksA = Wire.endTransmission();
  if (indeksA == 0) {
    Serial.println(F("RTC ada"));
  }
  if (indeksA != 0) {
    Serial.println(F("RTC Error"));
    digitalWrite(13, HIGH);
    while (1);
  }
  Wire.end();
  setSyncProvider(RTC.get); 

  //ATUR PIN YANG TIDAK DIGUNAKAN SEBAGAI OUTPUT LOW
  Serial.println(F("Atur pin digital sebagai output LOW"));
  Serial.flush();
  for (i = 3; i <= 13; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
    if (i == 5) i = 9;
  }


  // disable unused pin function
  Serial.println(F("Matikan semua fungsi peripheral"));
  Serial.flush();
  off();

  //CHECK IF PRESSURE SENSOR CONNECTED
  s_on();
  Serial.print(F("Sensor tekanan "));
  pinMode(connectPres, INPUT);
  indeksA = digitalRead(connectPres);
  if (indeksA == 1) {
    s_on();
    Serial.println(F("tidak terhubung!!!!"));
    Serial.flush();
    off();
    while (1) {
      digitalWrite(13, HIGH);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      digitalWrite(13, LOW);
      LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
    }
  }

  //nyala kan LED kalau sensor terhubung
  Serial.println(F("terhubung"));
  Serial.flush();
  s_off();
  indikator();

  s_on();
  Serial.println(F("\r\nInisialisasi serial GSM"));
  Serial.flush();
  serial1.begin(baud);  // SIM900A

  //waiting 15 second for GSM warm up
  for (indeksA = 1; indeksA <= 15; indeksA++) {
    s_on();
    Serial.print(indeksA);
    Serial.println(F(" detik"));
    Serial.flush();
    indikator();
  }
  s_on();
  Serial.println(F("Cek AT Command"));
  ceksim(); // Cek GSM modul apakah terhubung
  Serial.println(F("Registrasi SIM"));
  regsim(); // registrasi GSM modul ke network

  //atur waktu interupsi
  on();
  Serial.println(F("Atur Waktu Interupsi.\r\n I2C set di 400K")) ;
  Wire.setClock(400000) ;

  if (timeStatus() != timeSet) {
    Serial.println(F("Tidak bisa sinkronisasi dengan RTC. Atur menggunakan GSM"));
    waktu(); // ambil data waktu dari RTC network

  }
  else  {
    Serial.println(F("Waktu RTC OK"));

  }
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  GSMsleep(); //GSM modul tidur
  on();
  

  clearRTC();

  Serial.print(F("waktu = "));
  cekwaktu();
}

void loop() {
  sleepNow(); // Enter sleep
  
  if ( RTC.alarm(ALARM_1) )  {
  Serial.println("Saatnya bekerja");
  /*
  // turn on GSM & Pressure sensor
  t = RTC.get();
  tahun = year(t);
  bulan = month(t);
  hari = day(t);
  jam = hour(t);
  menit = minute(t);
  detik = second(t);
  alarmTime = t + ALARM_INTERVAL;
  Serial.println(F("Mulai Ambil Data"));
  printDateTime(t) ;

  //nyalakan GSM jika mati
  if (gsm == 1) {
    Serial.println(F("Nyalakan GSM"));
    GSMreset();
  }

  ceksim(); // Cek GSM modul apakah terhubung
  regsim(); // registrasi GSM modul ke network

  bersihdata();

  //AMBIL DATA SENSOR TEKANAN
  Serial.println(F("ambil data sensor tekanan"));
  Serial.flush();

  //burst data 5 seconds
  for (i = 0; i < burst; i++) {
    digitalWrite(13, HIGH);
    if (ada == 0)    reads1 = analogRead(pres); //pressure
    if (ada > 0) reads1 = ads.readADC_SingleEnded(0);
    if (reads1 < 0) {
      reads = reads + 0;
    }
    else {
      reads = reads + reads1;
    }
    delay(300);
    digitalWrite(13, LOW);
    delay(700);
  }

  //tegangan
  if (ada == 0)    reads1 = analogRead(tegangan);
  if (ada > 0) reads1 = ads.readADC_SingleEnded(2);

  //KONVERSI TEKANAN
  volt = (float)reads1 / 1024.00 * 5 ; // nilai voltase dari nilai DN
  tekanan = ((float)reads / (float)burst) / 1024.00 * 5; // nilai voltase dari nilai DN
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
*/
  //atur waktu tidur hingga waktu pengambilan data berikutnya
  on();
  alarmTime = t + ALARM_INTERVAL;
  // set the alarm
  Serial.print(F("Tidur hingga "));
  Serial.print(hour(alarmTime));
  Serial.print(":");
  Serial.println(minute(alarmTime));
  Serial.flush();
 // set the alarm
    RTC.setAlarm(ALM1_MATCH_HOURS, 0, minute(alarmTime), hour(alarmTime), 0);
}
}

void bersihdata() {
  reads = 0; reads1 = 0; tekanan = 0; volt = 0;
}

void sleepNow () {
  Serial.println("Entering sleep");
  delay(100);
  noInterrupts (); // Disable interrupts before entering sleep
  attachInterrupt (digitalPinToInterrupt(wakeupPin), wake, FALLING);  // Wake on falling edge of D3
  interrupts (); // Enable interrupts to ensure next instruction is executed
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}


void wake() {
detachInterrupt(digitalPinToInterrupt(wakeupPin));
}
