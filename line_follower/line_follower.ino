/******************************************************************
*******************************************************************
                      Function declarations
*******************************************************************
*******************************************************************/

#include "grid_following.h"
#include "sensing.h"

//This function changes the direction the robot is moving in. (forwards, backwards, clockwise, counterclockwise)
void changeHeading();

//Make the robot turn 90 degrees either clockwise or counterclockwise. 
void turn(class robotPosition &botPos, byte turnwise);
//this function makes the robot follow the path found by the empty_solver function.
void follow_directions(String  directions, class robotPosition &botPos);


//Code for following a line.
//Outputs via the serial terminal - Lower numbers mean more reflected
//3000 or more means nothing was reflected.

//HIGH --> black --> on the line
//LOW --> white --> off the line 

//The 5 line following pins. 

//Rightmost sensor is pin 9. 
//Right sensor is pin 6. 
//center sensor is pin 8 
//left sensor is pin 5
//leftmost sensor is pin 4

//Right now, leftmost and rightmost are both dead. 

//element 0: reading leftmost QRE pin, 1: left QRE pin, 2: centre QRE pin, 3: right QRE pin, 4: rightmost QRE pin. 
const int QRE_pin_array[] = {4, 5, 8, 6, 9};

//Stores the values detected by the line sensors.
//0: reading from leftmost QRE, 1: reading from left QRE, 2: centre QRE, 3: right QRE, 4: rightmost QRE. 
int QRE_val_array[5];


/******************************************************************
*******************************************************************
        The motor control pins and related constants
*******************************************************************
*******************************************************************/
//If leftDirectionPin is HIGH, motor goes forwards. 
const int leftDirectionPin = 2;
//If both are off, motor is off. 
const int leftEnablePin = 10;

//Same thing but for right motor. 
const int rightDirectionPin = 3;
const int rightEnablePin = 11;

//The speed of the motor (255 is the maximum)
int motorSpeed = 255;
global byte robot_heading = 0;
/* 0: Do nothing. 
 * 1: Go forwards. 
 * 2: Go in reverse. 
 * 3: Turn right in place. 
 * 4: turn left in place. 
*/


/******************************************************************
*******************************************************************
                      Setup and loop
*******************************************************************
*******************************************************************/
robotPosition botPos;
String directions;
int counter = 0;

void setup(){
    Serial.begin(9600);
    //setting pin modes.
   
     //Motor pins 
    pinMode(leftDirectionPin, OUTPUT);
    pinMode(rightDirectionPin, OUTPUT);
    pinMode(leftEnablePin, OUTPUT);
    pinMode(rightEnablePin, OUTPUT);
    //line pins. 
    for (byte i = 0; i < 5; i++) {
        pinMode(QRE_pin_array[i], INPUT);
    }

    botPos.botRow = 7;
    botPos.botCol = 2;
    botPos.botDirection = 'n';
    
    directions = empty_solver(7, 2, 'n', 5, 1);
    //Serial.print(directions);
    //Uncomment the line bellow to do direction following.
    //follow_directions(directions, botPos);

}
void loop() {
   //Uncomment this to test turning.
    //turn(botPos, 3);
    //delay(1000);
    //Serial.print("done turnin");
    
    //while (counter < 3) {
      goForwards(botPos);  
      Serial.print("I've crossed a line");
      //counter += 1;
    //}
    
    //delay(1000);
  
    
}

/******************************************************************
*******************************************************************
                      Function definitions
*******************************************************************
*******************************************************************/

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
      analogWrite(leftEnablePin, motorSpeed);

     //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (robot_heading == 4) {
      //Turn clockwise. 
      //Set left motor backwards. 
      digitalWrite(leftDirectionPin, LOW);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor forwards. 
      digitalWrite(rightDirectionPin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  }
} 

void goForwards(class robotPosition &botPos) {
  //This function makes the robot move forwards numTiles number of grid points using line following. 
  //WARNING, WILL NOT WORK ON RED LINE
  
  //Change the robot's heading so that it's moving forwards. 
  changeHeading(2);
  //Delay long enough so that it doesn't immediately stop. 
  delay(1000);
  
  while (true) {
    for (byte i = 1; i < 4; i++) {
        QRE_val_array[i] =  readQD(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.write(" ");
    }
    Serial.write("\n");
    //go forwards in a straight line. 
    if (QRE_val_array[1] == LOW and QRE_val_array[2] == HIGH and QRE_val_array[3] == LOW) {
    //if (true) {
        //Go straight
        //left motor is more powerful than the right motor so it's "default" is -60.
        analogWrite(leftEnablePin, motorSpeed - 20);
        analogWrite(rightEnablePin, motorSpeed);

        Serial.write("going forwards\n");
    } else if (QRE_val_array[1] == HIGH and QRE_val_array[3] == LOW) {
        //adjust left. 
        analogWrite(leftEnablePin, motorSpeed - 20 - 30);
        analogWrite(rightEnablePin, motorSpeed);
        Serial.write("Turning left\n");
    } else if (QRE_val_array[1] == LOW and QRE_val_array[3] == HIGH) {
        //adjust to right. 
        analogWrite(leftEnablePin, motorSpeed - 20);
        analogWrite(rightEnablePin, motorSpeed - 30);
        Serial.write("Turning right\n");

    } else if (QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH) {
        //we're crossing a line so we need to stop. 
        
        //Adjust the robot's position in botPos. 
        if (botPos.botDirection == 'n') {
            botPos.botCol -= 1;
        } else if (botPos.botDirection == 's') {
            botPos.botCol += 1;
        } else if (botPos.botDirection == 'e') {
            botPos.botRow += 1;
        } else if (botPos.botDirection == 'w') {
            botPos.botRow -= 1;
        }
        break;
    } else if (QRE_val_array[1] == LOW and QRE_val_array[2] == LOW and QRE_val_array[3] == LOW) {
        //we're off track, or are turning. 
        //If we keep turning then we'll go completely off, so this means start going forwards again. 
        //analogWrite(leftEnablePin, motorSpeed);
        //analogWrite(rightEnablePin, motorSpeed);
        
    } else {
        //Something's up: These cases shouldn't happen, or should only happen while turning. 
        //Therefore do nothing. 
        ;
    }
  }
  //change the robot's heading so that it's staying in place. 
  changeHeading(0);
}


void turn(class robotPosition &botPos, byte turnwise)  {
    //Controls the robot's wheels to make it turn 90 degrees clockwise / counterlclockwise (depending on what turnways is)
    //3 --> clockwise, 4 --> counterclockwise.
    changeHeading(turnwise);

    //Delay long enough so that it doesn't immediately stop turning. 
    delay(1000);
    
    while(true) {
      //Keep turning until we're on a line. 
      if ( QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH) {
          changeHeading(0);
          break;
      }
    }
    
    //For now, this is assuming 3 --> counterclockwise, 4 --> clockwise. 
    if(botPos.botDirection == 'n') {
        if (turnwise == 3) {
          botPos.botDirection == 'w';
        }else if (turnwise == 4) {
          botPos.botDirection == 'e';
        }
    } else if (botPos.botDirection == 'e') {
      if (turnwise == 3) {
          botPos.botDirection == 'n';
        }else if (turnwise == 4) {
          botPos.botDirection == 's';
        }
    } else if (botPos.botDirection == 's') {
      if (turnwise == 3) {
          botPos.botDirection == 'e';
        }else if (turnwise == 4) {
          botPos.botDirection == 'w';
        }
    } else if (botPos.botDirection == 'w') {
      if (turnwise == 3) {
          botPos.botDirection == 's';
        }else if (turnwise == 4) {
          botPos.botDirection == 'n';
        }
    }    
}

void follow_directions(String  directions, class robotPosition &botPos) {
    //input is a string of directions (which we get from the output of empty_solver().)
    //This function calls all the lower level functions to make the robot follow the specified directions. 
    int num_directions = (int)directions.length();
    for (int i = 0; i < num_directions; i++) {
        if (directions[i] == 'w') {
            goForwards(botPos);
        } else if (directions[i] == 'e') {
            turn(botPos, 3);
        } else if (directions[i] == 'q') {
            turn(botPos, 4); 
        }        
    }    
}

