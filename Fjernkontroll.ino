// Inkluderer relevante biblioteker
#include <analogWrite.h>
#include <SevSeg.h>
#include <CircusESP32Lib.h>
SevSeg sevseg1;
SevSeg sevseg2;

// Tilstander for switch case
enum seq{veivalg, user, room, hour1, hour2, minu1, minu2, dur1, dur2, conc, enhetsvalg, varmekabler, vifte, vindu, termostat, temp1, temp2, standby};
seq seqIs = standby;

// Lagring av verdier som vil bli sendt til CoT

// Lagring 
int saveUser = 0;
int saveRoom = 0;
int saveHour1 = 0;
int saveHour2 = 0;
int saveMinu1 = 0;
int saveMinu2 = 0;
int saveDur1 = 0;
int saveDur2 = 0;
int uselessButtonSignal = 1;
int response = 0;
int check = 0;
int target = 0;

int saveTemp1 = 0;
int saveTemp2 = 0;
int lastTemp = 0;
int currentTemp = 0;
int lastTemp1 = 0;
int currentTemp1 = 0;

int gulvStatus = 0;
int vifteStatus = 0;
int vinduStatus = 0;
int varmeStatus = 0;

int currentTempG = 0;
int currentTempV = 0;
int savevarmekabler = 0;
int saveGulv1 = 0;
int saveGulv2 = 0;
int savetermostat = 0;
int saveVarme1 = 0;
int saveVarme2 = 0;
int prevGulv = 0;
int prevVarme = 0;
int currVarme = 0;
int currGulv = 0;
int hei = 0;
int heihei = 0;
int first = 0;

// Variabler for klargjøring av verdier
String outHourVar = "";
String outMinuteVar = "";
String outTimeVar = "";
int outMinutes = 0;

// Trykknapper: button1 = endre; button2 = bekreft
const int button1 = 2;
int buttonState1 = 0;
const int button2 = 34;
int buttonState2 = 0;

// LED
const int green = 23;
const int red = 13;

// Variabler for 7segment
int digit1 = 0;
int digit2 = 0;
int digit12 = 0;
const int inc = 1;

// Strings
String txtStart = "Programmet starter";
String txtBruker = "Bruker: ";
String txtRom = "Rom: ";
String txtTime = "Time: ";
String txtMinutt = "Minutt: ";
String txtVarighet = "Varighet: ";
String txtH = " timer";
String txtH1 = " time";
String txtStarttid = "Starttid: ";
String txtTakk = "Takk for at du booker!";
String txtConn = "****ESTABLISHING CONNECTION****";
String txtWait = "Awaiting response";
String txtBooking = "Bookinginformasjon";

// Definerer keys for kommunikasjon med CoT | For booking
char varighetKey[] = "23226";
char timeKey[] = "30201";
char minuttKey[] = "982";
char brukerKey[] = "25048";
char romKey[] = "27564";
char bekreftKey[] = "16455";

char uselessButtonKey[] = "31470";

// Definerer keys for kommunikasjon med CoT | For styring
char gulvKey[] = "18600";
char vifteKey[] = "6990";
char vinduKey[] = "3345";
char varmeKey[] = "19380";

// Etablerer kontakt med CoT
char ssid[] = "Shady nettverk";
char password[] = "heiheihei";
char server[] = "www.circusofthings.com";
char key[] = "7819";
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTU5In0.gf8JypyMMsWYBRo5GM58ORIeEk66AAVNzLRcD1RCGJA";
char token1[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTczIn0.rf05c4f__YEi5lmmYKuXML9WKeZCjR-jNbCns2MUIQ8"; //kristin sin CoT
CircusESP32Lib circusESP32(server,ssid,password);

// Definerer variabler for buzzeren
const int freqb4 = 494;
const int freqd5 = 587;
const int freqg5 = 784;
const int channel = 0;
const int resolution = 8;

// Bitmaske for pin 2 og 34 (knapp1 og knapp2) for å vekke ESP32'en fra sovemodus
#define BUTTON_PIN_BITMASK 0x400000004

void setup() {
  // Pinmode for LED og knapper
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  
  // Begynner kommunikasjon med seriemonitor og CoT
  Serial.begin(115200);
  circusESP32.begin();

  // Buzzer setup
  ledcSetup(channel, freqd5, resolution);
  ledcAttachPin(25, channel);

  // Konfigurerer 7segment
  byte numDigits1 = 1;
  byte digitPins1[] = {};
  byte segmentPins1[] = {12, 14, 27, 26, 18, 33, 32, 22};
  bool resistorsOnSegments = true;
  byte hardwareConfig = COMMON_ANODE;
  sevseg1.begin(hardwareConfig, numDigits1, digitPins1, segmentPins1, resistorsOnSegments);
  sevseg1.setBrightness(90);

  byte numDigits2 = 1;
  byte digitPins2[] = {};
  byte segmentPins2[] = {15, 4, 19, 21, 16, 17, 5, 0};
  sevseg2.begin(hardwareConfig, numDigits2, digitPins2, segmentPins2, resistorsOnSegments);
  sevseg2.setBrightness(90);

  // Printer at programmet starter
  Serial.println(txtStart);

  // Setter pins som inngår i bitmasken til å vekke ESP32'en om verdien HIGH
  // registreres på en av dem
  esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);
}

void loop() {
  switch(seqIs) {

    // Taste inn bruker. Bekreft for å taste inn rom
    case user:
      user_loop();
      break;

    // Taste inn om man ønsker å booke rom eller styre ressurser
    case veivalg:
      veivalg_loop();
      break;
      
    // Taste inn rom. Bekreft for å taste inn første siffer av timetall
    case room:
      room_loop();
      break;

    // Taste inn første siffer (A) av timetall (AB:CD). Bekreft for å taste inn andre siffer av timetall
    case hour1:
      hour1_loop();
      break;

    // Taste inn andre siffer (B) av timetall (AB:CD). Bekreft for å taste inn første siffer av minutt
    case hour2:
      hour2_loop();
      break;

    // Taste inn første siffer (C) av minutt (AB:CD). Bekreft for å taste inn andre siffer av minutt
    case minu1:
      minu1_loop();
      break;

    // Taste inn andre siffer (D) av minutt (AB:CD). Bekreft for å taste inn første siffer av varighet
    case minu2:
      minu2_loop();
      break;

    // Første siffer av varighet (i minutter). Bekreft for å taste inn andre siffer av varighet
    case dur1:
      dur1_loop();
      break;

    // Andre siffer av varighet (i minutter) eller H, som vil si at første siffer indikerer antall timer
    // Bekreft for å konkludere bookingen
    case dur2:
      dur2_loop();
      break;

    // Skriver informasjon til seriemonitor og CoT. Venter på tilbakemelding fra CoT og displayer den.
    // Går deretter tilbake til stanby-mode
    case conc:
      conc_loop();
      break;

    // Første del av styringsmodulen. Velger hvilken ressurs operasjoner skal utføres på
    case enhetsvalg:
      enhetsvalg_loop();
      break;

    // Styring av varmekabler
    case varmekabler:
      varmekabler_loop();
      break;

    // Styrings av vifte
    case vifte:
      vifte_loop();
      break;

    // Styring av vindu
    case vindu:
      vindu_loop();
      break;

    // Styring av termostat
    case termostat:
      termostat_loop();
      break;

    // Sekvens for inntasting av temperatur, første siffer
    case temp1:
      temp1_loop();
      break;

    // Sekvens for inntasting av temperatur, andre siffer
    case temp2:
      temp2_loop();
      break;

    // Standby-modus
    case standby:
      standby_loop();
      break;
  }
}
