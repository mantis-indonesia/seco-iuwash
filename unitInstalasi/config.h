#define debug
//VARIABLES
float Lat=0.123456;
float Long=106.123456;
const byte ID = 11; // unit ID
#define burst 3 // second
#define interval 2 // menit
float offset = 0.0;
char apn[]="internet";
byte indeksB = 2; // waktu tunggu GSM

#define pres            A1 //channel
#define led      3
#define ledError 6
#define wakeupPin 2
#define baud 9600
#define rtc_addr    0x68
const byte SSpin = 53 ;    // pin Slave Select
char NamaFile[14]="LOG1.txt";
boolean gsmTidur = 0; // indikator GSM sedang tidur atau aktif
boolean GSMerror = 0; //indikator GSM error atau tidak
int i, kode, tahun, nomor=1;
char karakter;
byte indeksA;
byte bulan, hari, jam, menit, detik;
unsigned long reads = 0; //pressure
unsigned int reads1 = 0; //voltage
unsigned long awal, akhir, start;
float tekanan, volt;
String kalimat;
String result, json;
