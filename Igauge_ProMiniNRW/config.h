//VARIABLES
char ID[5] = "0021";
#define burst 5
#define interval 2
float offset = 0.0;
String source = "GSM";
#define baud 9600

String url = "/project/osh_2019/api/product/osh_data_c.php";
String server = "http://www.mantisid.id";

#define ads_addr    0x48
#define rtc_addr    0x68
#define pres            A1 //channel
#define pres1            A2 //channel
#define tegangan        A0 //channel
#define Rled      3
#define pwk        5
#define Gled      2
#define Bled      9
#define connectPres         8

#define wakeupPin 2
boolean gsm = 0; // indikator GSM sedang tidur atau aktif
boolean GSMerror = 0; //indikator GSM error atau tidak
