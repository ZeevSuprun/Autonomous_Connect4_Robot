
void raiseArm(int directionPin, int enablePin) {
  //This function lowers the arm and then raises it. 
    byte optimalArmSpeed = 255;  //arm speed between 0 and 255.
    int delaytime = 3500;         //time for arm to get into position in milliseconds. 
    
    //get it moving in the right direction. 
    Serial.println("Arm is raising");
    digitalWrite(directionPin, LOW);
    analogWrite(enablePin, optimalArmSpeed);
    //wait for it to move into position. 
    delay(delaytime);
    //Once it is in position, make it stop moving. 
    analogWrite(enablePin, 0);    
}

void lowerArm(int directionPin, int enablePin) {
    byte optimalArmSpeed = 255;  //arm speed between 0 and 255.
    int delaytime = 3500;         //time for arm to get into position in milliseconds. 
    //get it moving in the right direction. 
    Serial.print("arm is lowering");
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, optimalArmSpeed);
    //wait for it to get to the right position.
    delay(delaytime);
    //disable arm motor. 
    analogWrite(enablePin, 0);
}

void buttonHookMove(int directionPin, int enablePin, int buttonPin) {
   byte hookSpeed = 200;         //hook speed between 0 and 255.    
    //Make hook start extending. 
    Serial.println("hook extending");
    digitalWrite(directionPin, LOW);
    analogWrite(enablePin, hookSpeed);    
    //this variable will keep track of how long moving the hook takes. 
    int buttonVal;
    unsigned long moveTime = millis();

    //wait for it to move into position and activate the button.
    while (true) {
        //read the button pin and break when the button is pressed.
        buttonVal = digitalRead(buttonPin);
        if (buttonVal == 0) {
           digitalWrite(enablePin, 0);
           break; 
        }
    }
    Serial.println("Hook done extending");
    //Determine how long the hook was running and store that value.
    moveTime = millis() - moveTime;
    //make hook start retracting.
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, hookSpeed);    
    Serial.println("hook retracting");
    //delay until it's done retracting.
    
    //delay(moveTime);
    delay(2200);
    
    //stop retracting.
    digitalWrite(enablePin, 0);
}

