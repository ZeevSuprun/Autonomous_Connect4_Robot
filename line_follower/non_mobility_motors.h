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

void testMotor (int directionPin, int enablePin) {
    //briefly actuate a motor in the "high" direction to see which direction the HIGH direction actually here. 
    digitalWrite(directionPin, HIGH);
    analogWrite(enablePin, 200);
    //delay(500);
    delay(500);
    analogWrite(enablePin, 0);
}

/*
void extend_retract_hook(int directionPin, int oppositeDirPin, int enablePin) {
  //This function extends the hook and then retracts it again. 
    byte hookSpeed = 255;
    int delaytime = 28000;  //time for arm to get into position in milliseconds. 
    Serial.print("hook is extending");
    //get it moving in the right direction. (extend)
    digitalWrite(directionPin, HIGH);
    digitalWrite(oppositeDirPin, LOW);
    analogWrite(enablePin, hookSpeed);
    //wait for it to move into position. 
    delay(delaytime);
    //pause briefly?
    //analogWrite(enablePin, 0);    
    //reverse the direction 
    Serial.print("Hook is retracting");
    digitalWrite(directionPin, LOW);
    digitalWrite(oppositeDirPin, HIGH);
    analogWrite(enablePin, hookSpeed);
    //wait for it to go back to its initial position. 
    delay(delaytime);
    //disable arm motor. 
    analogWrite(enablePin, 0);    
}*/
