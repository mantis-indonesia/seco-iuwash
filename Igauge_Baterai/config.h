#define debug
String noHP="+6285958553254";
float batasBawah=5.6;
float batasAtas=6.0;

//VARIABLES
float Lat=0.123456;
float Long=106.123456;

const char * ID = "DEMO#6"; // unit ID
byte burst =3 ;// second
unsigned long interval = 120000; // menit * DETIK * MILIDETIK
float offset = 0.0;
String source = "GSM";
#define baud 9600

int i, kode, tahun;
char karakter;
byte indeksA;
byte indeksB=5;
byte bulan, hari, jam, menit, detik;
unsigned long reads = 0; //pressure
unsigned int reads1 = 0; //voltage
unsigned long awal, akhir,start;
float tekanan, volt;
String kalimat;
char apn[]="internet";
char sTime[]="2021-07-13T15:15:55.000Z";
String result, json;

#define pres            A0
#define tegangan        A1
#define Rled      2
#define Gled      3
#define Bled      4
#define pwk       5
#define connectPres     8

boolean gsmTidur = 0; // indikator GSM sedang tidur atau aktif
boolean GSMerror = 0; //indikator GSM error atau tidak
