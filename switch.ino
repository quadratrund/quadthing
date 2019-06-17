uint8_t outputs[8];
uint8_t outState[8];
uint32_t safetyTimeout;
uint32_t lastNow;

void switch_setupInitOutputs() {
  int i;

  outputs[0] = D0;
  outputs[1] = D1;
  outputs[2] = D2;
  outputs[3] = D3;
  outputs[4] = D5;
  outputs[5] = D6;
  outputs[6] = D7;
  outputs[7] = D8;
  for (i = 0; i < 8; i++) {
    pinMode(outputs[i], i < 4 ? OUTPUT : INPUT);
    outState[i] = HIGH;
    digitalWrite(outputs[i], HIGH);
  }
  safetyTimeout = 0;
  lastNow = millis();
}

void switch_setupOutputsFromCfg() {
  int i;

  if (!cfg_feedbackMode) {
    for (i = 4; i < 8; i++) {
      pinMode(outputs[i], OUTPUT);
      outState[i] = HIGH;
      digitalWrite(outputs[i], HIGH);
    }
  } else {
    for (i = 0; i < 4; i++) {
      if (digitalRead(outputs[i + 4]) == HIGH) {
        outState[i] = LOW;
        digitalWrite(outputs[i], LOW);
      }
    }
  }
}

bool switch_outputs(String params) {
  safetyTimeout = millis() + cfg_safetyTimeout_ms;
  uint8_t id = getParamValue(params, "id").toInt();
  if (id >= cfg_outputCount) {
    return false;
  }
  String set = getParamValue(params, "set");
  uint8_t newState;
  if (set.equals("toggle")) {
    switch_toggleOutState(id);
  } else if (set.equals("high")) {
    outState[id] = HIGH;
    digitalWrite(outputs[id], outState[id]);
  } else if (set.equals("low")) {
    outState[id] = LOW;
    digitalWrite(outputs[id], outState[id]);
  } else if (set.equals("on")) {
    switch_setStateWithFeedback(id, HIGH);
  } else if (set.equals("off")) {
    switch_setStateWithFeedback(id, LOW);
  }
  return true;
}

uint8_t switch_getOutState(int id) {
  if (cfg_feedbackMode) {
    return digitalRead(outputs[id + 4]);
  } else {
    return !outState[id];
  }
}

void switch_toggleOutState(int id) {
  outState[id] = !outState[id];
  digitalWrite(outputs[id], outState[id]);
}

void switch_setStateWithFeedback(int id, uint8_t newState) {
  uint8_t currentState = switch_getOutState(id);
  if (currentState != newState) {
    switch_toggleOutState(id);
  }
}

void switch_loop() {
  uint32_t now = millis();
  int id;

  if (safetyTimeout && safetyTimeout > lastNow) {
    if (now < lastNow) {
      safetyTimeout -= lastNow;
      safetyTimeout += now;
    }
    if (now >= safetyTimeout) {
      safetyTimeout = 0;
      for (id = 0; id < cfg_outputCount; id++) {
        switch_setStateWithFeedback(id, LOW);
      }
    }
  }

  lastNow = now;
}
