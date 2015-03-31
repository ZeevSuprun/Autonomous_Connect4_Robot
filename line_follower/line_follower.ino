/******************************************************************
*******************************************************************
                      Function declarations
*******************************************************************
*******************************************************************/

#include "hoppers.h"
#include "sensing.h"
//#include "maze_solving.h"  //This is included in hoppers.h.
#include "non_mobility_motors.h"
#include <Servo.h>

//This function changes the direction the robot is moving in. (forwards, backwards, clockwise, counterclockwise)
// 0: Do nothing.  1: Go forwards.  2: Go in reverse. 3: Turn right in place.  4: turn left in place. 
void changeHeading(byte heading);

//This function make the robot move forwards for one 
void goForwards(class robotPosition &botPos);
void inchForwards(unsigned long duration);
void inchForwardsWithoutLines(unsigned long duration);
void inchBackwards(unsigned long duration);
void inchBackwardsWithoutLines(boolean toLine, unsigned long duration);
//Make the robot turn 90 degrees either clockwise or counterclockwise. 
void turn(class robotPosition &botPos, byte turnwise);
//Make a hard turn to get back on track when lost. 
void turnAdjust(byte turnwise);
//This function turns for a certain duration.
void controlledTurn(unsigned long turnDuration, byte turnwise);
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
const int leftEnablePin = 3;
const int leftDirectionPin = 52;

//Same thing but for right motor. 
const int rightEnablePin = 4;
const int rightDirectionPin = 53;

/***********************Other Motor Pins**************************/
const int armEnablePin = 5;
const int armDirectionPin = 50;

const int hookEnablePin = 6;
const int hookDirectionPin = 51;

Servo gateServo;
const int servoPin = 13;

/***********************dip switch pins**************************/
//An array of dipswitch pins, from left to right. 
//4, 5, 6, 7, 8, 9, 10

int dipSwitchArray[] = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
const int hookEndPin = 14;
const int rightTouchSensorPin = 15;
const int leftTouchSensorPin = 16;
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
byte motorSpeed = 200;
//The speed the robot moves when inching forwards / backwards.
byte slowSpeed = 120;

//hoppers = {fixed left, variable left, fixed right, variable right}
hopperData hoppers[4];

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
    for (byte i = 0; i < 10; i++) {
      pinMode(dipSwitchArray[i], INPUT);
      digitalWrite(dipSwitchArray[i], HIGH);
    }
    //setting up touch sensor thingies.
    pinMode(hookEndPin, INPUT);
    digitalWrite(hookEndPin, HIGH);
    pinMode(rightTouchSensorPin, INPUT);
    digitalWrite(rightTouchSensorPin, HIGH);
    pinMode(leftTouchSensorPin, INPUT);
    digitalWrite(leftTouchSensorPin, HIGH);

    
    byte closedAngle = 0;
    byte openAngle = 180;
    gateServo.attach(servoPin);
    //gate starts closed.
    gateServo.write(closedAngle);
    //Interrupt pin.
    //pinMode(encoderPin, INPUT);
/******************************************************************
*******************************************************************
               Done Setting Pin Modes
*******************************************************************
*******************************************************************/
    //Turning off motors
    changeHeading(0);
    analogWrite(hookEnablePin, 0);
    analogWrite(armEnablePin, 0);
    
    //Initializing the number of balls each hopper has. 
    hoppers[0].numBalls = 4;
    hoppers[1].numBalls = 7;
    hoppers[2].numBalls = 4;
    hoppers[3].numBalls = 7;
    //initializing the entry row and column of each hopper.
    hoppers[0].entryRow = 7;
    hoppers[0].entryCol = 0;
    hoppers[2].entryRow = 7;
    hoppers[2].entryCol = 6;

    
    char arena[8][7];
    //it's [row][column]. row 0 is top row. column 0 is leftmost column. arena is a 8 row by 7 column grid

    //attachInterrupt(0, doLeftEncoder, CHANGE);
    //attachInterrupt(1, doRightEncoder, CHANGE);
    Serial.begin(9600);
    Serial.print("\nHello world. Serial monitor Start.\n");
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

    //readSwitches(dipSwitchArray, hoppers[1], hoppers[3], arena);
    //void add_hoppers(int hop1_row, int hop1_col, int hop2_col, char arena[8][7]);
    add_hoppers(0, 0, 2, arena);
    
    //printArena(arena);
    
    botPos.botRow = 0;
    botPos.botCol = 0;
    botPos.botDirection = 's';
              
    //To start on the red lne use row = 6 or 7, col = 3, dir = 'n', to same row, 5.

    Serial.print("delay starts\n");
    delay(5000);
    Serial.print("Delay over\n");
    
    //distance to get to column X, in cm, from (0,3): DISTANCE=4.553571429*column+5.867857143. Time to move distance: time = 120.71*(Distance)+25.706
    //**************************************************************************************************************
    //              time to get to column X from (0, 3) is = 549.7*ColumnNumber+1699.96
    //**************************************************************************************************************
    //To get to column X need to move forwards from (0, 3) 
    //inchForwardsWithoutLines(616);
    //int colNum = 0;
    //inchForwards(550*colNum + 152);
    
    int testFunction = -3;
    
    /*
    printArena(arena);
    directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 6, 1, arena);
    directions += change_dir(botPos.botDirection, 'e');
    Serial.println(directions);
    */
    //change the variable testFunction to choose which huge block of code to test.
    if (testFunction == -1) {
        //do nothing
       //raiseArm(armDirectionPin, armEnablePin);
       //ARM: lowering the arm --> HIGH, raising the arm --> LOW
       //HOOK: Extending hook --> LOW retracting hook --> HIGH
       testMotor(hookDirectionPin, hookEnablePin, LOW, 2000);
       //testMotor(hookDirectionPin, hookEnablePin, HIGH);
      
    } else if (testFunction == -2) {
        //Personal test function 1: Print the directions, and nothing else.
        byte colNum = 2; 
        int offset = 1100; //old data said 1700
        inchForwards(550*colNum + offset);
        changeHeading(0);
        //inchForwardsWithoutLines(550*colNum + 1700);
        Serial.println("Done inching forwards");
        //Open the servo gate
        gateServo.write(openAngle);
        //wait for long enough for the ball to roll into the connect 4 board.
        delay(1000);
        gateServo.write(closedAngle);
/******************************************************************
********************************************************************/
    } else if (testFunction == -3) {
        int buttonVal;
        while (true) {
          //read the button pin and break when the button is pressed.
          buttonVal = digitalRead(hookEndPin);
          Serial.println(buttonVal);
          delay(15);
        }
    } else if (testFunction == -3) {
        unsigned long turnDuration = 1350;
        controlledTurn(turnDuration, 4);
        changeHeading(2);
        delay(2000);
        changeHeading(0);

    } else if (testFunction == 1 or testFunction == 6) {
       //We are testing controlled locomotion: going from point A to point B, going around hoppers. 
       //Find the path.
       directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 7, 2, arena);
       Serial.println(directions);
       //Follow the path.
       follow_directions(directions, botPos);
       //BE THE PATH.
    } else if (testFunction == 2) {
       //We are testing picking upa a game ball with the robot aligned to a hopper.
       //Step 1, lower the arm.;
       lowerArm(armDirectionPin, armEnablePin);
       //Step 2, extend and then retract the hook to get the ball.
       extend_retract_hook(hookDirectionPin, hookEnablePin);
       //step 3, raise the arm again.
       raiseArm(armDirectionPin, armEnablePin);
       //ball is now onboard.
    } else if (testFunction == 3) {
        //With the robot at (0, 3), deposit the ball in a given column.
        byte colNum = 2;
        Serial.println("Starting to inch forwards");
        inchForwards(550*colNum + 1700);
        //inchForwardsWithoutLines(550*colNum + 1700);
        Serial.println("Done inching forwards");
        //Open the servo gate
        gateServo.write(openAngle);
        //wait for long enough for the ball to roll into the connect 4 board.
        delay(1000);
        gateServo.write(closedAngle);
        //inch backwards with line following
        //fuck I didn't make that function yet.
    } else if (testFunction == 4) {
       //from any location, go to (0,3) and then align to a given column.
       directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 0, 2, arena);
       directions += change_dir(botPos.botDirection, 'e');
       Serial.println(directions);
       follow_directions(directions, botPos);
       //should now be at correct grid squre, now to inch forwards.
        byte colNum = 2;
        Serial.println("Starting to inch forwards");
        inchForwards(2*550*colNum + 1700);
        changeHeading(0);
        //inchForwardsWithoutLines(550*colNum + 1700);
        Serial.println("Done inching forwards");
        //Open the servo gate
        gateServo.write(openAngle);
        //wait for long enough for the ball to roll into the connect 4 board.
        delay(1000);
        gateServo.write(closedAngle);
       
    } else if (testFunction == 5) {
       //from any location, go to a hopper and align to it. Part marks for just aligning to it.  
    } else if (testFunction == 7) {
       //demonstrate that we can use the dip switches to tell the robot where the hoppers are.
       //while (true) {
           readSwitches(dipSwitchArray, hoppers[1], hoppers[3], arena);
          //print ALL the things.
         
          Serial.print(hoppers[1].hopperRow);
          Serial.print(" ");
          Serial.print(hoppers[1].hopperCol);
          Serial.print(" ");
          Serial.print(hoppers[1].orientation);
          Serial.print("\n");
          Serial.print(hoppers[3].hopperRow);
          Serial.print(" ");
          Serial.print(hoppers[3].hopperCol);
          Serial.print(" ");
          Serial.println(hoppers[3].orientation);
          
          printArena(arena);
      //}
    } else if (testFunction == 8) {
       //demonstrate "gameplay strategy". 
    }
    
    Serial.println("movement complete, you're in the loop now");
}

void loop() {
    //inchForwards(5000);
    //goForwards(botPos);
    changeHeading(0);
    //raiseArm(armDirectionPin, armEnablePin);
    //Serial.println("I'm trying");
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

  } else if (heading == 4) {
      //Go forwards. COUNTERCLOCKWISE FOR NOW
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor forwards. 
      digitalWrite(rightDirectionPin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (heading == 3) {
      //Go backwards. (CLOCKWISE FOR NOW)
      //Set left motor backwards. 
      digitalWrite(leftDirectionPin, LOW);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
            
  } else if (heading == 2) {
      //Turn counterclockwise. BACKWARDS FOR NOW
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);
     //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (heading == 1) {
      //Turn clockwise. GO FORWARDSS FOR NOW.
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
  
  int leftOffset = -30;
  int rightOffset = 30;
  float turnFactor = 0.4;
  
  unsigned long startTime = millis();
  while (true) {
    //if the time limit is hit.
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

void inchBackwards(unsigned long duration) {
  //This function makes the robot move backwards for duration milliseconds while following a line. 
  
  //store the motorSpeed so that we can change it back to normal afterwards.
  byte storedSpeed = motorSpeed;
  //the speed to move forwards at slowly.
  motorSpeed = slowSpeed;  
  changeHeading(2);
  
  int leftOffset = -45;
  int rightOffset = 30;
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
      //if 0 1 - 0 0  then adjust RIGHT (because backwards)
        analogWrite(leftEnablePin, (motorSpeed - leftOffset));
        analogWrite(rightEnablePin, turnFactor*(motorSpeed - rightOffset));
        Serial.print("Adjusting left\n");
    } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[3] == HIGH and QRE_val_array[4] == LOW) {
        //if 0 0 - 1 0 then adjust LEFT (because backwards)
        analogWrite(leftEnablePin, turnFactor*(motorSpeed-leftOffset));
        analogWrite(rightEnablePin, (motorSpeed - rightOffset));
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
  
  int leftOffset = -45;
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

void inchBackwardsWithoutLines(boolean toLine, unsigned long duration) {
  //This function makes the robot drive backwards slowly.
  //If toLine is true, it goes bacwards until it hits a line.
  //otherwise it just goes bacwards for duration seconds. 
  
  //store the motorSpeed so that we can change it back to normal afterwards.
  byte storedSpeed = motorSpeed;
  //the speed to move forwards at slowly.
  motorSpeed = slowSpeed;  
  changeHeading(2);
  
  int leftOffset = -45;
  int rightOffset = 30;
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

void controlledTurn(unsigned long turnDuration, byte turnwise) {
      //Controls the robot's wheels to make it turn 90 degrees clockwise / counterlclockwise (depending on what turnways is)
    //3 --> clockwise, 4 --> counterclockwise.
    Serial.print("The turn function has been called");
    changeHeading(0);
    
    byte storedSpeed = motorSpeed;
    //the speed to move forwards at slowly.
    motorSpeed = slowSpeed;  

    changeHeading(turnwise);
    
    unsigned long startTime = millis();
    while(true) {
      //turn for a certain duration.
      if (millis() - startTime >= turnDuration) {
        break; 
      }
    }
    motorSpeed = storedSpeed;
    changeHeading(0);    
}

void turn(class robotPosition &botPos, byte turnwise)  {
    //Controls the robot's wheels to make it turn 90 degrees clockwise / counterlclockwise (depending on what turnways is)
    //3 --> clockwise, 4 --> counterclockwise.
    Serial.println("THE TURN FUNCTION HAS BEEN CALLED, TURN STARTING NOW");
    changeHeading(0);
    //Delay long enough so that it doesn't immediately stop turning. 
    changeHeading(turnwise);
    delay(1500);
    
    
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
        //if (QRE_val_array[1] == HIGH or QRE_val_array[2] == HIGH or QRE_val_array[3] == HIGH) {
          changeHeading(0);
          Serial.println("WE ARE DONE TURNING, PUNY HUMAN");
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
    //In case we need to adjust things. 
    /*
    byte leftOffset = 0;
    byte rightOffset = 0;
    analogWrite(leftEnablePin, motorSpeed - leftOffset);
    analogWrite(rightEnablePin, motorSpeed - rightOffset);
    */  
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
