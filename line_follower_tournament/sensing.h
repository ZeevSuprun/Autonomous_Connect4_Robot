//This function reads data from the QD sensor (line sensor)
#include <Arduino.h>

int readAnalogQRE(int pin);


//Code for the QRE1113 Analog board
//Outputs via the serial terminal - Lower numbers mean more reflected
int readAnalogQRE(int pin) {
  int QRE_Value = analogRead(pin);
  
  
  return QRE_Value;
  //return map(QRE_Value, 0, 1025, 0, 10000);
}

int binary_readAnalog(int pin) {
  int QRE_Value = analogRead(pin);
  int threshold = 110;
  
  if (QRE_Value >= threshold){
       return HIGH;
  } else {
       return LOW;
  }
}
