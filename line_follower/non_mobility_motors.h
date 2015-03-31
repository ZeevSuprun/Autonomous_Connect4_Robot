void raise_lower_arm(int directionPin, int enablePin) {
  //This function lowers the arm and then raises it. 
    byte optimalArmSpeed = 200;  //arm speed between 0 and 255.
    int delaytime = 1100;         //time for arm to get into position in milliseconds. 
    
    //get it moving in the right direction. 
    Serial.print("Arm is raising");
    digitalWrite(directionPin, LOW);
    analogWrite(enablePin, optimalArmSpeed);
    //wait for it to move into position. 
    delay(delaytime);
    //pause briefly?
    //analogWrite(enablePin, 0);    
    //reverse the direction 
    Serial.print("arm is lowering");
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, optimalArmSpeed);
    //wait for it to go back to its initial position. 
    delay(delaytime);
    //disable arm motor. 
    analogWrite(enablePin, 0);
}

void raiseArm(int directionPin, int enablePin) {
  //This function lowers the arm and then raises it. 
    byte optimalArmSpeed = 200;  //arm speed between 0 and 255.
    int delaytime = 1100;         //time for arm to get into position in milliseconds. 
    
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
    byte optimalArmSpeed = 200;  //arm speed between 0 and 255.
    int delaytime = 1100;         //time for arm to get into position in milliseconds. 
    //get it moving in the right direction. 
    Serial.print("arm is lowering");
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, optimalArmSpeed);
    //wait for it to get to the right position.
    delay(delaytime);
    //disable arm motor. 
    analogWrite(enablePin, 0);
}



void testMotor (int directionPin, int enablePin, int dir, int duration) {
    //briefly actuate a motor in the "high" direction to see which direction the HIGH direction actually here. 
    digitalWrite(directionPin, dir);
    analogWrite(enablePin, duration);
    //delay(500);
    delay(1000);
    analogWrite(enablePin, 0);
}

void extend_retract_hook(int directionPin, int enablePin) {
  //This function extends and then retracts the hook.
  //HIGH moves the hook in, LOW moves the hook out.
    byte hookSpeed = 100;         //hook speed between 0 and 255.
    int delaytime = 500;         //time hook arm to get into position in milliseconds. 
    
    //get it moving in the right direction. 
    Serial.println("hook extending");
    digitalWrite(directionPin, LOW);
    analogWrite(enablePin, hookSpeed);
    //wait for it to move into position. 
    delay(delaytime);
    //pause briefly?
    //analogWrite(enablePin, 0);    
    //reverse the direction 
    Serial.println("hook is retracting");
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, hookSpeed);
    //wait for it to go back to its initial position. 
    delay(delaytime);
    //disable arm motor. 
    analogWrite(enablePin, 0);
}

void buttonHookMove(int directionPin, int enablePin, int buttonPin) {
   byte hookSpeed = 100;         //hook speed between 0 and 255.    
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
    //Determine how long the hook was running and store that value.
    moveTime = millis() - moveTime;
    //make hook start retracting.
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, hookSpeed);    

    //delay until it's done retracting.
    delay(moveTime);
    //stop retracting.
    digitalWrite(enablePin, 0);
}

