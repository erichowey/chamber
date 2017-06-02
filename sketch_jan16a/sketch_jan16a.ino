#include <DFRobotDFPlayerMini.h>

#include <SoftwareSerial.h>

SoftwareSerial mySoftwareSerial(2, 3); // RX, TX
SoftwareSerial s7s(12, 13);

DFRobotDFPlayerMini myDFPlayer;

int backPin = 5;
int playPin = 6;
int forwardPin = 7;
int ledPin = 11;
int toggleVoxSwitch = 10;
int resetSwitch = 2;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output flickers

// Eyes Globals
int ledOff = 0;
int ledLow = 60;
int ledMed = 30;
int ledHigh = 255;  //WARNING: Max LED Voltage 3.3V. Setting higher will destory LEDs
int currentBrightness = 0;
int smoothDelay = 300;
int brightnesses[] = {ledOff, ledLow, ledHigh};
int lastToggleState = LOW;

// Audio Globals
int audioStatusPin = 4;
int queuedTrack = 1;
int currentTrack;

void setup() {
  
  pinMode(ledPin, OUTPUT);
  pinMode(toggleVoxSwitch, INPUT_PULLUP);
  pinMode(resetSwitch, INPUT_PULLUP);
  pinMode(backPin, INPUT_PULLUP);
  pinMode(playPin, INPUT_PULLUP);
  pinMode(forwardPin, INPUT_PULLUP);
  
  attachInterrupt(resetSwitch, stopAndResetBrightness, CHANGE);
  
  stopAndResetBrightness();
  audioSetup();
  s7sSetup();
}

void audioSetup() {

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(30);  //Set volume value (0~30).

  pinMode(audioStatusPin, INPUT);
  
}

void s7sSetup() {

  s7s.begin(9600);
  s7s.write(0x76);
  setS7s(queuedTrack);

}



void stopAndResetBrightness() {
  delay(50);
  setBrightnessSmooth(ledHigh);
}
  

void loop() {

  bool playing;
  
  if (isPlayerPlaying()) {
    chamberBlink();
    playing = true;
  }

  if (!isPlayerPlaying() and playing == true) {
    stopAndResetBrightness();
    playing = false;
  }

  if (isBackPressed() and debounceCheck()) {
    setQueuedTrack(1);
  }

  if (isPlayPressed() and debounceCheck()) {
    play();
  }

  if (isForwardPressed() and debounceCheck()) {
    setQueuedTrack(2);
  }
  
}

bool debounceCheck() {

  unsigned long lastTrigTime = millis() - lastDebounceTime;
    
  if (lastTrigTime > debounceDelay) {
    lastDebounceTime = millis();
    return true;
  }

  return false;
}

void setQueuedTrack(int player_direction) {

  if (player_direction == 1) {
    queuedTrack--;
    setS7s(queuedTrack);
  } else if (player_direction == 2) {
    queuedTrack++;
    setS7s(queuedTrack);
  }
  
}

void play() {
  
  myDFPlayer.play(queuedTrack);
  
}

int setS7s(int track) {

  s7s.write(0x76);
  s7s.print(track);
  
}

bool isBackPressed() {

  if (digitalRead(backPin) == LOW ) {
    return true;
  }

  return false;
}

bool isPlayPressed() {

  if (digitalRead(playPin) == LOW ) {
    return true;
  }

  return false;
}

bool isForwardPressed() {

  if (digitalRead(forwardPin) == LOW ) {
    return true;
  }

  return false;
}

bool isPlayerPlaying() {

  if (digitalRead(audioStatusPin) == HIGH) {
    return false;
  }

  return true;
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

