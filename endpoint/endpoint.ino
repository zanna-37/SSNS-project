#include <SoftwareSerial.h>
#include <Sleep_n0m1_custom.h> //See ../libraries/Sleep_n0m1_custom/README.md
#include <Math.h>

#define DEBUG //comment: off | uncomment: on
#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define PIN_LIGHT A5
#define PIN_TRIG 9
#define PIN_ECHO 10

#define THRESHOLD_LIGHT 500 //0-1024
#define THRESHOLD_DIFF_DISTANCE 1
#define TIMEOUT_KEEPALIVE_MS 10000
#define SLEEP_MS 4000
#define ID_ENDPOINT 7

SoftwareSerial XBee(2, 3); // RX, TX
Sleep sleep;

int lastLight = -1;
int lastDistance = -1;
long lastSend = -1;
long lastChange = -1;

void setup() {
  Serial.begin(9600);
  XBee.begin(9600);
    
  #ifdef DEBUG
    Serial.println();
    Serial.println("##############");
    Serial.println("#  STARTING  #");
    Serial.println("##############");
    Serial.println();
    Serial.print  ("TYPE      : ENDPOINT "); Serial.println(ID_ENDPOINT);
    Serial.println("PIN_LIGHT : " + String(PIN_LIGHT));
    Serial.println("PIN_TRIG  : " + String(PIN_TRIG));
    Serial.println("PIN_ECHO  : " + String(PIN_ECHO));
    Serial.println("--------------");
  #endif

    pinMode(PIN_LIGHT, INPUT);
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO, INPUT);
}

void loop() {
  bool light = getLight();
  int distance = getProximity();

  if( isTransmissionNeeded(light, distance) ) {
    sendData(ID_ENDPOINT, light, distance);
  }
  if( isKeepAliveNeeded() ) {
    sendKeepAlive(ID_ENDPOINT);
  }

  #ifdef DEBUG
    Serial.println();
  #endif

  XBee.flush();
  Serial.flush();
  sleep.pwrDownMode();
  sleep.sleepDelay(SLEEP_MS);

  #ifdef DEBUG
    Serial.println("Woken up");
  #endif
  
  //delay(SLEEP_MS);
}

bool getLight() {
  int lightValueAnalog = analogRead(PIN_LIGHT); //HIGHER: darker | LOWER: brighter

  #ifdef DEBUG
    Serial.println("[!] LIGHT");
    Serial.print("[.] Light: ");
    if (isBright(lightValueAnalog)) {
      Serial.print("BRIGHT (");
    } else {
      Serial.print("DARK (");
    }
    Serial.print( lightValueAnalog );
    Serial.println(")");
  #endif

  return isBright(lightValueAnalog);
}

bool isBright(int lightValueAnalog) {
  return lightValueAnalog < THRESHOLD_LIGHT;
}

int getProximity() {

  float duration, distance;
  //Reset
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);

  //Sense
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  //Calculations
  duration = pulseIn(PIN_ECHO, HIGH);
  distance = (duration / 2) * 0.0344;
 
  #ifdef DEBUG
    Serial.println("[!] PROXIMITY");
    Serial.print("[.] Distance (cm): ");
    Serial.println( distance );
  #endif

  return round(distance);
}

void sendData(int ID, bool light, int distance) {
  #ifdef DEBUG
    Serial.println("[.] Sending data...");
  #endif
  
  XBee.print("{ID:");
  XBee.print(ID);  
  XBee.print("L:");
  XBee.print(light);  
  XBee.print("P:");
  XBee.print(distance);
  XBee.println("}");
}

void sendKeepAlive(int ID) {
  #ifdef DEBUG
    Serial.println("[.] Sending KeepAlive...");
  #endif
  
  XBee.print("{ID:");
  XBee.print(ID);  
  XBee.print("KEEPALIVE");
  XBee.println("}");
}

bool isTransmissionNeeded(bool light, int distance) {
  if(light!=lastLight || abs(distance-lastDistance)>THRESHOLD_DIFF_DISTANCE) {
    #ifdef DEBUG
    #ifdef VERBOSE
      Serial.print("[.] Change in light: ");
      Serial.print(lastLight);
      Serial.print("-->");
      Serial.println(light);
      Serial.print("[.] Change in proximity: ");
      Serial.print(lastDistance);
      Serial.print("-->");
      Serial.println(distance);
    #endif
    #endif

    lastLight = light;
    lastDistance = distance;
    lastSend = getElapsedTime();

    return 1;
  } else {
    return 0;
  }
}

bool isKeepAliveNeeded() {

  #ifdef DEBUG
    #ifdef VERBOSE
      Serial.print("[.] Change in send timestamp: ");
      Serial.print(lastSend);
      Serial.print("-->");
      Serial.print(getElapsedTime());
      Serial.print(" (diff ");
      Serial.print(abs(getElapsedTime()-lastSend));
      Serial.println(")");
    #endif
    #endif
  if (abs(getElapsedTime()-lastSend) > TIMEOUT_KEEPALIVE_MS) {
    #ifdef DEBUG
    #ifdef VERBOSE
      Serial.print("[.] Change in send timestamp: ");
      Serial.print(lastSend);
      Serial.print("-->");
      Serial.print(getElapsedTime());
      Serial.print(" (diff ");
      Serial.print(abs(getElapsedTime()-lastSend));
      Serial.println(")");
    #endif
    #endif

    lastSend = getElapsedTime();

    return 1;
  } else {
    return 0;
  }
}

unsigned long getElapsedTime() {
  return sleep.WDTMillis();
}
