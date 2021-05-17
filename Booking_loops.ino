// Loop for room tilstand
void room_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit2 om knapp1 trykkes
      digit2 = digit2 + inc;
    if (digit2 == 4) { // Begrenser verdien til digit2
      digit2 = 1;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
   }
   // Går videre til hour1 om knapp2 trykkes
   if (buttonState2 == HIGH) {
    saveRoom = digit2; // Lagrer verdi til global variabel
    beep();
    delay(150);
    displayZeroZero();
    seqIs = hour1;
  }
}

// Loop for hour1 tilstand
void hour1_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit1 om knapp1 trykkes
    digit1 = digit1 + inc;
    if (digit1 == 3) { // Begrenser verdien til digit1
      digit1 = 0;
    }
    sevseg1.setNumber(digit1);
    sevseg1.refreshDisplay();
    delay(150);
  }
  // Går videre til hour2 om knapp2 trykkes
  if (buttonState2 == HIGH) {
    saveHour1 = digit1; // Lagrer verdi til global variabel
    beep();
    delay(150);
    readyDisplay();
    seqIs = hour2;
  }
}

// Loop for hour2 tilstand
void hour2_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit2 om knapp1 trykkes
     digit2 = digit2 + inc;
    if ((digit1 == 2) && (digit2 >= 4)) { // Begrenser verdien til digit2 til max 3 om digit1 er 2
      digit2 = 0;
    }
    if (digit2 == 10) { // Begrenser verdien til digit2
      digit2 = 0;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    // Går videre til hour1 om minu1 trykkes
    saveHour2 = digit2; // Lagrer verdi til global variabel
    beep();
    delay(150);
    displayZeroZero();
    seqIs = minu1;
  }
}

// Loop for minu1 tilstand
void minu1_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit1 om knapp1 trykkes
    digit1 = digit1 + inc;
    if (digit1 == 6) { // Begrenser verdien til digit1
      digit1 = 0;
    }
    sevseg1.setNumber(digit1);
    sevseg1.refreshDisplay();
    delay(150);
 }
 // Går videre til minu2 om knapp2 trykkes
  if (buttonState2 == HIGH) {
    saveMinu1 = digit1; // Lagrer verdi til global variabel
    beep();
    delay(150);
    readyDisplay();
    seqIs = minu2;
  }
}

// Loop for minu2 tilstand
void minu2_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit2 om knapp1 trykkes
    digit2 = digit2 + inc;
    if (digit2 == 10) { // Begrenser verdien til digit2
      digit2 = 0;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  // Går videre til dur1 om knapp2 trykkes
  if (buttonState2 == HIGH) {
    saveMinu2 = digit2; // Lagrer verdi til global variabel
    beep();
    seqIs = dur1;
    delay(150);
    displayZeroZero();
  }
}

// Loop for dur1 tilstand
void dur1_loop() {
  buttonStatus();
  sevseg1.refreshDisplay();
  sevseg2.refreshDisplay();
  if (buttonState1 == HIGH) { // +1 på digit1 om knapp1 trykkes
   digit1 = digit1 + inc;
   if (digit1 == 10) { // Begrenser verdien til digit1
      digit1 = 0;
    }
    sevseg1.setNumber(digit1);
    sevseg1.refreshDisplay();
    delay(150);
  }
  // Går videre til dur2 om knapp2 trykkes
  if (buttonState2 == HIGH) {
    saveDur1 = digit1; // Lagrer verdi til global variabel
    beep();
    delay(150);
    readyDisplay();
    seqIs = dur2;
  }
}

// Loop for dur2 tilstand
void dur2_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit2 om knapp1 trykkes
    digit2 = digit2 + inc;
    if (digit2 <= 9) {
      sevseg2.setNumber(digit2);
      sevseg2.refreshDisplay();
      delay(150);
    }        
    if (digit2 == 11) { // Begrenser verdien til digit2
      digit2 = 0;
      sevseg2.setNumber(digit2);
      sevseg2.refreshDisplay();
      delay(150);
    }
    // Displayer bokstaver H for å indikere timer derson digit 2 er 10
    if (digit2 == 10) {
      h();
      delay(150);
    }
  }
  // Går videre til conc om knapp2 trykkes
  if (buttonState2 == HIGH) {
    saveDur2 = digit2; // Lagrer verdi til global variabel
    beep();
    delay(150);
    seqIs = conc;
  }
}

// Loop for conc tilstand
void conc_loop() {
  info(); // Skriver bookinginformasjon til seriemonitoren
  toCOT(); // Sender bookinginformasjon til CoT
  checking(); // Venter på respons fra backend, og gir tilbakemelding om responsen som mottas
  seqIs = standby; // Går tilbake til standby
}
