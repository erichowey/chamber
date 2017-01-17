int ledPin = 6;
int toggleVoxSwitch = 2;
bool isVoxToggled = false;
int lastToggleState = LOW;
int resetSwitch = 3;
int ledOff = 0;
int ledLow = 5;
int ledMed = 30;
int ledHigh = 170;  //WARNING: Max LED Voltage 3.3V. Setting higher will destory LEDs
int currentBrightness = 0;
int smoothDelay = 300;
int brightnesses[] = {ledOff, ledLow, ledHigh};

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers


void setup() {
  
  pinMode(ledPin, OUTPUT);
  pinMode(toggleVoxSwitch, INPUT_PULLUP);
  pinMode(resetSwitch, INPUT_PULLUP);
  pinMode(resetSwitch, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(resetSwitch), stopAndResetBrightness, CHANGE);
  
  stopAndResetBrightness();
}

void stopAndResetBrightness() {
  isVoxToggled = false;
  delay(50);
  setBrightnessSmooth(ledHigh);
}
  

void loop() {
  
  if (digitalRead(toggleVoxSwitch) == LOW) {
    
    unsigned long lastTrigTime = millis() - lastDebounceTime;
    
    if (lastTrigTime > debounceDelay) {
    
      if (isVoxToggled == true) {
        stopAndResetBrightness();
      } else {
        isVoxToggled = true;
      }
    
    }
    
    lastDebounceTime = millis();
  }
  
  if (isVoxToggled) {
    chamberBlink();
  }

}


void chamberBlink() {
  
  int brightness = brightnesses[random(0,3)];
  
  if (brightness != currentBrightness) {    
    setBrightnessSmooth(brightness);
    delay(100);
  }
  
}



bool setBrightness(int newBrightness) {
  
  if (newBrightness > ledHigh) return false;
  
  analogWrite(ledPin, newBrightness);
  currentBrightness = newBrightness;
  
  return true;  
}


bool setBrightnessSmooth(int newBrightness) {
  
  if (newBrightness > currentBrightness) {
    for (int i = currentBrightness; i <= newBrightness; i++) {
      setBrightness(i); 
      delayMicroseconds(smoothDelay);
    }
    return true;
  }
  
  if (newBrightness < currentBrightness) {
    for (int i = currentBrightness; i >= newBrightness; i--) {
      setBrightness(i); 
      delayMicroseconds(smoothDelay);
    }
    return true;
  }
}

bool setBrightnessHard(int newBrightness) {
  return setBrightness(newBrightness);
}

