// Loop for enhetsvalg tilstand
void enhetsvalg_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit2 = digit2 + inc;
    if (digit2 == 5) {
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
      seqIs = varmekabler;
    }
    else if (digit2 == 2) {
      displayZeroOne();
      seqIs = vifte;
    }
    else if (digit2 == 3) {
      displayZeroOne();
      seqIs = vindu;
    }
    else if (digit2 == 4) {
      displayZeroOne();
      seqIs = termostat;
    }
  }
}

// Loop for temp1 tilstand
void temp1_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit1 = digit1 + inc;
    if (digit1 == 3) {
      digit1 = 0;
    }
    sevseg1.setNumber(digit1);
    sevseg1.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    beep();
    checkTarget1();
    delay(150);
    readyDisplay();
    seqIs = temp2;
  }
}

// Loop for temp2 tilstand
void temp2_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit2 = digit2 + inc;
    if (digit2 == 10) {
      digit2 = 0;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    beep();
    checkTarget2();
    displayZeroOne();
    delay(150);
    seqIs = standby;
  }
}

// Loop ofr varmekabler tilstand
void varmekabler_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit2 = digit2 + inc;
    if (digit2 == 4) {
      digit2 = 1;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    int state = digit2;
    beep();
    delay(150);
    if (state == 1) {
      target = 1;
      currGulv = circusESP32.read(gulvKey, token1);
      separate_display();
      delay(2000);
      seqIs = standby;
    }
    else if (state == 2) {
      target = 1;
      if (first == 0) {
        firstCheck();
      }
      toggleGulv();
      seqIs = standby;
    }
    else if (state == 3) {
      target = 1;
      displayZeroZero();
      seqIs = temp1;
    }
  }
}

// Loop for termostat tilstand
void termostat_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit2 = digit2 + inc;
    if (digit2 == 4) {
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
      target = 0;
      currVarme = circusESP32.read(varmeKey, token1);
      separate_display();
      delay(2000);
      seqIs = standby;
    }
    else if (digit2 == 2) {
      target = 0;
      if (first == 0) {
        firstCheck();
      }
      toggleVarme();
      seqIs = standby;
    }
    else if (digit2 == 3) {
      target = 0;
      displayZeroZero();
      seqIs = temp1;
    }
  }
}

// Loop for vifte tilstand
void vifte_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit2 = digit2 + inc;
    if (digit2 == 3) {
      digit2 = 1;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    vifteStatus = circusESP32.read(vifteKey, token1);
    hei = digit2;
    beep();
    delay(150);
    vifteAction();
    delay(2000);
  }
}

// Loop for vindu tilstand
void vindu_loop() {
  buttonStatus();
  if (buttonState1 == HIGH) {
    digit2 = digit2 + inc;
    if (digit2 == 3) {
      digit2 = 1;
    }
    sevseg2.setNumber(digit2);
    sevseg2.refreshDisplay();
    delay(150);
  }
  if (buttonState2 == HIGH) {
    vinduStatus = circusESP32.read(vinduKey, token1);
    heihei = digit2;
    beep();
    delay(150);
    vinduAction();
    delay(250);
  }
}
