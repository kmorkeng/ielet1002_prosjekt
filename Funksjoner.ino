// Leser verdien som er skrevet til varmekabler/panelovn
void firstCheck() {
  int sjekkG = circusESP32.read(gulvKey, token1);
  if (sjekkG == 0) {
    gulvStatus = 0;
    }
    else {
    gulvStatus = 1;
    }
    
  int sjekkV = circusESP32.read(varmeKey, token1);
  if (sjekkV == 0) {
    varmeStatus = 0;
  }
  else {
    varmeStatus = 1;
  }
  first = 1;
}

// Skrur på panelovn hvis den er av, eller av hvis den er på
void toggleVarme() {
  if (varmeStatus == 1) {
    prevVarme = currVarme;
    currVarme = 0;
    circusESP32.write(varmeKey, currVarme, token1);
    varmeStatus = 0;
    sad_beep();
  }
  else if (varmeStatus == 0) {
    currVarme = prevVarme;
    circusESP32.write(varmeKey, currVarme, token1);
    varmeStatus = 1;
    happy_beep();
  }
}

// Skrur på varmekabler hvis den er av, eller av hvis den er på
void toggleGulv() {
  if (gulvStatus == 1) {
    prevGulv = currGulv;
    currGulv = 0;
    circusESP32.write(gulvKey, currGulv, token1);
    gulvStatus = 0;
  }
  else if (gulvStatus == 0) {
    currGulv = prevGulv;
    circusESP32.write(gulvKey, currGulv, token1);
    gulvStatus = 1;
  }
}

// Lagrer verdien til riktig variabel
void checkTarget1() {
  if (target == 1) {
    saveGulv1 = digit1;
  }
  if (target == 0) {
    saveVarme1 = digit1;
  }
}

// Lagrer verdien til riktig variabel og sender til CoT
void checkTarget2() {
  if (target == 1) {
    saveGulv2 = digit2;
    currGulv = (saveGulv1 * 10) + saveGulv2; // Lager et heltall tilsvarende inntastet temperatur
    circusESP32.write(gulvKey, currGulv, token1); // Skriver den nye temperaturen til CoT
    happy_beep();
  }
  else if (target == 0) {
    saveVarme2 = digit2;
    currVarme = (saveVarme1 * 10) + saveVarme2; // Lager et heltall tilsvarende inntastet temperatur
    circusESP32.write(varmeKey, currVarme, token1); // Skriver den nye temperaturen til CoT
    happy_beep();
  }
}

// Separerer tallet for å finne sifret på enerplassen og sifret på tierplassen
void separate_display() {
  if (target == 1) {
    int tens = currGulv/10;
    int ones = currGulv%10;
    Serial.println(tens);
    Serial.println(ones);
    tensones();
  }
  else if (target == 0) {
    int tens = currVarme/10;
    int ones = currVarme%10;
    tensones();
  }
}

// Displayer heltallet på syvsegmentet
void tensones() {
  sevseg1.setNumber(tens);
  sevseg1.refreshDisplay();
  sevseg2.setNumber(ones);
  sevseg2.refreshDisplay(); 
}

// Operasjonsvalg for vindu
void vinduAction() {
  if (heihei == 1) { // Hvis digit2 er 1 skal gjeldende verdi for vindu i CoT displayes
    vinduReact();
    delay(150);
    if ((buttonState1 == HIGH) || (buttonState2 == HIGH)) {
      seqIs = standby;
    }
  }
  else if (heihei == 2) { // Hvis digit2 er 2 skal vindu lukkes om den er åpen eller åpnes om den er lukket
    if (vinduStatus == 0) {
      vinduStatus = 1;
      circusESP32.write(vinduKey, vinduStatus, token1);
      happy_beep();
      delay(150);
    }
    else if (vinduStatus == 1) {
      vinduStatus = 0;
      circusESP32.write(vinduKey, vinduStatus, token1);
      sad_beep();
      delay(150);
    }
    seqIs = standby;
  }
}

// Genererer en respons basert på gjeldende verdi i CoT
void vinduReact() {
  if (vinduStatus == 0) {
    sevseg1.setNumber(0);
    sevseg1.refreshDisplay();
    l();
    sad_beep();
  }
  else if (vinduStatus == 1) {
    sevseg1.setNumber(0);
    sevseg1.refreshDisplay();
    h();
    happy_beep();
  }
}

// Operasjonsvalg for vifte
void vifteAction() {
  if (hei == 1) { // Hvis digit2 er 1 skal gjeldende verdi for vifte i CoT displayes
    vifteReact();
    if ((buttonState1 == HIGH) || (buttonState2 == HIGH)) {
      seqIs = standby;
    }
  }
  else if (hei == 2) { // Hvis digit2 er 2 skal vifte skrus av om den er på, eller skrus på om den er av
    if (vifteStatus == 0) {
      vifteStatus = 1;
      circusESP32.write(vifteKey, vifteStatus, token1);
      happy_beep();
      delay(150);
    }
    else if (vifteStatus == 1) {
      vifteStatus = 0;
      circusESP32.write(vifteKey, vifteStatus, token1);
      sad_beep();
      delay(150);
    }
    seqIs = standby;
  }
}

// Genererer en respons basert på gjeldende verdi i CoT
void vifteReact() {
  if (vifteStatus == 0) {
    sevseg1.setNumber(0);
    sevseg1.refreshDisplay();
    l();
    sad_beep();
  }
  else if (vifteStatus == 1) {
    sevseg1.setNumber(0);
    sevseg1.refreshDisplay();
    h();
    happy_beep();
  }
}

// Displayer 01 på syvsegment
void displayZeroOne() {
  digit1 = 0;
  digit2 = 1;
  readyDisplay();
}

// Displayer 00 på syvsegment
void displayZeroZero() {
  digit1 = 0;
  digit2 = 0;
  readyDisplay();
}

// Setter displayet til variabelverdiene
void readyDisplay() {
  sevseg1.setNumber(digit1);
  sevseg1.refreshDisplay();
  sevseg2.setNumber(digit2);
  sevseg2.refreshDisplay();
}

// Leser av status for trykknapper
void buttonStatus() {
  buttonState1 = digitalRead(button1);
  buttonState2 = digitalRead(button2);
}

// Funksjon for å displaye "H" på syvsegment
void h() {
  digitalWrite(15, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(19, LOW);
  digitalWrite(4, HIGH);
  digitalWrite(16, LOW);
  digitalWrite(17, LOW);
  digitalWrite(5, LOW);
  digitalWrite(18, HIGH);
}

// Funksjon for å displaye "L" på syvsegment
void l() {
  digitalWrite(15, HIGH);
  digitalWrite(2, HIGH);
  digitalWrite(19, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(16, LOW);
  digitalWrite(17, LOW);
  digitalWrite(5, HIGH);
  digitalWrite(18, HIGH);
}

// Funksjoner som formatterer brukerinput klart til print og CoT

// Funksjon for å identifisere bruker
void whoDis() {
  switch(saveUser) {
    case 1:
      Serial.print("Kristin");
      break;
    case 2:
      Serial.print("Mina");
      break;
    case 3:
      Serial.print("Ragnhild");
      break;
    case 4:
      Serial.print("Kristoffer");
      break;
    case 5:
      Serial.print("Karoline");
      break;
    case 6:
      Serial.print("Marius");
      break;
  }
}

// Funksjon for å identifisere rom
void whereDis() {
  switch(saveRoom) {
    case 1:
      Serial.print("Kjøkken");
      break;
    case 2:
      Serial.print("Stue");
      break;
    case 3:
      Serial.print("Bad");
      break;
  }
}

// Setter sammen tidsvariablene til et klokkeslett
void outTime() {
  outHourVar = "";
  outMinuteVar = "";
  outTimeVar = "";
  // Setter sammen time som vil bli skrevet til CoT
  outHourVar.concat(saveHour1);
  outHourVar.concat(saveHour2);
  // Setter sammen minutt som vil bli skrevet til CoT
  outMinuteVar.concat(saveMinu1);
  outMinuteVar.concat(saveMinu2);
  // Setter sammen time og minutt for print
  outTimeVar.concat(outHourVar);
  outTimeVar.concat(":");
  outTimeVar.concat(outMinuteVar);
}

// Funksjoner som formatterer og printer informasjon til seriemonitor
void printBegin() {
  Serial.println();
  Serial.println();
  Serial.print(txtBooking);
}

void printUser() {
  Serial.println();
  Serial.print(txtBruker);
  whoDis();
}

void printRoom() {
  Serial.println();
  Serial.print(txtRom);
  whereDis();
}

void printTime() {
  Serial.println();
  Serial.print(txtStarttid);
  outTime();
  Serial.print(outTimeVar);
}

void printDuration() {
  Serial.println();
  Serial.print(txtVarighet);
  Serial.print(saveDur1);
  if (saveDur2 == 10) {
    if (saveDur1 == 1) {
      Serial.print(txtH1);
    }
    else {
      Serial.print(txtH);
    }
  }
  else {
    Serial.print(saveDur2);
    Serial.print(" minutter");
  }
  delay(500);
}

void printGoodbye() {
  Serial.println();
  Serial.print(txtTakk);
  Serial.println("");
  Serial.println(txtConn);
}

void info() {
  printBegin();
  printUser();
  printRoom();
  printTime();
  printDuration();
  printGoodbye();
}

// ____________RESPONS-FUNKSJONER_______________

void error_LED() {
  digitalWrite(green, HIGH);
  digitalWrite(red, HIGH);
  delay(2000);
  digitalWrite(green, LOW);
  digitalWrite(red, LOW);
}

void displayResponse() {
  response = circusESP32.read(bekreftKey, token);
  if (response == 1) {
    happy_beep();
  }
  if (response == 0) {
    sad_beep();
  }
  else if (response == 3) {
    error_LED();
  }
}

void checking() {
  int check = circusESP32.read(uselessButtonKey, token);
  while (check == 1) {
    Serial.println(txtWait);
    delay(500);
    check = circusESP32.read(uselessButtonKey, token);

    if (check == 0) {
      continue;
    }
  }
  displayResponse();
}

// ________COT-FUNKSJONER________

// Setter varighet til integer i minutter
void outDur() {
  outMinutes = 0;
  if (saveDur2 == 10) {
    outMinutes = saveDur1*60;    
  } 
  else {
    String outDuration = "";
    outDuration.concat(saveDur1);
    outDuration.concat(saveDur2);
    outMinutes = outDuration.toInt();
  }
}

// Funksjoner som sender informasjon til CoT
// Sender starttiden for bookingen til CoT
void outTimeSend() {
  outTime();
  circusESP32.write(timeKey, outHourVar.toInt(), token);
  circusESP32.write(minuttKey, outMinuteVar.toInt(), token);
}

// Sender varigheten til CoT
void outDurSend() {
  outDur();
  circusESP32.write(varighetKey, outMinutes, token);  
}

// Funksjon for å sende informasjonen til CoT
void toCOT() {
  outTimeSend();
  outDurSend();
  circusESP32.write(brukerKey, saveUser, token);
  circusESP32.write(romKey, saveRoom, token);
  circusESP32.write(uselessButtonKey, uselessButtonSignal, token);
}

// Lager en buzzerbeep
void beep() {
  ledcWriteTone(channel, freqd5);
  delay(50);
  ledcWriteTone(channel, 0);
  delay(1000);
}

// Buzzer beep med tre toner i synkende rekkefølge og blink fra rød LED
void sad_beep() {
  digitalWrite(red, HIGH);
  ledcWriteTone(channel, freqg5);
  delay(100);
  ledcWriteTone(channel, freqd5);
  delay(100);
  ledcWriteTone(channel, freqb4);
  delay(100);
  ledcWriteTone(channel, 0);
  digitalWrite(red, LOW);
}

// Buzzer beep med tre toner i synkende rekkefølge, uten LED
void sleep_beep() {
  ledcWriteTone(channel, freqg5);
  delay(200);
  ledcWriteTone(channel, freqd5);
  delay(200);
  ledcWriteTone(channel, freqb4);
  delay(200);
  ledcWriteTone(channel, 0);
}

// Buzzer beep med tre toner i stigende rekkefølge og blink fra grønn LED
void happy_beep() {
  digitalWrite(green, HIGH);
  ledcWriteTone(channel, freqb4);
  delay(100);
  ledcWriteTone(channel, freqd5);
  delay(100);
  ledcWriteTone(channel, freqg5);
  delay(100);
  ledcWriteTone(channel, 0);
  digitalWrite(green, LOW);
}
