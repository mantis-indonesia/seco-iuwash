String ID = "DEMO#4";
String hotspot;
float flon= 106.123456;
float flat=-6.54321;

#define DHTPIN            D3
#define DHTTYPE           DHT22     // DHT 11 
#define oled 0x3c
#define ads_addr 0x48
#define cs D8
#define panjang 128
#define lebar 64
#define pres 1
#define button 2
#define tegangan 0

//server
char sTime[] = "2021-07-13T15:15:55.000Z";
int httpCode;

//GLOBAL VARIABLE
int i, tahun;
char sdcard[200];
byte a, b, interval;
int burst;
byte bulan, hari, jam, menit, detik;
char str[13];
unsigned long reads = 0; //pressure
int16_t reads1 = 0; //pressure
unsigned long waktu;
float offset = 0.0;
float tekanan, temp, humid, volt;

String network, json;

//flag for saving data
bool shouldSaveConfig = false;
