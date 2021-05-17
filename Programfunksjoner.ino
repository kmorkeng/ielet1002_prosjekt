
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Hovedmenyen for admin-enheten. Her kan man velge hvilken av hovedoppgavene til admin-enheten du ønsker å utføre

void settingFunction() {//Funksjon for valg av bruksområde
  if (numButton == 1) {//Hindrer repeterte knappetrykk
    if (digitalRead(button1) == buttonOff) {
      numButton = 0;
      delay(50);
    }
  }

  else if (digitalRead(button1) == buttonOn) { //Registrerer knappesignal

    if (digit1 > 2) { //Stiller display tilbake til 0 fra 9
      digit1 = 1;
      numButton = 1;//Registrerer at knapp er trykt
    }
    else { //Inkrementerer display
      digit1++;
      numButton = 1; //Registrerer at knapp er trykt
    }
    lcdFunc1();
  }
  if (digitalRead(button2) == buttonOn) {
    lcd.clear();


    switch (digit1) {//Flytter tilstand og klargjør LCD-skjerm
      case 1: {
          admin = registerGuest;
          digit1 = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Gjest Nummer:");

          lcd.setCursor(0, 1);
          lcd.print(digit1);

          break;
        }
      case 2: {
          admin = adminRoom;
          digit1 = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Stue");

          break;
        }
      case 3: {

          admin = energyData;

          digit1 = 1;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Stromproduksjon");
          lcd.setCursor(0, 1);
          lcd.print("Sanntid");
          break;
        }
    }

  }
  while (digitalRead(button2) == buttonOn);//Hindrer at programmet kjører før knapp slippes

  delay(10);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//=======================================================================================================================================================================================

//Printer valgt funksjonalitet på LCD-skjermen

void lcdFunc1() {//Funksjon for styring av LCD-skjerm
  switch (digit1) {
    case 1: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Registrer Gjest");
        break;
      }
    case 2: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fellesarealer");
        break;
      }
    case 3: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Energidata");
        break;
      }

  }
}
//=======================================================================================================================================================================================



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Velger ønsket energidata, og deretter printer valgt energidata på LCD-skjermen. 

void energyDataFunction() {//Funksjon for henting av energidata

  if (digitalRead(button1) == buttonOn) {//Bytte mellom strømpris og strømproduksjon
    if (digit1 == 1) {
      digit1 = 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Strompris");
      lcd.setCursor(0, 1);
      lcd.print("Sanntid");
    }
    else if (digit1 == 2) {
      digit1 = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Stromproduksjon");
      lcd.setCursor(0, 1);
      lcd.print("Sanntid");
    }
  }
  while (digitalRead(button1));//Stopper til knapp 1 slippes





  if (digitalRead(button2) == buttonOn) {//Velger datatype og henter data

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Henter data");



    if (digit1 == 2) {//Henter og printer valgt informasjon

      currentPrice = circusESP32.read(energyDataKey, token1);
      delay(500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Stromprisen");
      lcd.setCursor(0, 1);
      lcd.print("er ");
      lcd.print(currentPrice);
      lcd.print(" ore/kWh");
      admin = adminStandby;
    }
    if (digit1 == 1) {//Henter og printer valgt informasjon

      currentProd = circusESP32.read(productionKey, token1);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Stromproduksjon");
      lcd.setCursor(0, 1);
      lcd.print("er ");
      lcd.print(currentProd);
      lcd.print(" kW");
      admin = adminStandby;
    }

    while ((digitalRead(button1) || digitalRead(button2)) == 0);//Venter til en knapp er påtrykt

    delay(10);
    while (digitalRead(button1) || digitalRead(button2));

    delay(10);
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//=======================================================================================================================================================================================

//Lar bruker skrive inn 8 tall på LCD-skjermen, og deretter sender nummer til RPi dersom det ble rett innskrevet

void registerGuestFunction() {//Funksjon for registrering av gjestenummer




  if (numLen < 8) {

    if (numButton == 1) { //Hindrer repeterte knappesignaler
      if (digitalRead(button1) == buttonOff) {
        numButton = 0;//Tillater nye knappesignal
        delay(50);//Hindrer støy
      }
    }

    else if (digitalRead(button1) == buttonOn) { //Registrerer knappesignal

      if (digit1 > 8) { //Stiller display tilbake til 0 fra 9
        digit1 = 0;
        numButton = 1;//Registrerer at knapp er trykt
        lcd.setCursor(numLen, 1);
        lcd.print(digit1);
      }
      else { //Inkrementerer display
        digit1++;
        numButton = 1; //Registrerer at knapp er trykt
        lcd.setCursor(numLen, 1);
        lcd.print(digit1);
      }
    }



    if (selectButton == 1) {
      if (digitalRead(button2) == buttonOff) {
        selectButton = 0;
        delay(50);
      }
    }
    else if (digitalRead(button2) == buttonOn) {


      numLen += 1;//Oppdaterer resterenede siffer

      guestNumber = guestNumber * 10 + digit1;//Legger til inntastet siffer i nummer
      selectButton = 1;
      lcd.setCursor(0, 1);
      lcd.print(guestNumber);
      lcd.print(digit1);




    }
  }

  else {

    if (repeatRun == 0) {//Kjører kode ved første gjennomgang
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Riktig?");
      lcd.setCursor(0, 1);
      lcd.print(guestNumber);
      repeatRun = 1;
      yes = 1;
      lcd.setCursor(8, 0);
      lcd.print("Ja ");


    }


    if (digitalRead(button1) == buttonOn) {//Endrer valg
      if (yes == 1) {
        yes = 0;
        lcd.setCursor(8, 0);
        lcd.print("Nei");
      }
      else if (yes == 0) {
        yes = 1;
        lcd.setCursor(8, 0);
        lcd.print("Ja ");
      }
    }
    while (digitalRead(button1) == buttonOn);
    delay(10);
    if (digitalRead(button2) == buttonOn) {//Velger ja/nei
      if (yes == 0) {
        numLen = 0;

        repeatRun = 0;
        guestNumber = 0;
        lcd.clear();
        admin = adminStandby;
      }
      else if (yes == 1) {

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Sender til RPi");
        circusESP32.write(generalKey, guestNumber, token2);
        numLen = 0;

        repeatRun = 0;
        while (guestNumber) {
          delay(1000);
          guestNumber = circusESP32.read(generalKey, token2);
        }


        lcd.clear();
        admin = adminStandby;



      }
    }
    while (button2 == buttonOn);
    delay(10);

  }
}
//=======================================================================================================================================================================================



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Velger ønsket rom å kjøre "Se informasjon"- og "Fjern bruker"-funksjonalitetene

void adminRoomFunction() {//Funksjon for valg av rom 
  if (numButton == 1) {
    if (digitalRead(button1) == buttonOff) {
      numButton = 0;
      delay(50);
    }
  }

  else if (digitalRead(button1) == buttonOn) { //Registrerer knappesignal

    if (digit1 > 3) { //Stiller display tilbake til 1 fra 3
      digit1 = 1;
      numButton = 1;//Registrerer at knapp er trykt
    }
    else { //Inkrementerer display
      digit1++;
      numButton = 1; //Registrerer at knapp er trykt
    }
    lcdFunc2();
  }
  if (digitalRead(button2) == buttonOn) {

    if (digit1 == 4) {
      admin = adminStandby;
    }
    else {

      roomUser = 10 * digit1;//Legger til rom i rom-bruker-variabel

      roomSelect = digit1;
      admin = chooseMode;
      digit1 = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Se informasjon");
    }
  }
  while (digitalRead(button2) == buttonOn);

  delay(10);
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//=======================================================================================================================================================================================

//Printer valgt rom/tilbake

void lcdFunc2() {//Funksjon for kontroll over LCD-skjerm
  switch (digit1) {
    case 1: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Stue");
        break;
      }
    case 2: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Kjokken");
        break;
      }
    case 3: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Bad");
        break;
      }
    case 4: {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Tilbake");
        break;
      }

  }

}
//=======================================================================================================================================================================================



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Gir valg mellom å se informasjon elleå å fjerne bruker fra rommet valgt i adminRoomFunction().

void chooseModeFunction() {//Funksjon for valg mellom informasjon og fjerning av bruker
  if (digitalRead(button1) == buttonOn) {//Blar gjennom ønsket funksjonalitet
    
    if (digit1 == 2) {//Viser informasjon
      digit1 = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Se informasjon");

    }
    else if (digit1 == 1) {//Fjerner bruker
      digit1 = 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Fjern bruker");

    }
  }
  while (digitalRead(button1));
  delay(10);
  if (digitalRead(button2) == buttonOn) {//Velger ønsket funksjonalitet
    if (digit1 == 1) {
      admin = getValue;
    }
    else {
      lcd.print(": ");
      digit1 = 1;
      lcd.setCursor(0, 1);
      lcd.print(digit1);
      admin = cancel;
    }
  }
  while (digitalRead(button2));
  delay(10);

}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



//=======================================================================================================================================================================================

//Henter booking-data fra rom valgt i adminRoomFunction(), og displayer det på LCD-skjermen.

void getValueFunction() {//Fuinksjon for innhenting av data fra RPi

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Henter data");

  circusESP32.write(generalKey, roomSelect, token2);//Sender bestemt rom til CoT
  delay(1000);



  CoTcheck = circusESP32.read(generalKey, token2);//Leser av verdi fra CoT

  while (CoTcheck != 0) {
    CoTcheck = circusESP32.read(generalKey, token2);//Venter til signal blir tilbakestilt

    delay(1000);
  }


  regUsers = circusESP32.read(spaceKey, token2);//Henter registrerte brukere fra RPi
  delay(500);


  lcd.clear();
  lcd.setCursor(0, 0);

  switch (roomSelect) {//Printer plassering på LCD-skjerm
    case 1: {
        lcd.print("Stue");
        break;
      }
    case 2: {
        lcd.print("Kjokken");
        break;
      }
    case 3: {
        lcd.print("Bad");
        break;
      }
  }
  lcd.setCursor(0, 1);
  lcd.print("Brukere: ");

  space1 = regUsers % 10;//Henter ut bruker 1

  regUsers -= space1;//Fjerner bruker 1 fra variabel

  space2 = (regUsers % 100)/10;//Henter ut bruker 2

  regUsers -= space2;//Fjerner bruker 2 fra variabel

  space3 = regUsers/100;//Henter bruker 3

  lcd.print(space1);
  lcd.print(",");
  lcd.print(space2);
  lcd.print(",");
  lcd.print(space3);


  while ((digitalRead(button1)||digitalRead(button2)) == 0);//Venter til en valgfri knapp blir påtrykt
  delay(10);

  while (digitalRead(button1)||digitalRead(button2));//Venter til ingen knapper er påtrykt
  delay(10);
  

  lcd.clear();
  lcd.setCursor(0,0);
  digit1=1;
  lcd.print("Stue");

  admin = adminRoom;//Tilbakestiller admin-enhet til hovedmeny

  delay(10);

}
//=======================================================================================================================================================================================



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Velger en bruker som skal fjernes fra rom valgt i adminRoomFunction().

void removeUser() {//Funksjon for fjerning av bruker fra booking-database
  if (numButton == 1) { //Hindrer repeterte knappesignaler
    if (digitalRead(button1) == buttonOff) {
      numButton = 0;//Tillater nye knappesignal
      delay(50);//Hindrer støy
    }
  }

  else if (digitalRead(button1) == buttonOn) { //Registrerer knappesignal

    if (digit1 > 5) { //Stiller display tilbake til 0 fra 9
      digit1 = 1;
      numButton = 1;//Registrerer at knapp er trykt
      lcd.setCursor(0, 1);
      lcd.print(digit1);
    }
    else { //Inkrementerer display
      digit1++;
      numButton = 1; //Registrerer at knapp er trykt
      lcd.setCursor(0, 1);
      lcd.print(digit1);
    }
  }

  if (digitalRead(button2) == buttonOn) {
    roomUser += digit1;//Legger til bruker til CoT-signalet

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fjerner bruker ");
    lcd.print(digit1);
    delay(100);
    circusESP32.write(generalKey, roomUser, token2); //Sender signal til CoT
    while (roomUser) {
      delay(1000);
      roomUser = circusESP32.read(generalKey, token2);//Venter til RPi tilbakestiller signalet
    }
    delay(1000);
    while (digitalRead(button2));
    delay(10);

    admin = adminStandby;//Flytter admin-enhet tilbake til hovedmeny
  }
}
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
