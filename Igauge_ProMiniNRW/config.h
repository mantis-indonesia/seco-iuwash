//s1 : tekanan, s2: level, s3: terserah, b: baterai
// nilai lain kalau tidak ada maka diberi -1

#define debug
String noHP="+6285958553254";
float batasBawah=5.6;
float batasAtas=6.0;

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
char sTime[]="2021-07-13T15:15:55.000Z";
String result, json;


//VARIABLES
//float Lat=-6.569142;
//float Long=106.786768;
float Lat=-6.561963;
float Long=106.724292;

//const byte ID = 11; // unit ID
//const char * ID = "DEP_J_GTA#01"; // unit ID
//const char * ID = "DEP_P_CLKP#01"; // unit ID
//const char * ID = "DEP_P_TD#01"; // unit ID pompa taman duta KP 1
//const char * ID = "DEP_P_TD#02"; // unit ID
//const char * ID = "DEP_B_SKTN#01"; // unit ID
//const char * ID = "DEP_J_MTD#01"; // unit ID
//const char * ID = "DEP_J_PSM#01"; // unit ID
//const char * ID = "DEP_P_RAN#01"; // unit ID
//const char * ID = "DEP_P_WFI#01"; // unit ID
//const char * ID = "DEMO#01"; // unit ID
//const char * ID = "DEMO#2"; // unit ID
//const char * ID = "DEMO#3"; // unit ID
//const char * ID = "DEMO#4"; // unit ID
//const char * ID = "DEMO#6"; // unit ID
//const char * ID = "DEMO#8"; // unit ID
//const char * ID = "DEMO#9"; // unit ID
//const char * ID = "DEMO#10"; // unit ID
const char * ID = "DEMO#13"; // unit ID
#define burst 3 // second
#define interval 2 // menit
float offset = 0.0;
String source = "GSM";
#define baud 9600

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
