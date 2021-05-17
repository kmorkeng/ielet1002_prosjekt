//===============================Biblioteker=======================================
#include <Adafruit_LiquidCrystal.h>//Bibliotek for LCD-skjerm
#include <Wire.h>//Bibliotek for I2C-kommunikasjon
#include<CircusESP32Lib.h>//Bibliotek for oppkobling til CoT
//=================================================================================


//-----------------------------LCD-Skjerm------------------------------------------
Adafruit_LiquidCrystal lcd(0x70);
//---------------------------------------------------------------------------------


//==========================Sleep-funksjonalitet===================================

#define wakeupPin 0x300000000  // Pinner 32 og 33

int autoSleep = 120000;//Bestemmer tid til automatisk aktivering av deep sleep
int deepSleepHoldTime = 2500;//Bestemmer tid til start av deep sleep om begge knapper er påtrykt

int pending;//Hindrer program å kjøre når begge knapper er nedtrykt
unsigned long resetMillis;//Lagring av tid for automatisk deep sleep
unsigned long reset;//Lagring av tid for manuell deep sleep
//=================================================================================


//---------------------------------------------------------------------------------
int regUsers;//Lagring av innhentet brukerdata fra CoT
int space1;//Lagring av første bruker i rom
int space2;//Lagring av andre bruker i rom
int space3;//Lagring av tredje bruker i rom
int roomUser;//Lagrer rom og bruker for fjerning
//--------------------------diverse variabler--------------------------------------

int repeatRun;//Hindrer program i å kjøre flere ganger

int guestNumber;//Lagrer innskrevet gjestenummer

int numLen;//Lagrer lengde på innskrevet gjestenummer


float currentPrice;//Strømpris i sanntid
float currentProd;//Strømproduksjon i sanntid

int yes;//Sender gjestenummer til RPi his ja, tilbake til hovedmen hvis nei

int CoTcheck;//Venter til CoT klargjør verdier


int roomSelect;//Lagring av valgt rom

int digit1;//Generell nummervariabel

//---------------------------------------------------------------------------------








//--------------------------------CoT----------------------------------------------
char ssid[] = "mina sin iPhone";
char password[] = "heihei123";
char server[] = "www.circusofthings.com";


char energyDataKey[] = "12439";//Key til strømpris
char productionKey[] = "9545";//Key til energiproduksjon

char generalKey[] = "1794";//Key til div. kommunikasjon mellom ESP32 og RPi
char spaceKey[] = "436";//Key for lagring av brukere fra CoT


char token1[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTU5In0.gf8JypyMMsWYBRo5GM58ORIeEk66AAVNzLRcD1RCGJA";
char token2[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI1MTI2In0.DZ9fF-FV8PeyFZJ8aJJ7hjOIENv6L1z471tRgcHCPyo";


CircusESP32Lib circusESP32(server, ssid, password);//Klargjør oppkobling til CoT
//---------------------------------------------------------------------------------


//============================Tilstandsmaskin oppsett==============================
enum adminSeq {
  adminStandby,//Klargjør setting
  setting,//Velger funksjonalitet
  energyData,//Leser av energidata fra RPi
  registerGuest,//Registrerer gjestenummer
  adminRoom,//Velger rom
  chooseMode,//Velger mellom info og Fjerning av bruker
  getValue,//Henter bookingverdier til valgt rom
  cancel//Viser bookingverdier til valgt rom, med mulighet for
};

adminSeq admin = adminStandby;//Starter program i adminStandby
//=================================================================================


//---------------------------------Knapper-----------------------------------------

const int button1 = 16;//Pin til knapp 1
const int button2 = 4;//Pin til knapp 2


//Knappeoppkobling (pull-up)
const int buttonOn = HIGH;
const int buttonOff = LOW;

//Registrering av knappetrykk
int numButton = 0;//Knapp 1
int selectButton = 0;//Knapp 2
//---------------------------------------------------------------------------------

void setup() {

  esp_sleep_enable_ext1_wakeup(wakeupPin, ESP_EXT1_WAKEUP_ANY_HIGH);//Klargjør ESP32 for oppvåkning med knapper etter sleep

  pinMode(button1, INPUT);
  pinMode(button2, INPUT);

  //--------------------------Begynner prosesser-----------------------------------
  lcd.begin(16, 2);//Klargjør LCD-skjerm
  Serial.begin(115200);
  lcd.setCursor(0, 0);
  lcd.print("Starter");
  circusESP32.begin();//Starter oppkobling mot CoT



  delay(1000);

  //-------------------------------------------------------------------------------

  lcd.clear();
  
  resetMillis = millis() + autoSleep;//Hindrer at ESP32 inngår deep sleep ved oppstart

}

void loop() {
  deepSleep();//Kjører deep sleep om begge knapper er nedtrykt i en bestemt periode


  if (pending == 1);//Hindrer at andre programmer kjører når begge knapper er nedtrykt

  else {
    adminControl();//Kjører funksjon for admin-kontroll
  }
  delay(50);
}
