//If leftDirectionPin is HIGH, motor goes forwards. 
const int leftDirectionPin = 2;
//If both are off, motor is off. 
const int leftEnablePin = 10;

//Same thing but for right motor. 
const int rightDirectionPin = 3;
const int rightEnablePin = 11;

//This variable determines what the 2 wheels are doing. 
int robot_heading = 0;
/* 0: Do nothing. 
 * 1: Go forwards. 
 * 2: Go in reverse. 
 * 3: Turn right in place. 
 * 4: turn left in place. 
*/

//This varaible controls the speed of both motors. 255 is the max. 
int motorSpeed = 255; 

void setup() {
 
  pinMode(leftDirectionPin, OUTPUT);
  pinMode(rightDirectionPin, OUTPUT);
  
  //robot_heading = 1;
  
  digitalWrite(leftDirectionPin, LOW);
  digitalWrite(rightDirectionPin, LOW);
}
void loop() {
      
  
      //This should be going... straight? 
      analogWrite(leftEnablePin, motorSpeed - 10);

     //Set right motor backwards
      analogWrite(rightEnablePin, motorSpeed);

   
}

void changeHeading(byte heading) {
  //Read the global variable robot_heading and change the robot's speed and direction. 
 /* 0: Do nothing. 
 * 1: Go forwards. 
 * 2: Go in reverse. 
 * 3: Turn clockwise / right in place. 
 * 4: turn counterclockwise / left in place. 
*/
  robot_heading = heading; 

  if (robot_heading == 0) {
      //Stay still. 
      analogWrite(rightEnablePin, LOW);
      analogWrite(leftEnablePin, LOW);
      //Serial.write("The robot should not be moving.");

  } else if (robot_heading == 1) {
      //Go forwards. 
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor forwards. 
      digitalWrite(rightDirectionPin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (robot_heading == 2) {
      //Go backwards. 
      //Set left motor backwards. 
      digitalWrite(leftDirectionPin, LOW);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
            
  } else if (robot_heading == 3) {
      //Turn counterclockwise. 
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed - 60);

     //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (robot_heading == 4) {
      //Turn clockwise. 
      //Set left motor backwards. 
      digitalWrite(leftDirectionPin, LOW);
      analogWrite(leftEnablePin, motorSpeed - 60);

      //Set right motor forwards. 
      digitalWrite(rightDirectionPin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  }
} 

