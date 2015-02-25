/******************************************************************
*******************************************************************
                      Function declarations
*******************************************************************
*******************************************************************/

#include "grid_following.h"
#include "sensing.h"

//This function changes the direction the robot is moving in. (forwards, backwards, clockwise, counterclockwise)
// 0: Do nothing.  1: Go forwards.  2: Go in reverse. 3: Turn right in place.  4: turn left in place. 
void changeHeading(byte heading);

//This function make the robot move forwards for one 
void goForwards(class robotPosition &botPos);
//Make the robot turn 90 degrees either clockwise or counterclockwise. 
void turn(class robotPosition &botPos, byte turnwise);
//this function makes the robot follow the path found by the empty_solver function.
void follow_directions(String  directions, class robotPosition &botPos);

//Line following sensors. 
//element 0: reading leftmost QRE pin, 1: left QRE pin, 2: centre QRE pin, 3: right QRE pin, 4: rightmost QRE pin, 5: front QRE pin.
const int QRE_pin_array[] = {A5, A2, A1, A3, A4, A0};

//Stores the values detected by the line sensors.
//0: reading from leftmost QRE, 1: reading from left QRE, 2: centre QRE, 3: right QRE, 4: rightmost QRE. 5: front QRE pin 
int QRE_val_array[6];


/******************************************************************
*******************************************************************
        The motor control pins and related constants
*******************************************************************
*******************************************************************/
//If leftDirectionPin is HIGH, motor goes forwards. 
const int leftDirectionPin = 5;
//If both are off, motor is off. 
const int leftEnablePin = 4;

//Same thing but for right motor. 
const int rightDirectionPin = 3;
const int rightEnablePin = 2;

//The speed of the motor (255 is the maximum)
byte motorSpeed = 255;



/******************************************************************
*******************************************************************
                      Setup and loop
*******************************************************************
*******************************************************************/
robotPosition botPos;
String directions;
int counter = 0;

void setup(){
    
    /*
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
    */
    changeHeading(1);
          //Set left motor backwards. 

}
void loop() {
  /*
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

  if (heading == 0) {
      //Stay still. 
      analogWrite(rightEnablePin, LOW);
      analogWrite(leftEnablePin, LOW);
      //Serial.write("The robot should not be moving.");

  } else if (heading == 1) {
      //Go forwards. 
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor forwards. 
      digitalWrite(rightDirectionPin, HIGH);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (heading == 2) {
      //Go backwards. 
      //Set left motor backwards. 
      digitalWrite(leftDirectionPin, LOW);
      analogWrite(leftEnablePin, motorSpeed);

      //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
            
  } else if (heading == 3) {
      //Turn counterclockwise. 
      //Set left motor forwards. 
      digitalWrite(leftDirectionPin, HIGH);
      analogWrite(leftEnablePin, motorSpeed);

     //Set right motor backwards
      digitalWrite(rightDirectionPin, LOW);
      analogWrite(rightEnablePin, motorSpeed);
  } else if (heading == 4) {
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
   
  int error;
  int k = 100;
  
  while (true) {
    //Populate sensor array.
    for (byte i = 0; i < 4; i++) {
        /*
        if (i = 0 or i = 4) {
          QRE_val_array[i] =  binary_readAnalog(QRE_pin_array[i]);
        } else {
          QRE_val_array[i] =  readAnalogQRE(QRE_pin_array[i]);
        }*/
        QRE_val_array[i] =  readAnalogQRE(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.write(" ");
    }
    Serial.write("\n");
    
    //The error is proportional to the distance of the center censor from the line it is following. 
    error = (QRE_val_array[3] - QRE_val_array[1]) / (QRE_val_array[1] + QRE_val_array[2] + QRE_val_array[3]);
    
    if (error < 0 ) {
      //negative error means adjust left
        analogWrite(leftEnablePin, motorSpeed -k*error);
        analogWrite(rightEnablePin, motorSpeed);
        Serial.write("Adjusting left\n");
    } else {
        //positive error, means adjust right. 
         analogWrite(leftEnablePin, motorSpeed);
         analogWrite(rightEnablePin, motorSpeed - k*error);
         Serial.write("Adjusting right\n"); 
    }
    
    if (QRE_val_array[0] == HIGH and QRE_val_array[4] == HIGH) {
        //we're crossing a line so we need to take note. 
        
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
      
      //Read from sensors.
      for (byte i = 0; i < 5; i++) {
        QRE_val_array[i] =  binary_readAnalog(QRE_pin_array[i]);
        //To test what values the sensor array is reading. 
        Serial.print(QRE_val_array[i]);
        Serial.write(" ");
      }
      if ( QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH) {
          changeHeading(0);
          Serial.write("The between-the wheel sensors detected that we've completed a turn.");
          break;
      } else if (QRE_val_array[5] == HIGH) {
          changeHeading(0);
          Serial.write("The front sensor detected that we'd completed a turn.");
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

