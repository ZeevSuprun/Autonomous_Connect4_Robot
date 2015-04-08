//This function reads data from the QD sensor (line sensor)
#include <Arduino.h>

int readQD(int pin);
int readAnalogQRE(int pin);

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
  if (diff >= 200) {
      return HIGH;
  } else {
      return LOW;
  } 
}

//Code for the QRE1113 Analog board
//Outputs via the serial terminal - Lower numbers mean more reflected
int readAnalogQRE(int pin) {
  int QRE_Value = analogRead(pin);
  
  
  return QRE_Value;
  //return map(QRE_Value, 0, 1025, 0, 10000);
}

int binary_readAnalog(int pin) {
  int QRE_Value = analogRead(pin);
  int threshold = 200;
  
  if (QRE_Value >= threshold){
       return HIGH;
  } else {
       return LOW;
  }
}
