//This sketch reads and prints data from all of the line sensors. 


//element 0: reading leftmost QRE pin, 1: left QRE pin, 2: centre QRE pin, 3: right QRE pin, 4: rightmost QRE pin. 
const int QRE_pin_array[] = {4, 5, 8, 6, 9};
//Stores the values detected by the line sensors.
//0: reading from leftmost QRE, 1: reading from left QRE, 2: centre QRE, 3: right QRE, 4: rightmost QRE. 
int QRE_val_array[5];

//declaring a function.
int readQD(int pin);

void setup() {
      Serial.begin(9600);

}

// the loop function runs over and over again forever
void loop() {
      for (byte i = 0; i < 5; i++) {
        QRE_val_array[i] =  readQD(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.print(" ");
    }
   Serial.print("\n");

}

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
  
  return diff;
}
