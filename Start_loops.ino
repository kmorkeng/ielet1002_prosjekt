// Loop for standby tilstand
void standby_loop() {
  sleep_beep();
  delay(150);
  displayZeroOne();
  esp_light_sleep_start(); // Start lavstrømsmodus, vekkes med knapp1 eller knapp2
  happy_beep();
  seqIs = user; // Setter bruker-tilstanden som neste tilstand etter oppvåkning
  delay(150);
  displayZeroOne();
}

// Loop for user tilstand
void user_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit2 om knapp1 trykkes
    digit2 = digit2 + inc;
    if (digit2 == 7) { // Begrenser verdien til digit2
      digit2 = 1;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    saveUser = digit2;
    beep();
    delay(150);
    displayZeroOne();
    seqIs = veivalg;
  }
}

// Loop for veivalg tilstand
void veivalg_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) { // +1 på digit2 om knapp1 trykkes
    digit2 = digit2 + inc;
    if (digit2 == 3) { // Begrenser verdien til digit2
      digit2 = 1;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    beep();
    delay(150);
    if (digit2 == 1) {
      displayZeroOne();
      seqIs = room;
    }
    else if (digit2 == 2) {
      displayZeroOne();
      seqIs = enhetsvalg;
    }
  }
}
