//If leftForwardPin is HIGH, motor goes forwards. 
const int leftForwardPin = 12;
//If leftReversePin is HIGH, motor goes backwards.
const int leftReversePin = 8;
//If both are off, motor is off. 
const int leftEnablePin = 13;

//Same thing but for right motor. 
const int rightForwardPin = 2;
const int rightReversePin = 3;
const int rightEnablePin = 9;

//This variable determines what the 2 wheels are doing. 
int robot_heading = 2;
/* 0: Do nothing. 
 * 1: Go forwards. 
 * 2: Go in reverse. 
 * 3: Turn right in place. 
 * 4: turn left in place. 
*/

//This varaible controls the speed of both motors. 255 is the max. 
int motorSpeed = 255; 

int leftMotorState = 0;
int rightMotorState = 0;

void setup() {
 
  pinMode(leftForwardPin, OUTPUT);
  pinMode(leftReversePin, OUTPUT);
  
  pinMode(rightForwardPin, OUTPUT);
  pinMode(rightReversePin, OUTPUT); 
  
  robot_heading = 0;
}
void loop() {
 
  //Set the left motor to whatever it's supposed to be. 
  if (leftMotorState == 1) {
    //Set left motor forwards. 
    digitalWrite(leftForwardPin, HIGH);
    digitalWrite(leftReversePin, LOW);
    analogWrite(leftEnablePin, motorSpeed);
  } else if (leftMotorState == -1) {
    //Set left motor backwards. 
    digitalWrite(leftForwardPin, LOW);
    digitalWrite(leftReversePin, HIGH);
    analogWrite(leftEnablePin, motorSpeed);
  } else {
    //turn off left motor
    analogWrite(leftEnablePin, 0);
  }
  
  //Set the right motor to whatever it's supposed to be
  if (rightMotorState == 1) {
      //Set right motor forwards. 
      digitalWrite(rightForwardPin, HIGH);
      digitalWrite(rightReversePin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (rightMotorState == -1) {
      //Set right motor backwards
      digitalWrite(rightForwardPin, LOW);
      digitalWrite(rightReversePin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  } else {
      //Set right motor off. 
      analogWrite(rightEnablePin, 0);
  }
  
  //Read the robot_heading variable to 
  if (robot_heading == 0) {
      //Stay still. 
      leftMotorState = 0;
      rightMotorState = 0;
  } else if (robot_heading == 1) {
      //Go forwards. 
      leftMotorState = 1;
      rightMotorState = 1;             
  } else if (robot_heading == 2) {
      //Go backwards. 
      leftMotorState = -1;
      rightMotorState = -1;             
  } else if (robot_heading == 3) {
      //Turn counterclockwise. 
      leftMotorState = 1;
      rightMotorState = -1; 
  } else if (robot_heading == 4) {
      //Turn clockwise. 
      leftMotorState = -1;
      rightMotorState = 1; 
  }
}
