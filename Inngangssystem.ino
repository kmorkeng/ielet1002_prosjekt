//------------------------------------------Inkludering av biblioteker--------------------------------------------------

#include <CircusESP32Lib.h>             // Biblioteket for ESPen til Circus of things
#include <Keypad.h>                     // Biblioteket til tastaturet hvor man taster inn passordet
#include <analogWrite.h>                // Bibliotek for å kunne skrive analogWrite med en ESP
#include "Wire.h"                       // Biblioteket som snakker med I2C 
#include <WiFi.h>                       // Kobler ESP-til WiFi
#include "Adafruit_LiquidCrystal.h"     // Biblioteket for å sette opp LCD displayet
Adafruit_LiquidCrystal lcd(0x70);       // klargjør LCD-skjermen


//------------------------------------------ Oppsett til CoT og WiFi --------------------------------------------------

char ssid[] = "";                                         //nettverksnavnet
char password[] = "";                                     // passordet til nettverket
char token[] = "";                                        // tilgang til brukeren i CoT
char server[] = "www.circusofthings.com";                 // 
char keyAcceptDenied[] = "21617";                         // kode for tilgang til signalet
char keyChessPasscode[] = "6032";                         // ------||-----
char keyPasscode[] = "9656";                              // ------||-----
CircusESP32Lib circusESP32(server, ssid, password);       //nettadressen leses for CoT, ssid og passordet til nettverket


//------------------------------------------ Oppsett RGB-led --------------------------------------------------

int redLedPin = 5;         //rød
int blueLedPin = 15;       //blå
int greenLedPin = 0;       //grønn


//------------------------------------------ Passord / Telefonnummer, definerer sifferrekkefølge----------------------------------------------
// Her defineres sifferene og variablene som skal sendes via ESP-CoT 

int digit1 = 0;                 //første siffer derson man taster inn telefonnummer
int digit2 = 0;                 //
int digit3 = 0;                 //
int digit4 = 0;                 //
int digit5 = 0;                 //første siffer dersom man taster inn et passord
int digit6 = 0;                 //
int digit7 = 0;                 //
int digit8 = 0;                 //

byte digitCount = 0;            //antall påtrykte tall fra keypad

int passcodeVar;                //variablen som brukes til å sende over passord-signalet til til CoT, variablen blir videre definert i "void pinVar()"
int phoneNumbervar;             //variablen som brukes til å sende over telefonnummer-signalet til til CoT, variablen blir videre definert i "void phoneVar()"

int one = 1;                    //er en konstant som sendes hver gang en kode/telefonnummer sendes, slik at Python vet at de skal lese signalet

//------------------------------------------Oppsett for LCD-skjerm--------------------------------------------------
                                       
#define PasswordLength 5                //Definerer passordlengde på 5 siden det under er en char, passordet består derfor av fire tegn pluss én 0 på enden, som representerer enden av arrayet. 4 (passordlengde) + 1 (null character) = 5 som er passordlengde
char Data[PasswordLength];              //Deklarerer et array. Denne variablen vil holde på verdien til "Data"-inputen i en string.
                                        

#define PhoneNumberLength 9             // Definerer telefonnummer til 9, telefonnummeret består av 8 tall pluss én 0 på enden
char Number[PhoneNumberLength];         // Deklarerer et array, hvor variablen vil holde input verdien i en string på 8 tegn
int GuestPassword = 0000;               // Definerer hva gjestepassordet skal være, alle gjester har dette passordet

//------------------------------------------ Oppsett tastatur også kjent som Keypad----------------------------------------------------

const byte ROWS = 4;                     //rader
const byte COLS = 3;                     //kolonner

char hexaKeys[ROWS][COLS] = {            //Setter opp hvilke verdier som skal skriver når man trykker inn de forskjellige knappene på tastaturet
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {27, 26, 25, 33};  // definerer pinnene som brukes til radene
byte colPins[COLS] = {32, 12, 14};      // definerer pinnene som brukes til kolonnene

int inputGuest;

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


void setup() {  
  pinMode(redLedPin, OUTPUT);              // setter RØD led som utgangspinne
  pinMode(blueLedPin, OUTPUT);             // setter BLÅ led som utgangspinne
  pinMode(greenLedPin, OUTPUT);            // setter GRØNN led som utgangspinne 
  Serial.begin(115200);                    // setter datahastigheten til 115200 bits per sekund 
  circusESP32.begin();                     // starter opp CoT med ESP
  lcd.begin(16, 2);                        // definererdisplayet på LCD-skjermen
  lcd.clear();                             // Nullstilller displayet for tekst
  lcd.setCursor(0, 0);                     // startsposisjonen til det neste som skal printes
  lcd.print("Tast inn kode:");             // Printer anvist tekst til LCD-skjermen
}


//---------------------------------------------- Setter sammen kombinasjonen som blir tastet inn -----------------------------------------------------------------------------
void phoneVar() {
  phoneNumbervar = ((digit1) * 10000000) + ((digit2) * 1000000) + ((digit3) * 100000) + ((digit4) * 10000) + ((digit5) * 1000) + ((digit6) * 100) + ((digit7) * 10) + (digit8);       //setter sammen telefonnummeret fra enkeltvariabler til et heltall

  circusESP32.write(keyPasscode, phoneNumbervar, token);                              //sender telefonnummeret til CoT
  circusESP32.write(keyChessPasscode, one, token); 
  Serial.println(digit1);
}


void pinVar() {
  passcodeVar = (((digit5) * 1000) + ((digit6) * 100) + ((digit7) * 10) + (digit8)); //setter sammen koden fra enkeltvariabler til et heltall                                                            
  if (passcodeVar != 0) {                                                            //sjekker om heltallet ikke er lik 0
    circusESP32.write(keyPasscode, passcodeVar, token);                              //sender koden til CoT
    delay(500);                                                                      
    circusESP32.write(keyChessPasscode, one, token);                                 //sier til CoT at det er klart for Python å lese signalet
    delay(500);                                                                      
  }
}


void loop() {
  int digitKeypad = customKeypad.getKey();                                           //henter verdiene som blir tastet på keypaden

  if (inputGuest != 0) {                                                             //sjekker om det er gjestepassordet som er tastet inn
    lcd.clear();                                                                     //Nullstilller displayet for teksten 
    lcd.setCursor(0, 0);                                                             //Starter sitering på første linje 
    lcd.print("Telefonnummer:");                                                     //Printer "Telefonnummer:" på linja
    digitCount = 0;                                                                  //setter verdien av påtrykte tall fra keypad til 0

    while (1) {

      digitKeypad = customKeypad.getKey();                                           //henter verdiene som blir tastet på keypaden


      if (digitKeypad) {                                                             //sjekker om det blir tastet noe på keypaden
        rgbColor(90, 0, 30);                                                         //setter RGB-leden til gul farge
        Number[digitCount] = digitKeypad;                                            //teller antall påtrykk fra keypad
        digitCount++;                                                                //legger til ett påtrykk for hver gang det blir påtrykkt noe fra keypaden
        if (digitCount == 1) {                                                       //sjekker etter første påtrykk fra keypad
          lcd.setCursor(0, 1);                                                       //Starter sitering på andre linje 
          digit1 = digitKeypad - 48;                                                 //gjør om påtrykket fra keypdad fra ASCII kode til desimal form
          lcd.print(digit1);                                                         //printer det første tallet påtrykt fra kaypad på LCD-skjermen
        }
        else if (digitCount == 2) {                                                  //Gjenntar samme hendelse som over 8 ganger for hvert tall i et telefonnummer
          digit2 = digitKeypad - 48;
          lcd.print(digit2);

        }
        else if (digitCount == 3) {
          digit3 = digitKeypad - 48;
          lcd.print(digit3);

        }
        else if (digitCount == 4) {
          digit4 = digitKeypad - 48;
          lcd.print(digit4);

        }
        else if (digitCount == 5) {
          digit5 = digitKeypad - 48;
          lcd.print(digit5);

        }
        else if (digitCount == 6) {
          digit6 = digitKeypad - 48;
          lcd.print(digit6);

        }
        else if (digitCount == 7) {
          digit7 = digitKeypad - 48;
          lcd.print(digit7);

        }
        else if (digitCount == 8) {
          digit8 = digitKeypad - 48;
          lcd.print(digit8);

          phoneVar();                                                                   //sender telefonnummeret til phoneVar loopen
          delay(200);

          waitloop();                                                                   //venter på resultat fra CoT
          clearData();                                                                  //setter antall påtrykk fra keypad til 0
          inputGuest = 0;                                                               //setter verdien til 0 så koden  bryter ut av gjestepassord casen

          digit8 = 0;                                                                   //nullstiller
          digitCount = 0;                                                               //nullstiller 
          digitKeypad = 0;                                                              //nullstiller
          break;                                                                        //bryter ut av while loopen
        }


      }
    }
  }
  if (digitKeypad) {                                                                    //sjekker om det blir tastet noe på keypaden
    rgbColor(90, 0, 30); // GUL                                                         //setter RGB-leden til gul farge
    Data[digitCount] = digitKeypad;                                                     //teller antall påtrykk fra keypaden
    digitCount++;                                                                       //legger til ett påtrykk for hver gang det blir påtrykket noe fra keypaden
    if (digitCount == 1) {                                                              //sjekeker etter første påtrykk fra keypad
      lcd.setCursor(0, 1);                                                              //Starter sitering på andre linje                                      
      lcd.print("*");                                                                   //printer en stjerne for hvet påtrykk i koden
      digit5 = digitKeypad - 48;                                                        //gjør om påtrykket fra keypdad fra ASCII kode til desimal form
    }
    else if (digitCount == 2) {                                                         //gjentar samme hendelse som over 4 ganger for hvert tall i et telefonnummer
      lcd.setCursor(0, 1);
      lcd.print("* *");
      digit6 = digitKeypad - 48;
    }
    else if (digitCount == 3) {
      lcd.setCursor(0, 1);
      lcd.print("* * *");
      digit7 = digitKeypad - 48;
    }
    else if (digitCount == 4) {
      lcd.setCursor(0, 1);
      lcd.print("* * * *");
      digit8 = digitKeypad - 48;
      delay(250);

      if (((((digit5) * 1000) + ((digit6) * 100) + ((digit7) * 10) + (digit8)) == GuestPassword)) {         //sjekker etter tallkominasjonen for gjestepassordet internt
        inputGuest = 1;                                                                                     //sier fra at det er gjestepassordet som er påtrykt
      }
      else {
        pinVar();                                                                                           //sender telefonnummeret til phoneVar loopen

        waitloop();                                                                                         //venter på resultat fra CoT
        clearData();                                                                                        //setter antall påtrykk fra keypad til 0

      }
    }


  }

}
void rgbColor(int redLedValue, int blueLedValue, int greenLedValue){                                       //henter ut fargene rød, blå, grønn
  analogWrite(redLedPin, redLedValue);                                                                     //definerer fargen rød
  analogWrite(blueLedPin, blueLedValue);                                                                   //definerer fargen blå
  analogWrite(greenLedPin, greenLedValue);                                                                 //defineret fagen grønn
}

void clearData() {                                                                                         //lager en slett data loop
  while (digitCount != 0) {                                                                                //sjekker at antall påtrykk fra keypad er ulik 0
    Data[digitCount--] = 0;                                                                                //setter telleren av påtrykk fra keypad til null
  }
  return;
}


void waitloop() {                                                                                         //Loop for å vente på svar fra databasen gjennom CoT
  int check = circusESP32.read(keyPasscode, token);                                                       //Sjekker om svaret har kommet til CoT
  while (check != 0) {                                                                                    //Når svaret ikke har kommet fram, kjører denne loopen
    delay(500);
    check = circusESP32.read(keyPasscode, token);                                                         //Sjekker etter svaret på nytt

    if (check == 0) {                                                                                     //Hvis svaret har kommet inn, bryt loopen
      continue;
    }
  }
  displayResponse();                                                                                      //Gi respons som korresponderer med svaret

}


void displayResponse() {                                                    //
  int bekreft = circusESP32.read(keyAcceptDenied, token);                   //leser av verdien i CoT 
  if (bekreft == 1) {                                                       //dersom verdien er 1 blir passordet godkjent
    lcd.clear();  
    rgbColor(0, 0, 90);                                                     //setter LEDen til å lyse grønnt
    lcd.setCursor(0, 0);
    lcd.print("Velkommen!");                                                //printer "velkommen" på LCD-skjermen
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tast inn kode:");                                            //setter skjermen tilbake til "tast inn kode:"
    rgbColor(0, 0, 0);                                                      //slår av LEDen
  }
  else if (bekreft == 0) {                                                  //dersom verien er 0 blir passordet ikke godkjent
    rgbColor(110, 0, 0);                                                    //setter LEDen til å lyse rødt
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ugyldig passord?");
    delay(3000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tast inn kode:");
    rgbColor(0, 0, 0);
  }
}
