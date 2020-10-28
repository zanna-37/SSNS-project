#include <SoftwareSerial.h>

#define DEBUG //comment: off | uncomment: on
//#define VERBOSE //Only works with DEBUG set | comment: off | uncomment: on

#define SLEEP_MS 10

SoftwareSerial XBee(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  XBee.begin(9600);
      
  #ifdef DEBUG
    Serial.println();
    Serial.println("##############");
    Serial.println("#  STARTING  #");
    Serial.println("##############");
    Serial.println();
    Serial.println("TYPE: COORDINATOR");
    Serial.println("--------------");
  #endif
}

void loop() {

  String message = "";
  
  while (bytesAvailable() > 0) {
    String message = XBee.readStringUntil('\n'); //buffer is 64bytes, do not send a longer message without a \n in the middle
    #ifdef DEBUG
      Serial.print("[.] Received: ");
    #endif
    Serial.println(message);
    message="";
  }
  
  #ifdef DEBUG
  #ifdef VERBOSE
    Serial.println("[.] Zzz");
    Serial.println("");
  #endif
  #endif
  
  delay(SLEEP_MS); //do not sleep too long or the buffer (64bytes) will fill
}

int bytesAvailable() {
  int byteToRead = XBee.available();
  
  #ifdef DEBUG
    if ( byteToRead > 0 ) {
      Serial.print("[.] avail:");
      if (byteToRead < 10) Serial.print(" ");
      if (byteToRead < 100) Serial.print(" ");
      Serial.println(byteToRead);
    } else {
      #ifdef VERBOSE
        Serial.print("[.] avail:");
        if (byteToRead < 10) Serial.print(" ");
        if (byteToRead < 100) Serial.print(" ");
        Serial.println(byteToRead);
      #endif
    }
  #endif
  
  return byteToRead;
}
