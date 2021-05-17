

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void deepSleep() {//Setter ESP32 i deep sleep når begge knapper har vært nedtrykt i en bestemt tid


  if (digitalRead(button1) || digitalRead(button2)) {//Forlenger tid for automatisk deep sleep
    resetMillis = millis() + autoSleep;
  }

  if (millis() > resetMillis) {//Starter deep sleep ved inaktivitet
    lcd.clear();
    esp_deep_sleep_start();
  }

  if (digitalRead(button1) && digitalRead(button2)) {//Kjører manuell deep sleep
    if (pending) {//Kjøres etter reset har vært satt
      if (millis() > reset) {



        while (digitalRead(button1) || digitalRead(button2)) {//Hindrer at deep sleep starter når wakeup-knapper holdes inn
          lcd.clear();

          lcd.setCursor(0, 0);
          lcd.print("Avslutter");


          delay(100);
        }
        delay(100);
        lcd.clear();
        esp_deep_sleep_start();//Starter deep sleep
      }

    }

    else {
      reset = millis() + deepSleepHoldTime;//Bestemmer når ESP32 skal settes i deep sleep

      pending = 1;//registrerer at tid har blitt satt



    }
  }
  else {//Tilbakestiller verdier når trykknapper slippes for tidlig
    pending = 0;
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//=======================================================================================================================================================================================
void adminControl() {//Kontrollerer hvilken kode som skal kjøre
  switch (admin) {

    case adminStandby: {//Klargjør LCD-skjerm for plassering i hovedmeny
        digit1 = 1;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Registrer Gjest");
        admin = setting;


        break;
      }

    case setting: {//Kjører funksjon for hovedmeny
        settingFunction();
        break;
      }

    case energyData: {//Kjører funksjon for energidata
        energyDataFunction();
        break;
      }

    case registerGuest: {//Kjører funksjon for registrering av gjest
        registerGuestFunction();
        break;
      }

    case adminRoom: {//Kjører funksjon for valg av rom
        adminRoomFunction();
        break;
      }

    case chooseMode: {//Kjører funksjon for valg mellom informasjon og brukerfjerning
        chooseModeFunction();
        break;
      }

    case getValue: {//Kjører funksjon for henting av rominformasjon fra RPi
        getValueFunction();
        break;
      }

    case cancel: {//Kjører funksjon for fjerning av brukere fra booking
        removeUser();
        break;
      }
  }
}
//=======================================================================================================================================================================================
