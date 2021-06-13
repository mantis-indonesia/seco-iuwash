#define debug

//boolean ada = 0;
int i, kode, tahun;
char karakter;
byte indeksA, indeksB;
byte bulan, hari, jam, menit, detik;
unsigned long reads = 0; //pressure
unsigned int reads1 = 0; //voltage
unsigned long awal, akhir, start;
float tekanan, volt;
String kalimat;
char apn[]="internet";
String result, json;


//VARIABLES
float Lat=0.123456;
float Long=106.123456;

const byte ID = 11; // unit ID
#define burst 3 // second
#define interval 2 // menit
float offset = 0.0;
String source = "GSM";
#define baud 115200

#define rtc_addr    0x68
#define pres            A1 //channel
#define pres1            A2 //channel
#define tegangan        A0 //channel
#define pwk        5
#define Bled      9
#define connectPres         8

#define wakeupPin 2
boolean gsmTidur = 0; // indikator GSM sedang tidur atau aktif
boolean GSMerror = 0; //indikator GSM error atau tidak
