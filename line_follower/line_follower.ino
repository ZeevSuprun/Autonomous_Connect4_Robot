/******************************************************************
*******************************************************************
                      Function declarations
*******************************************************************
*******************************************************************/

#include "hoppers.h"
#include "sensing.h"
#include "maze_solving.h"
#include "non_mobility_motors.h"
#include <Servo.h>

//This function changes the direction the robot is moving in. (forwards, backwards, clockwise, counterclockwise)
// 0: Do nothing.  1: Go forwards.  2: Go in reverse. 3: Turn right in place.  4: turn left in place. 
void changeHeading(byte heading);

//This function make the robot move forwards for one 
void goForwards(class robotPosition &botPos);
void inchForwards(unsigned long duration);
void inchForwardsWithoutLines(unsigned long duration);
//Make the robot turn 90 degrees either clockwise or counterclockwise. 
void turn(class robotPosition &botPos, byte turnwise);
//this function makes the robot follow the path found by the empty_solver function.
void follow_directions(String  directions, class robotPosition &botPos);
//Interrupt Service Functions
/*
void doLeftEncoder ();
void doRightEncoder();
*/
/******************************************************************
*******************************************************************
                            Pins
*******************************************************************
*******************************************************************/
/***********************line following sensors*********************/
//element 0: reading leftmost QRE pin, 1: left QRE pin, 2: centre QRE pin, 3: right QRE pin, 4: rightmost QRE pin, 5: back QRE pin.
const int QRE_pin_array[] = {A6, A7, A5, A2, A4, A1};

//Stores the values detected by the line sensors.
//0: reading from leftmost QRE, 1: reading from left QRE, 2: centre QRE, 3: right QRE, 4: rightmost QRE. 5: front QRE pin 
int QRE_val_array[6];

/***********************navigation motor pins**************************/
//If leftDirectionPin is HIGH, left motor goes forwards. 
const int leftEnablePin = 4;
const int leftDirectionPin = 40;

//Same thing but for right motor. 
const int rightEnablePin = 2;
const int rightDirectionPin = 22;

/***********************Other Motor Pins**************************/
const int armEnablePin = 6;
const int armDirectionPin = 53;

const int hookEnablePin = 5;
const int hookDirectionPin = 52;

Servo gateServo;
const int servoPin = 9;
const byte closedAngle = 0;
const byte openAngle = 90;

/***********************dip switch pins**************************/
//An array of dipswitch pins, from left to right. 
int dipSwitchArray[] = {7, 8, 9, 10, 11, 13, 6};

/***********************wheel encoding pins**************************/
/*
//I need to remember to change this to leftEncoderCount
const int encoderPin = 3;

volatile unsigned int leftEncoderCount = 0;
volatile unsigned int rightEncoderCount = 0;
*/
/******************************************************************
*******************************************************************
                      Setup and loop
*******************************************************************
*******************************************************************/

robotPosition botPos;
hopperData leftHopper;
hopperData rightHopper;

String directions;
//The speed of the motor (255 is the maximum)
byte motorSpeed = 255;
//The speed the robot moves when inching forwards / backwards.
byte slowSpeed = 120;
//int tempVal;

void setup(){
/******************************************************************
*******************************************************************
               Setting Pin Modes
*******************************************************************
*******************************************************************/
   
    //mobility Motor pins 
    pinMode(leftDirectionPin, OUTPUT);
    pinMode(rightDirectionPin, OUTPUT);
    pinMode(leftEnablePin, OUTPUT);
    pinMode(rightEnablePin, OUTPUT);
    
    //Arm and hook pins.
    pinMode(armDirectionPin, OUTPUT);
    pinMode(armEnablePin, OUTPUT);

    pinMode(hookEnablePin, OUTPUT);
    pinMode(hookDirectionPin, OUTPUT);

    //line following sensor pins. 
    for (byte i = 0; i < 5; i++) {
        pinMode(QRE_pin_array[i], INPUT);
    }
    //dip switch pins. 
    for (byte i = 0; i < 7; i++) {
      pinMode(dipSwitchArray[i], INPUT);
      digitalWrite(dipSwitchArray[i], HIGH);
    }
    
    //gateServo.attach(servoPin);
    //gateServo.write(closedAngle);
    //Interrupt pin.
    //pinMode(encoderPin, INPUT);
    
/******************************************************************
*******************************************************************
               Done Setting Pin Modes
*******************************************************************
*******************************************************************/

    //Turning off motors so they don't seizure during upload. 
    changeHeading(0);
    analogWrite(hookEnablePin, 0);
    analogWrite(armEnablePin, 0);

    char arena[8][7];
    //it's [row][column]. row 0 is top row. column 0 is leftmost column. arena is a 8 row by 7 column grid

    //attachInterrupt(0, doLeftEncoder, CHANGE);
    //attachInterrupt(1, doRightEncoder, CHANGE);
    Serial.begin(9600);
    Serial.print("\nHello world. Serial monitor Start.\n");
/******************************************************************
*******************************************************************
 #7, locating an obstacle test: Should print out the entered data 
*******************************************************************
*******************************************************************/
    /*
    readSwitches(dipSwitchArray, leftHopper, rightHopper, arena);
    
    Serial.print(leftHopper.hopperRow);
    Serial.print(" ");
    Serial.print(leftHopper.hopperCol);
    Serial.print(" ");
    Serial.print(leftHopper.orientation);
    Serial.print("\n");
    Serial.print(rightHopper.hopperRow);
    Serial.print(" ");
    Serial.print(rightHopper.hopperCol);
    Serial.print(" ");
    Serial.print(rightHopper.orientation);
    
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            Serial.print(arena[i][j]);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
    Serial.print("------------------\n   0 1 2 3 4 5 6\n");
  */
/******************************************************************
*******************************************************************
         #2: Picking up a game ball. 
*******************************************************************
*******************************************************************/
    
    /*
    //testMotor(hookDirectionPin, hookEnablePin);
    Serial.print("delay starts\n");
    delay(5000);
    Serial.print("Delay over\n");
    
    extend_retract_hook(hookDirectionPin, oppositeDirPin, hookEnablePin);
    raise_lower_arm(armDirectionPin, armEnablePin);
    //changeHeading(0);
    //*/
    //testMotor(armDirectionPin, armEnablePin);

/******************************************************************
*******************************************************************
         #1 and #6 locomotion with obstacle avoidance.
*******************************************************************
*******************************************************************/    
    
    //filling the game board
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 7; j++)
        {
            arena[i][j] = '0';
        }
    }
    //void add_hoppers(int hop1_row, int hop1_col, int hop2_col, char arena[8][7]);
    //
    //directions =  solve(botPos.botRow, botPos.botCol, botPos.botDirection, 0, 0, arena);
    //Serial.print(directions);
    //Serial.print("we missed the boat");
    add_hoppers(0, 0, 3, arena);
    
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 7; j++) {
            Serial.print(arena[i][j]);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
    Serial.print("------------------\n0 1 2 3 4 5 6\n");

    botPos.botRow = 6;
    botPos.botCol = 0;
    botPos.botDirection = 'e';
    
    directions = blockedSolver(6, 0, botPos.botDirection, 4, 2, arena);
    //directions = empty_solver(0, 2, 'n', 5, 1);
    
    Serial.println(directions);  
    Serial.println("Delay start");
    delay(5000);
    Serial.println("delay over");
    follow_directions(directions, botPos);
    //inchForwardsWithoutLines(616);
    Serial.println("movement complete");
}

void loop() {
  //goForwards(botPos);
  changeHeading(0);
    /*
    for (byte i = 0; i < 6; i++) {
      QRE_val_array[i] =  readAnalogQRE(QRE_pin_array[i]);
      //To test what values the sensor array is reading. 
      Serial.print(QRE_val_array[i]);
      Serial.print(" ");
    }
    Serial.print("\n");
    delay(15);
    */
  //goForwards(botPos);

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

  //NOTE: FOR NOW, the comments below are wrong because of the way the motors are wired
  //This means that 
  if (heading == 0) {
      //Stay still. Stay silent. 
      analogWrite(rightEnablePin, 0);
      analogWrite(leftEnablePin, 0);
      //Serial.print("The robot should not be moving.");

  } else if (heading == 3) {
      //Go forwards. CLOCKWISE FOR NOW
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor forwards. 
      digitalWrite(rightDirectionPin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (heading == 4) {
      //Go backwards. (COUNTERCLOCKWISE FOR NOW)
      //Set left motor backwards. 
      digitalWrite(leftDirectionPin, LOW);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
            
  } else if (heading == 1) {
      //Turn counterclockwise. FORWARDS FOR NOW
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);
     //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (heading == 2) {
      //Turn clockwise. GO BACKWARDS FOR NOW.
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
  changeHeading(1);
  //Delay long enough so that it doesn't immediately stop. 
  delay(500);
  
  int leftOffset = 0;
  int rightOffset = 30;
  float turnFactor = 0.4;
  
  while (true) {
    //Populate sensor array.
    for (byte i = 0; i < 6; i++) {
        QRE_val_array[i] =  binary_readAnalog(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.print(" ");
    }
    Serial.print("\n");
    
    if ((QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[2] == HIGH and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) \ 
       or (QRE_val_array[0] == LOW and QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH and QRE_val_array[4] == LOW)) {
      //if 0 0 1 0 0  or 0 1 1 1 0 Go straight
        //left motor is more powerful than the right motor so it's "default" is -60.
        analogWrite(leftEnablePin, motorSpeed - leftOffset);
        analogWrite(rightEnablePin, motorSpeed - rightOffset);
        Serial.print("going forwards\n");
    } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == HIGH and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
      //if 0 1 - 0 0  then adjust left.
        analogWrite(leftEnablePin, turnFactor*(motorSpeed - leftOffset));
        analogWrite(rightEnablePin, motorSpeed - rightOffset);
        Serial.print("Adjusting left\n");
    } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[3] == HIGH and QRE_val_array[4] == LOW) {
        //if 0 0 - 1 0 then adjust right
        analogWrite(leftEnablePin, motorSpeed-leftOffset);
        analogWrite(rightEnablePin, turnFactor*(motorSpeed - rightOffset));
        Serial.print("Adjusting right\n");
        
    } else if (QRE_val_array[0] == HIGH and QRE_val_array[1] == LOW and QRE_val_array[2] == LOW and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
         //1 0 0 0 0
         //we're far off course, stop and turn left/counterclockwise.
         turnAdjust(4);
         changeHeading(1);
         
    } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[2] == LOW and QRE_val_array[3] == LOW and QRE_val_array[4] == HIGH) {
          //0 0 0 0 1
         //we're far off course to the right, stop and turn right/clockwise.
         turnAdjust(3);
         changeHeading(1);
    } 
    if (QRE_val_array[5] == HIGH) {
        //The robot's axis of rotation has crossed a line. 
        //Adjust the robot's position in botPos. 
        if (botPos.botDirection == 'n') {
            botPos.botRow -= 1;
        } else if (botPos.botDirection == 's') {
            botPos.botRow += 1;
        } else if (botPos.botDirection == 'e') {
            //This if statement accounts for the red line. 
            if(botPos.botCol == 2) {
              botPos.botCol += 2;
            } else {
              botPos.botCol += 1;
            }
        } else if (botPos.botDirection == 'w') {
            //Taking into account the red line. 
            if(botPos.botCol == 4) {
              botPos.botCol -= 2;
            } else {
              botPos.botCol -= 1;
            }
        }
        Serial.println("A LINE HAS BEEN CROSSED");
        break; 
    }
  }
  //change the robot's heading so that it's staying in place. 
  //changeHeading(0);
}

void inchForwards(unsigned long duration) {
  //This function makes the robot move forwards for duration milliseconds while following a line. 
  
  //store the motorSpeed so that we can change it back to normal afterwards.
  byte storedSpeed = motorSpeed;
  //the speed to move forwards at slowly.
  motorSpeed = slowSpeed;  
  changeHeading(1);
  
  int leftOffset = 0;
  int rightOffset = 15;
  float turnFactor = 0.4;
  
  unsigned long startTime = millis();
  while (true) {
    //if the 
    if (millis() - startTime >= duration) {
        break; 
    }
    
    //Populate sensor array.
    for (byte i = 0; i < 6; i++) {
        QRE_val_array[i] =  binary_readAnalog(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.print(" ");
    }
    Serial.print("\n");
    
    if ((QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[2] == HIGH and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) \ 
       or (QRE_val_array[0] == LOW and QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH and QRE_val_array[4] == LOW)) {
      //if 0 0 1 0 0  or 0 1 1 1 0 Go straight
        //left motor is more powerful than the right motor so it's "default" is -60.
        analogWrite(leftEnablePin, motorSpeed - leftOffset);
        analogWrite(rightEnablePin, motorSpeed - rightOffset);
        Serial.print("going forwards\n");
    } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == HIGH and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
      //if 0 1 - 0 0  then adjust left.
        analogWrite(leftEnablePin, turnFactor*(motorSpeed - leftOffset));
        analogWrite(rightEnablePin, motorSpeed - rightOffset);
        Serial.print("Adjusting left\n");
    } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[3] == HIGH and QRE_val_array[4] == LOW) {
        //if 0 0 - 1 0 then adjust right
        analogWrite(leftEnablePin, motorSpeed-leftOffset);
        analogWrite(rightEnablePin, turnFactor*(motorSpeed - rightOffset));
        Serial.print("Adjusting right\n");
    }
  }
  //change the robot's heading so that it's staying in place.
  changeHeading(0);
  //set motorSpeed back to normal. 
  motorSpeed = storedSpeed;
}

void inchForwardsWithoutLines(unsigned long duration) {
  //This function makes the robot drive forwards slowly for a set duration without line following.
  
  //store the motorSpeed so that we can change it back to normal afterwards.
  byte storedSpeed = motorSpeed;
  //the speed to move forwards at slowly.
  motorSpeed = slowSpeed;  
  changeHeading(1);
  
  int leftOffset = -30;
  int rightOffset = 30;
  //The offsets compensate for one motor being stronger or weaker than another. 
  analogWrite(leftEnablePin, motorSpeed - leftOffset);
  analogWrite(rightEnablePin, motorSpeed - rightOffset);

  unsigned long startTime = millis();
  while (true) {
    //if the 
    if (millis() - startTime >= duration) {
        break; 
    }
  }
  //change the robot's heading so that it's staying in place.
  changeHeading(0);
  //set motorSpeed back to normal
  motorSpeed = storedSpeed;
}

void inchBackwards(boolean toLine, unsigned long duration) {
  //This function makes the robot drive backwards slowly.
  //If toLine is true, it goes bacwards until it hits a line.
  //otherwise it just goes bacwards for duration seconds. 
  
  //store the motorSpeed so that we can change it back to normal afterwards.
  byte storedSpeed = motorSpeed;
  //the speed to move forwards at slowly.
  motorSpeed = slowSpeed;  
  changeHeading(2);
  
  int leftOffset = 0;
  int rightOffset = 0;
  //The offsets compensate for one motor being stronger or weaker than another. 
  analogWrite(leftEnablePin, motorSpeed - leftOffset);
  analogWrite(rightEnablePin, motorSpeed - rightOffset);

  if (toLine) {
    //we go backwards until we hit a line.
    while (true) {
      QRE_val_array[5] =  binary_readAnalog(QRE_pin_array[5]);
      if (QRE_val_array[5] == HIGH) {
        break; 
      }
    }
  } else{
  //if we are backing up for a duration, and not until we hit a line, then break out of the loop when the duration ends. 
    unsigned long startTime = millis();
    while (true) {
      if (millis() - startTime >= duration) {
          break; 
      }
    }
  }
  //change the robot's heading so that it's staying in place.
  changeHeading(0);
  //set motorSpeed back to normal
  motorSpeed = storedSpeed;
}

void turn(class robotPosition &botPos, byte turnwise)  {
    //Controls the robot's wheels to make it turn 90 degrees clockwise / counterlclockwise (depending on what turnways is)
    //3 --> clockwise, 4 --> counterclockwise.
    Serial.print("The turn function has been called");
    changeHeading(0);
    //Delay long enough so that it doesn't immediately stop turning. 
    changeHeading(turnwise);
    delay(500);
    
    
    //unsigned long turnDuration;
    //unsigned long startTime = millis();
    while(true) {
      //Keep turning until we're on a line. 
      
      //Read from sensors.
      for (byte i = 0; i < 6; i++) {
        QRE_val_array[i] =  binary_readAnalog(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.print(" ");
      }
      Serial.print("\n");
      if (QRE_val_array[0] == HIGH or QRE_val_array[1] == HIGH or QRE_val_array[2] == HIGH or QRE_val_array[3] == HIGH or  QRE_val_array[4] == HIGH) {
          changeHeading(0);
          Serial.print("The front sensors detected that we've completed a turn.\n");
          break;
      }
      /*
      if (millis() - startTime >= turnDuration) {
        break; 
      }*/
      
    }
    changeHeading(0);
    // 3 --> clockwise, 4 --> counterclockwise. 
    //Change the direction we're facing in botPos. 
    if(botPos.botDirection == 'n') {
        if (turnwise == 4) {
          botPos.botDirection == 'w';
        }else if (turnwise == 3) {
          botPos.botDirection == 'e';
        }
    } else if (botPos.botDirection == 'e') {
      if (turnwise == 4) {
          botPos.botDirection == 'n';
        }else if (turnwise == 3) {
          botPos.botDirection == 's';
        }
    } else if (botPos.botDirection == 's') {
      if (turnwise == 4) {
          botPos.botDirection == 'e';
        }else if (turnwise == 3) {
          botPos.botDirection == 'w';
        }
    } else if (botPos.botDirection == 'w') {
      if (turnwise == 4) {
          botPos.botDirection == 's';
        }else if (turnwise == 3) {
          botPos.botDirection == 'n';
        }
    }    
}

void turnAdjust(byte turnwise)  {
    //Controls the robot's wheels to make it turn until it finds the line again.
    //3 --> clockwise, 4 --> counterclockwise.
    Serial.print("The turn function has been called");
    changeHeading(0);
    //Delay long enough so that it doesn't immediately stop turning. 
    changeHeading(turnwise);
    delay(50);
        
    //unsigned long turnDuration;
    //unsigned long startTime = millis();
    while(true) {
      //Keep turning until we're on a line. 
      
      //Read from sensors.
      for (byte i = 0; i < 6; i++) {
        QRE_val_array[i] =  binary_readAnalog(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.print(" ");
      }
      Serial.print("\n");
      if (QRE_val_array[0] == HIGH or QRE_val_array[1] == HIGH or QRE_val_array[2] == HIGH or QRE_val_array[3] == HIGH or  QRE_val_array[4] == HIGH) {
          changeHeading(0);
          Serial.print("The front sensors detected that we've completed a turn.\n");
          break;
      }
      /*
      if (millis() - startTime >= turnDuration) {
        break; 
      }*/
    }
    changeHeading(0);
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
    changeHeading(0);    
}

/******************************************************************
*******************************************************************
                      interrupts
*******************************************************************
*******************************************************************/
/*
void doLeftEncoder () {
  leftEncoderCount += 1;
  if (leftEncoderCount == 65535) {
    leftEncoderCount = 0;
  }
  Serial.println(leftEncoderCount);
  
}

void doRightEncoder () {
  rightEncoderCount += 1;
  if (rightEncoderCount == 65535) {
    rightEncoderCount = 0;
  }
  Serial.println("Interrupt happened.");
  Serial.println(rightEncoderCount);
}
*/
