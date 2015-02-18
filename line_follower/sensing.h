//This function reads data from the QD sensor (line sensor)
#include <Arduino.h>

int readQD(int pin);

int readQD(int pin){
  //Returns value from the QRE1113 
  //Lower numbers mean more reflecive
  //More than 3000 means nothing was reflected.
  pinMode(pin, OUTPUT );
  digitalWrite(pin, HIGH );  
  delayMicroseconds(10);
  pinMode(pin, INPUT );

  long time = micros();
  int diff;
  //time how long the input is HIGH, but quit after 3ms as nothing happens after that
  while (digitalRead(pin) == HIGH && micros() - time < 3000) { 
    diff = micros() - time;
  }
  //return diff;
  if (diff >= 500) {
      return HIGH;
  } else {
      return LOW;
  } 
}

