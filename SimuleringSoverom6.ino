#include <ESP32Servo.h>
#include <analogWrite.h>

#include <CircusESP32Lib.h>


char ssid[] = "Shady nettverk";
char password[] = "heiheihei";
char server[] = "www.circusofthings.com";
char order_key_takvifte[] = "6990";
char order_key_varmekabler[] = "18600";
char order_key_luftevindu[] = "3345";
char order_key_panelovn[] = "19380";
char token[] = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiI0OTczIn0.rf05c4f__YEi5lmmYKuXML9WKeZCjR-jNbCns2MUIQ8";
CircusESP32Lib circusESP32(server, ssid, password);

//-------Servo-------//

int servoPin = 13;// setter servo på pinne 13

Servo Servo1; // kallenavn på servo

//-------Varmekabler bad og soverom-------//

int R_lys_pin_gulv = 33; //rødt lys på RGB - viser at varmekabler er koblet inn
int G_lys_pin_gulv = 27; //grønt lys på RGB - viser at varmekabler har nådd ønsket temperatur


//-------Panelovn soverom-------//

int R_lys_pin_panel = 4; //rødt lys på RGB - viser at termostat er koblet inn
int G_lys_pin_panel = 5; //grønt lys på RGB F- viser at termostat har nådd ønsket temperatur


//-------Takvifte-------//

const int EN1 = 15; // fester H-bro til ESP32 - Enabler
const int IN1 = 25; // fester H-bro til ESP32 - Input 1
const int IN2 = 32; // fester H-bro til ESP32 - Input 2

//-------Temperaturmåler-------//

int tempPin = 12; //setter temperaturmåler TMP på pinne 12
float tempVal; //rådata på temperaturmåler
float temp; // faktisk temperaturvaliabel
float volt; // variabel for å lagre spenning

int prevButton; // button hindrer at det blir registrert flere knappetrykk etter knapp er trykket
int counter = 0; // endrer mellom at takvifte er temperaturbasert eller høy og lav


void setup() {

  Servo1.setPeriodHertz(50); //Servo 50hz
  Serial.begin(115200);
  Serial.println("3"); //printer som en decimal
  circusESP32.begin(); // Kommunikasjon med cot
  Serial.println("4");//printer som en decimal
  Servo1.attach(servoPin); // Kobling til pinnen
  pinMode(servoPin, OUTPUT); //Setter servoPin som output


  //analogWriteResolution(9); // bestemmer hvor mange bits analogWrite verdien skal få
  pinMode(EN1, OUTPUT); // setter DC-motor ENabler- takvifte - til output
  pinMode(IN1, OUTPUT); // setter DC-motor INput1- takvifte - til output
  pinMode(IN2, OUTPUT); // setter DC-motor INput2 - takvifte - til output
  pinMode(tempPin, INPUT); // setter temperaturmål - til input
  

  pinMode(R_lys_pin_gulv, OUTPUT); // setter RGB-led - rødt lys - til output
  pinMode(G_lys_pin_gulv, OUTPUT); // setter RGB-led - grønt lys - til output
  pinMode(R_lys_pin_panel, OUTPUT); // setter RGB-led - rødt lys - til output
  pinMode(G_lys_pin_panel, OUTPUT); // setter RGB-led - grønt lys - til output
}

void loop() {
  tempVal = analogRead(tempPin); // setter verdien på temperaturmåler
  volt = tempVal * 3.3;
  volt /= 4096;
  temp = (volt - 0.33) * 100;
  temp += 55; // pga av feilmålinger på TMP-sensor, så er volt i dette tilfelle satt til 0 grader og deretter er det lagt til 55 grader, for å oppnå en realistisk temperatur på 22 grader
  Serial.print("!!!!Temperaturen er ");
  Serial.println(temp);

  int status_key_takvifte = circusESP32.read(order_key_takvifte, token);
  delay(500);


  digitalWrite(IN1, HIGH); // setter DC-motor - takvifte - INput1 - til høy(på)
  digitalWrite(IN2, LOW); // setter DC-motor - takvifte - INput2 - til lav(av)

  status_key_takvifte = circusESP32.read(order_key_takvifte, token);
  delay(500);





  Serial.println(temp);

  if (status_key_takvifte == 1) {
    digitalWrite(EN1, HIGH);
    Serial.println("Fan On"); // hvis status er 1, så er takvifte på
    delay(100);
  }

  else if (status_key_takvifte == 0) { // setter trykknappstatus til 2 - som skrur takvifte på - men er temperaturbasert
    digitalWrite(EN1, LOW);
    Serial.println("Fan Off");
    delay(100);
  }

  else {

    if (temp < 20) // hvis temperatur er under 20 grader...
    {
      digitalWrite(EN1, 0);
      Serial.println(" Fan off   ");// ..så er vifte av
      delay(100);
    }

    else if (temp < 22) // hvis temperatur er mellom 20 og 22 grader..
    {
      analogWrite(EN1, 51);
      Serial.println(" Fan speed: 20 %   "); // ..så går vifte med 20% hastighet
      delay(100);
    }

    else if (temp < 24) // hvis temperatur er mellom 22 og 24 grader..
    {
      analogWrite(EN1, 102);
      Serial.println("Fan Speed: 40%   "); //.. så går vifte med 40% hastighet
      delay(100);
    }

    else if (temp < 26) // hvis temperatur er mellom 24 og 26 grader..
    {
      analogWrite(EN1, 153);
      Serial.println("Fan Speed: 60%   "); //.. så går vifte med 60% hastighet
      delay(100);
    }

    else if (temp < 29) // hvis temperatur er mellom 26 og 29 grader..
    {
      analogWrite(EN1, 204);
      Serial.println("Fan Speed: 80%    "); // ..så går vifte med 80% hastighet
      delay(100);
    }
    else if (temp > 29) // hvis temperatur er over 29 grader..
    {
      analogWrite(EN1, 255);
      Serial.println("Fan Speed: 100%   "); // så går vifte med 100% hastighet
      delay(100);
    }
  }


  Serial.print("Status key_varmekabler: ");
  int status_key_varmekabler = circusESP32.read(order_key_varmekabler, token);
  delay(500);

  if (temp < status_key_varmekabler) { // hvis temperatur er under gitt verdi i CoT..
    RGB_farge_gulv(110, 0); // RØD // ..så lyser LED rødt og starter varmekabler i gulv på bad og soverom for oppvarming
    Serial.println("Low temperature"); //
  }
  else if (status_key_varmekabler != 0) {
    RGB_farge_gulv(0, 90); // GRØNN // hvis temperatur er over 22 grader..
    Serial.println("High temperature"); //..så lyser LED grønt og varmekabler er ikke innkoblet
  }

  else if (status_key_varmekabler == 0) {
    RGB_farge_gulv(0, 0); // Ingen lys om varmekabler er skrudd av
    Serial.println("Cables off");
  }

  Serial.print("Status key_panelovn: ");
  int status_key_panelovn = circusESP32.read(order_key_panelovn, token);
  delay(500);

  if (temp < status_key_panelovn) { // hvis temperatur er under gitt verdi i CoT..
    RGB_farge_panel(110, 0); // RØD // ..så lyser LED rødt og starter panelovn
    Serial.println("Low temperature"); //
  }
  else if (status_key_panelovn != 0) {
    RGB_farge_panel(0, 90); // GRØNN // hvis temperatur er under gitt verdi i CoT...
    Serial.println("High temperature"); //..så lyser LED grønt og panelovn har nådd ønsket temp
  }

  else if (status_key_panelovn == 0) {
    RGB_farge_panel(0, 0); // Ingen lys om varmekabler er skrudd av
    Serial.println("Panel oven off");
  }

  Serial.print("Status key_luftevindu: ");
  int status_key_luftevindu = circusESP32.read(order_key_luftevindu, token);
  delay(500);

  Serial.print("Verdi: ");
  Serial.println(status_key_luftevindu);

  Serial.print("Vindu (0-1) set to: ");
  Serial.println();
  delay(100);

  if (status_key_luftevindu == 1) { // Hvis signal "1" fra cot: åpner luftevindu i 90 grader
    Servo1.write(90);
    delay(100);
  }

  else if (status_key_luftevindu == 0) { // Hvis signal"0" fra cot: Lukker vindu
    Servo1.write(0);
    delay(100);
  }


  delay(50);
}


void RGB_farge_gulv(int R_lys_value_gulv, int G_lys_value_gulv) {
  analogWrite(R_lys_pin_gulv, R_lys_value_gulv);
  analogWrite(G_lys_pin_gulv, G_lys_value_gulv);
}
void RGB_farge_panel(int R_lys_value_panel, int G_lys_value_panel) {
  analogWrite(R_lys_pin_panel, R_lys_value_panel);
  analogWrite(G_lys_pin_panel, G_lys_value_panel);
}
