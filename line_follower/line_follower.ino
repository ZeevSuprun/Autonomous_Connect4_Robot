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

//This function make the robot move forwards for one grid space
void goForwards(class robotPosition &botPos);
//These four functions make the robot move forwards / backwards with / without line following
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
//Interrupts if we flip a switch when the robot gets lost.

//This function makes the robot exit a hopper and sets exitRow, exitCol and exitDir to the robot's new position.
void exitHopper(hopperData hoppers[4], byte index, byte &exitRow, byte &exitCol, char &exitDir);
//this function makes the robot enter a hopper and returns 0 if successful or -1 if the robot was paused while the function was running.
int enterHopper(hopperData hoppers[4], byte index);


//void gotLost();


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
const int leftDirectionPin = 52;

//Same thing but for right motor. 
const int rightEnablePin = 3;
const int rightDirectionPin = 53;

/***********************Other Motor Pins**************************/
const int armEnablePin = 5;
const int armDirectionPin = 50;

const int hookEnablePin = 6;
const int hookDirectionPin = 51;

Servo gateServo;
const int servoPin = 13;
//angle at which the 
byte closedAngle = 0;
byte openAngle = 180;

/***********************dip switch pins**************************/
//An array of dipswitch pins, from left to right. 
//4, 5, 6, 7, 8, 9, 10

int dipSwitchArray[] = {30, 31, 32, 33, 34, 35, 36, 37, 38, 2};

const int hookEndPin = 49;    //This pin is 0 when the button is pressed and 1 otherwise. 
const int rightTouchSensorPin = 15;
const int leftTouchSensorPin = 16;
/***********************hopper detection pins**************************/
//These pins connect to the zoidberg claws. 
const int leftHopperDetectPin = 48;
const int rightHopperDetectPin = 47;

int leftHopDetect, rightHopDetect;
char endDir;
/***********************interrupt pins**************************/

//const int gotLostPin = 2;

/******************************************************************
*******************************************************************
                      Setup and loop
*******************************************************************
*******************************************************************/

robotPosition botPos;

String directions;
//The speed of the motor (255 is the maximum)
//byte motorSpeed = 255;
byte motorSpeed = 255;
//The speed the robot moves when inching forwards / backwards.
byte slowSpeed = 255;

//hoppers = {fixed left, variable left, fixed right, variable right}
hopperData hoppers[4];
//it's [row][column]. row 0 is top row. column 0 is leftmost column. arena is a 8 row by 7 column grid
char arena[8][7];

//The number of balls placed in the game board. (starts at 0. 
byte ballCount;
//the column number that we want to put the ball in.
byte colNum;
//This is the index of the hopper we're going to iin 
byte hopperChosen;

//if the enable switch (dip switch 10) is off, (enable = 1), then stop iterating through the loop until it's turned on again.
int enable;

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
    //hopper detection pins. 
    pinMode(leftHopperDetectPin, INPUT);
    digitalWrite(leftHopperDetectPin, HIGH);
    
    pinMode(rightHopperDetectPin, INPUT);
    digitalWrite(rightHopperDetectPin, HIGH);
    
    //setting up touch sensor thingies.
    pinMode(hookEndPin, INPUT);
    digitalWrite(hookEndPin, HIGH);
    
    gateServo.attach(servoPin);
    //gate starts closed.
    gateServo.write(closedAngle);
    //Interrupt pin.
    /*    
    pinMode(gotLostPin, INPUT);
    digitalWrite(gotLostPin, HIGH);
    attachInterrupt(0, gotLost, FALLING);
    */  
/******************************************************************
*******************************************************************
               Done Setting Pin Modes
*******************************************************************
*******************************************************************/
    //Turning off motors
    changeHeading(0);
    analogWrite(hookEnablePin, 0);
    analogWrite(armEnablePin, 0);
    
    //Initializing the number of balls each hopper has. (other 3 hoppers set to 0 balls so the robot doesn't try to go there).
    hoppers[0].numBalls = 4;
    hoppers[1].numBalls = 0; //7;
    hoppers[2].numBalls = 4;
    hoppers[3].numBalls = 0; //7;
    //initializing the entry row and column of each hopper.
    hoppers[0].entryRow = 6;
    hoppers[0].entryCol = 1;
    hoppers[0].entryDir = 'w';
    hoppers[2].entryRow = 6;
    hoppers[2].entryCol = 5;
    hoppers[2].entryDir = 'e';

    Serial.begin(9600);
    Serial.println("\nHello world. Serial monitor Start.");
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
    
    printArena(arena);
    
    botPos.botRow = 6;
    botPos.botCol = 6;
    botPos.botDirection = 'n';
    
    //directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 0, 3, arena, endDir);
    //directions += change_dir(botPos.botDirection, 'e');
    Serial.println(directions);
        
    //To start on the red line use row = 7, col = 3, dir = 'n', to same row, 5.
    
    
    //distance to get to column X, in cm, from (0,3): DISTANCE=4.553571429*column+5.867857143. Time to move distance: time = 120.71*(Distance)+25.706
    //**************************************************************************************************************
    //              time to get to column X from (0, 3) is = 549.7*ColumnNumber+1699.96
    //**************************************************************************************************************
    //To get to column X need to move forwards from (0, 3) 
    //inchForwardsWithoutLines(616);
    //int colNum = 0;
    //inchForwards(550*colNum + 152);
    
    int testFunction = 5;

    Serial.print("delay starts\n");
    delay(5000);
    Serial.print("Delay over\n");
    

    //change the variable testFunction to choose which huge block of code to test.
    if (testFunction == -1) {
        /*********************** -1: inching *****************************/
      
        /*
        void inchForwards(unsigned long duration);
        void inchForwardsWithoutLines(unsigned long duration);
        void inchBackwards(unsigned long duration);
        void inchBackwardsWithoutLines(boolean toLine, unsigned long duration);
        */
        //inchForwardsWithoutLines(2000);
        //inchForwards(2000);
        //inchBackwardsWithoutLines(false, 2000);
        //delay(500);
        //inchBackwardsWithoutLines(true, 1000);
        //turn(botPos, 3);
        
        directions = "weww";
        //follow_directions(directions, botPos);
        
        //changeHeading(1);
      
    }else if (testFunction == -5) {
       /*********************** -5: testMotor *****************************/
      
       //do nothing
       //raiseArm(armDirectionPin, armEnablePin);
       //ARM: lowering the arm --> HIGH, raising the arm --> LOW
       //HOOK: Extending hook --> LOW retracting hook --> HIGH
       testMotor(armDirectionPin, armEnablePin, HIGH, 3000);
       //delay(2000);
       Serial.println("Aaaaaaarm");
       testMotor(armDirectionPin, armEnablePin, HIGH, 3000);

       //delay(2000);
       //testMotor(armDirectionPin, armEnablePin, LOW, 3000);
       //testMotor(hookDirectionPin, hookEnablePin, HIGH);
        gateServo.write(openAngle);
        //wait for long enough for the ball to roll into the connect 4 board.
        delay(200);
        gateServo.write(closedAngle);
      
    } else if (testFunction == -2) {
      /*********************** -2: Misc *****************************/
      int buttonVal;
      while(true) {
        buttonVal = digitalRead(hookEndPin);
        if (buttonVal == 0) {
           Serial.println("the hook button was pressed");
        }
      }

    } else if (testFunction == -3) {
      /*********************** -3: buttonHookMove *****************************/
        Serial.println("hook is actuating");
        buttonHookMove(hookDirectionPin, hookEnablePin, hookEndPin);
        
    } else if (testFunction == -4) {
        //Do a controlled turn. 
        unsigned long turnDuration = 780/2;
        controlledTurn(turnDuration, 4);
        /*
        delay(1000);
        controlledTurn(turnDuration, 4);
        delay(1000);
        controlledTurn(turnDuration, 3);
        delay(1000);
        controlledTurn(turnDuration, 3);
        delay(1000);
        */
        
        changeHeading(0);

    } else if (testFunction == 1) {
        /*********************** 1: Follow Directions *****************************/
        
        //We are testing controlled locomotion: going from point A to point B, going around hoppers. 
        //Find the path.
        directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 3, 6, arena, endDir);
        Serial.println(directions);
        //Follow the path.
        follow_directions(directions, botPos);
        //BE THE PATH.
    } else if (testFunction == 2) {
        /*********************** 2: Pick a ball (including arm movement) ************************************/
        
        //We are testing picking upa a game ball with the robot aligned to a hopper.
        //Step 1, lower the arm.
        lowerArm(armDirectionPin, armEnablePin);
        //Step 2, extend and then retract the hook to get the ball.
        //buttonHookMove(hookDirectionPin, hookEnablePin, hookEndPin);
        delay(5000);
        //step 3, raise the arm again.
        raiseArm(armDirectionPin, armEnablePin);
        //ball is now onboard.
    } else if (testFunction == 3) {
      /*********************** 3: Deposit a game ball from (0,2) ************************************/
      
        //With the robot at (0, 2), deposit the ball in a given column.
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
    } else if (testFunction == 4) {
        /*********************** 4: From any location, deposit a game ball. ************************************/
        
        //from any location, go to (0,2) and then align to a given column.
        directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 0, 2, arena, endDir);
        directions += change_dir(endDir, 'e');
        Serial.println(directions);
        follow_directions(directions, botPos);
        //should now be at correct grid squre, now to inch forwards.
        /*
        byte colNum = 2;
        Serial.println("Starting to inch forwards");
        inchForwards(2*550*colNum + 1700);
        */
        changeHeading(0);

        Serial.println("Done inching forwards");
        //Open the servo gate
        gateServo.write(openAngle);
        //wait for long enough for the ball to roll into the connect 4 board.
        delay(1000);
        gateServo.write(closedAngle);
       
    } else if (testFunction == 5) {
      /*********************** 5: Find a hopper (go to a hopper entry point)********************************************/
        botPos.botRow = 0;
        botPos.botCol = 4;
        botPos.botDirection = 'e';

        byte nearHop;
        directions = findNearestHopper(hoppers, botPos, arena, nearHop);
        Serial.println(directions);
        Serial.println(nearHop);
        //follow_directions(directions, botPos);

    } else if (testFunction == 6) {
      /*********************** 6: From a hopper entry point, align to it.  ************************************/
      Serial.println("we're entering a hopper");
      botPos.botRow = 6;
      botPos.botCol = 1;
      botPos.botDirection = 'w';
    
      enterHopper(hoppers, 0);

       //from any location, go to a hopper and align to it. Part marks for just aligning to it.  
    } else if (testFunction == 7) {
      /*********************** 7: exit from a hopper  ************************************/
      exitHopper(hoppers, hopperChosen, botPos.botRow, botPos.botCol, botPos.botDirection);
 
       
    } else if (testFunction == 8) {
     /*********************** 8: demonstrate that we can use the dip switches to tell the robot where the hoppers are.  ************************************/

       while (true) {
           readSwitches(dipSwitchArray, hoppers[1], hoppers[3], arena);
        }    //print ALL the things.
         
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
      
    }

    ballCount = 0;
    Serial.println("movement complete, you're in the loop now");
}


void loop() {
    //inchForwards(5000);
    //goForwards(botPos);
    changeHeading(0);
    
    /*
    //leftHopDetect = digitalRead(leftHopperDetectPin);
    rightHopDetect = digitalRead(hookEndPin);
    //rightHopDetect = digitalRead(rightHopperDetectPin);
    //Serial.print(leftHopDetect);
    //Serial.print(" ");
    Serial.println(rightHopDetect);
    delay(15);
    */
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
    
    /*
    Main gameplay function:
    1. Find the nearest non-empty hopper.
    2. Go to the designated entry point for that hopper.
    3. Enter the hopper.
    4. Retrieve a ball.
    5. Back up until we get back to the designated entry point. 
    6. Go to the gameboard.
    7. Determine where to deposit the ball. 
    8. Deposit the ball. 
    9. Go back to a grid point and know where that grid point is.
    10. Go back to step 1. 
    */
    enable = digitalRead(dipSwitchArray[9]);
    enable = -1;
    if (enable == 1) {
        //if enable is 1, the switch is off, and we are on pause and not moving.
        changeHeading(0);
        //couldn't hurt to turn off the arm, too, could it?
        analogWrite(hookEnablePin, 0);
        analogWrite(armEnablePin, 0);

        //Set the robot position to the right place (maybe comment this bit out for testing)
        botPos.botRow = 7;
        botPos.botCol = 3;
        //This might need to be 'n'.
        botPos.botDirection = 'w';
        //Change this delay time to be whatever it's supposed to be.

    } else if(enable == 0) {
        //The enable switch was just flipped to unpause, delay a few seconds to give us a chance to get out of the robot's way.
        //I could accomplish the same thing with an interrupt on rising edge maybe? This way should work too.
        delay(5000);
    } 
    while(enable == 0) {
        //If the enable is equal to 0, the switch is on and we are moving as normal.
        
        /*********************** #1: Find the nearest non empty hopper. **************************/
        directions = "";
        directions = findNearestHopper(hoppers, botPos, arena, hopperChosen);
        //directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 6, 1, arena, endDir);

        Serial.println(directions);
        //directions now holds the path to the nearest hopper, hopperChosen is the index of hoppers we're going to. 
        
        /*********************** #2: Go to the nearest non-empty hopper **************************/
        follow_directions(directions, botPos);
        //the robot is now at the designated grid point for the nearest non-empty hopper. 
        
        //Poll the enable. 
        enable = digitalRead(dipSwitchArray[9]);
        if (enable == 1) {
           break; 
        }
         
        /*********************** #3: Enter the hopper. ********************************************/        
        //Move into the hopper.
        enterHopper(hoppers, hopperChosen);
        //Poll the enable. 
        enable = digitalRead(dipSwitchArray[9]);
        if (enable == 1) {
           break; 
        }        
        
        /*********************** #4: retrieve the ball. ********************************************/
        //Step 1, lower the arm.;
        lowerArm(armDirectionPin, armEnablePin);
        //Step 2, extend and then retract the hook to get the ball.
        //buttonHookMove(hookDirectionPin, hookEnablePin, hookEndPin);
        //step 3, raise the arm again.
        raiseArm(armDirectionPin, armEnablePin);
        //ball is now onboard.
        
        /*********************** #5: Back out of the hopper. ********************************************/
        
        //exit the hopper and put the robot's new position into botPos.
        exitHopper(hoppers, hopperChosen, botPos.botRow, botPos.botCol, botPos.botDirection);
        //Poll for off switch. 
        enable = digitalRead(dipSwitchArray[9]);
        if (enable == 1) {
           break; 
        }
        
        /***********************#6: Going to the gameboard. ********************************************/
        //from any location, go to (0,2) and then align to a given column.
        directions = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, 0, 2, arena, endDir);
        directions += change_dir(botPos.botDirection, 'e');
        Serial.println(directions);
        follow_directions(directions, botPos);
        //should now be at correct grid squre, now to inch forwards.
        
        /***********************# 7, Determine where to deposit the ball.  *****************************/
        colNum = whereToPlace(ballCount);
        
        /***********************# 8.part 1 Align to the right column.  *****************************/
        //Poll for off switch. 
        enable = digitalRead(dipSwitchArray[9]);
        if (enable == 1) {
           break; 
        }
        
        Serial.println("Starting to inch forwards");
        inchForwards(550*colNum + 1700);            //This line is suspect, we need to check it again to make sure we can reliably go to the right column. (or one beside it)
        changeHeading(0);
        Serial.println("Done inching forwards");
        
        /***********************# 8.part 2 Depositing the ball  *****************************/
        //Open the servo gate
        gateServo.write(openAngle);
        //wait for long enough for the ball to roll into the connect 4 board.
        delay(500);
        gateServo.write(closedAngle);
        //increment ballcount. 
        ballCount += 1;
    
        /***********************#9: Go back to a grid point while knowing what that grid point is.*****************************/
        //Poll for off switch. 
        enable = digitalRead(dipSwitchArray[9]);
        if (enable == 1) {
           break; 
        }
        
        goForwards(botPos);
        changeHeading(0);
        botPos.botCol = 4;
        
    }    
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
      //Go backwards. (counterCLOCKWISE FOR NOW)
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
  //This function makes the robot move forwards until it hits a grid line.
  //WARNING, WILL NOT WORK ON RED LINE
  
  //Change the robot's heading so that it's moving forwards. 
  changeHeading(1);
  //Delay long enough so that it doesn't immediately stop. 
  delay(500);
  
  int leftOffset = 0;
  int rightOffset = 0;  //30
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
  int rightOffset = 0;
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
  
  int leftOffset = 0;
  int rightOffset = 0;
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
  
  int leftOffset = 0;
  int rightOffset = 0;
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

void controlledTurn(unsigned long turnDuration, byte turnwise) {
     //Controls the robot's wheels to make it turn 90 degrees clockwise / counterlclockwise (depending on what turnways is)
    //3 --> clockwise, 4 --> counterclockwise.
    Serial.println("The turn function has been called");
    changeHeading(0);
    
    byte storedSpeed = motorSpeed;
    //the speed to do a controlled turn. (at this speed , it takes 945 seconds to do a 90 degree turn.
    motorSpeed = 255;  

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
    delay(1000);
    
    
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
      //if (QRE_val_array[0] == HIGH or QRE_val_array[1] == HIGH or QRE_val_array[2] == HIGH or QRE_val_array[3] == HIGH or  QRE_val_array[4] == HIGH) {
        if (QRE_val_array[1] == HIGH or QRE_val_array[2] == HIGH or QRE_val_array[3] == HIGH) {
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
        //poll the enable button, if enable is off then return early.
        enable = digitalRead(dipSwitchArray[9]);
        if (enable == 1) {
             break; 
        }
        
    }
    changeHeading(0);    
}

/******************************************************************
*******************************************************************
                 interrupt Service Routines
*******************************************************************
*******************************************************************/
/*
void gotLost() {
  //turn off all motors, change botPos so that it's in the starting square, and then delay for however long.
  Serial.println("Interrupt triggered");
  changeHeading(0);
  Serial.print("Old bot row: ");
  Serial.println(botPos.botRow);
  //now change botPos to what it's supposed to be. Do I need to make botPos volatile? 
  botPos.botRow = 7;
  botPos.botCol = 3;
  Serial.print("New bot row: ");
  Serial.println(botPos.botRow);
  //This might need to be 'n'.
  botPos.botDirection = 'e';
  //Change this delay time to be whatever it's supposed to be.
  delay(8000); 
}
*/
/******************************************************************
*******************************************************************
                 Entering and Leaving a hopper
*******************************************************************
*******************************************************************/
int enterHopper(hopperData hoppers[4], byte index) {
  //hopperData is the array of hoppers = {fixed left, variable left, fixed right, variable right}
  //index is the hopper in hoppers that we are approaching
  //This function moves the robot into position to grab a ball.
  //This function returns 0 if everything works properly, and it returns -1 if the enable was flipped.
  if( index == 0 ) {
      //We are entering the left fixed hopper, assume we're at (6, 1) facing west. 
      
      //First turn 45 degrees counterclockwise to be facing the hopper.
      controlledTurn(314, 4);  //Turn into position. 
      delay(2000);
      //Next, start going forwards into the hopper.
      changeHeading(1);
      //go forwards until we run into the gameboard. 
      Serial.println("start moving forwards until I hit the hoppers");
      while(true) {
          //Keep moving forwards until both of the zoidberg claws are clicked
          leftHopDetect = digitalRead(leftHopperDetectPin);
          rightHopDetect = digitalRead(rightHopperDetectPin);
          if (rightHopDetect == 0 and leftHopDetect == 0) {
             break; 
             
          }
          //Poll for off switch. 
          enable = digitalRead(dipSwitchArray[9]);
          if (enable == 1) {
             return -1; 
          }
      }
      Serial.println("I have hit the hoppers");
      changeHeading(0);
  } else if (index == 2) {
      //We are entering the right fixed hopper, assume we're at (6, 5) facing east. 
      //First turn 45 degrees counterclockwise to be facing the hopper.
      controlledTurn(600, 4);  //Turn into position. 
      
      
      //Next, start going forwards into the hopper.
      changeHeading(1);
      //go forwards until we run into the gameboard. 
      
      while(true) {
          //Keep moving forwards until both of the zoidberg claws are clicked
          leftHopDetect = digitalRead(leftHopperDetectPin);
          rightHopDetect = digitalRead(rightHopperDetectPin);
          if (rightHopDetect == 0 and leftHopDetect == 0) {
             break; 
          }
          //Poll for off switch. 
          enable = digitalRead(dipSwitchArray[9]);
          if (enable == 1) {
             return -1; 
          }
      }
      changeHeading(0);
    
  }
  
  return 0;
}

void exitHopper(hopperData hoppers[4], byte index, byte &exitRow, byte &exitCol, char &exitDir) {
  //hopperData is the array of hoppers = {fixed left, variable left, fixed right, variable right}
  //index is the hopper in hoppers that we are approaching
  //Assuming you're pressed up against a hopper with zoidberg claws clicked, this function takes you out of the hopper.
  //This function returns 0 if everything works properly, and it returns -1 if the enable was flipped.
  if( index == 0 ) {
    //We are exiting the left hopper.
    inchBackwardsWithoutLines(true, 1000);
    delay(500);
    inchBackwardsWithoutLines(true, 1000);
    turn(botPos, 3);

    exitRow = 6;
    exitCol = 1;
    exitDir = 'w';

  } else if (index == 2) {
    //We are exiting the right fixed hopper, assume we're at (6, 5) facing east. 
    inchBackwardsWithoutLines(true, 1000);
    delay(500);
    inchBackwardsWithoutLines(true, 1000);
    turn(botPos, 4);

    exitRow = 6;
    exitCol = 5;
    exitDir = 'e';
  }  
}
