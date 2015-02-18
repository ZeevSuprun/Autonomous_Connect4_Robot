//
//  compositeTest.h
//  robot_gameplay
//
//  Created by Zeev Suprun on 2015-02-06.
//  Copyright (c) 2015 Zeev Suprun. All rights reserved.
//
/*
#ifndef __robot_gameplay__compositeTest__
#define __robot_gameplay__compositeTest__

#include <iostream>

#endif /* defined(__robot_gameplay__compositeTest__) */




void goForwards(class robotPosition &botPos) {
    //This function makes the robot move forwards numTiles number of grid points using line following.
    //This function currently won't work right, because it'll detect the line that it starts on and it'll rack up the counter as soon as it crosses a line.
    
    //Change the robot's heading so that it's moving forwards.
    changeHeading(1);
    //Delay long enough so that it doesn't immediately stop.
    delay(1000);
    
    while (true) {
        for (byte i = 0; i < 5; i++) {
            QRE_val_array[i] =  readQD(QRE_pin_array[i]);
            //To test what values the sensor array is reading.
            //Serial.print(QRE_val_array[i]);
            //Serial.print(" ");
        }
        //go forwards in a straight line.
        if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[2] == HIGH and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
            //Go straight
            analogWrite(leftEnablePin, motorSpeed);
            analogWrite(rightEnablePin, motorSpeed);
        } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == HIGH and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
            //adjust left.
            analogWrite(leftEnablePin, motorSpeed - 30);
            analogWrite(rightEnablePin, motorSpeed);
        } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[3] == HIGH and QRE_val_array[4] == LOW) {
            //adjust to right.
            analogWrite(leftEnablePin, motorSpeed);
            analogWrite(rightEnablePin, motorSpeed - 30);
            
        } else if (QRE_val_array[0] == HIGH and QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH and QRE_val_array[4] == HIGH) {
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
        } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[2] == LOW and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
            //we're off track, or are turning.
            //therefore don't change anything, keep doing what you're doing.
            ;
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
    changeHeading(turnwise);
    
    //Delay long enough so that it doesn't immediately stop turning.
    delay(1000);
    
    while(true) {
        //Keep turning until we're on a line.
        if (QRE_val_array[0] == HIGH and QRE_val_array[1] == HIGH and QRE_val_array[2] == HIGH and QRE_val_array[3] == HIGH and QRE_val_array[4] == HIGH) {
            changeHeading(0);
            break;
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

/******************************************************************
 *******************************************************************
 High Level functions.
 *******************************************************************
 *******************************************************************/

String empty_solver(byte startRow, byte startCol, char dir, byte destRow, byte destCol){
    /*This grid solver tells the robot to get from a starting point to a destination point
     * but it only works on an empty grid.
     *This function is passed integers for startRow, startCol and dir because I don't want it to be able to actually change the robot's position.
     */
    
    //stores the path that the robot takes.
    //w means move forwards one grid line.
    //s means move backwards one grid line.
    //e means rotate clockwise 90 degrees.
    //q means rotate counterclockwise 90 degrees.
    String path = "";
    
    //(0, 0) is in the top right. (row, column)
    //bottom right corner is (7, 6)
    
    //first move to required column.
    if (startCol > destCol) {
        //I need to go west.
        
        //change direction to face west.
        path += change_dir(dir, 'w');
        dir = 'w';
        //Move forwards until I get to required position.
        for (int i = 0; i < startCol - destCol; i++) {
            path += "w";
        }
    } else if (startCol < destCol) {
        //I need to go east.
        
        //change direction to face east.
        path += change_dir(dir, 'e');
        dir = 'e';
        
        //Move forwards until I get to required position.
        for (int i = 0; i < destCol - startCol; i++) {
            path += "w";
        }
    }
    //Move to required row.
    if (startRow > destRow) {
        //I need to go north.
        
        //change direction to face north.
        path += change_dir(dir, 'n');
        dir = 'n';
        //Move forwards until I get to required position.
        for (int i = 0; i < startRow - destRow; i++) {
            path += "w";
        }
    } else if (startRow < destRow) {
        //I need to go south.
        
        //change direction to face south.
        path += change_dir(dir, 's');
        dir = 's';
        //Move forwards until I get to required position.
        for (int i = 0; i < destRow - startRow; i++) {
            path += "w";
        }
    }
    return path;
}

String change_dir (char startDir, char endDir) {
    /*This function takes a starting direction and returns a set of instructions
     *to turn from the initial direction to the final direction.
     *(clockwise, clockwise twice, or counterclockwise).
     */
    
    //Tested the run time on this function, max runtime is only about 2* min runtime (so not alot)
    
    if (startDir == endDir) {
        return "";
    }
    if (startDir == 'n') {
        if (endDir == 'e') {
            return "e";
        } else if (endDir == 's') {
            return "ee";
        } else if (endDir == 'w') {
            return "q";
        }
    } else if (startDir == 'e') {
        if (endDir == 's') {
            return "e";
        } else if (endDir == 'w') {
            return "ee";
        } else if (endDir == 'n') {
            return "q";
        }
    } else if (startDir == 's') {
        if (endDir == 'w') {
            return "e";
        } else if (endDir == 'n') {
            return "ee";
        } else if (endDir == 'e') {
            return "q";
        }
    } else if (startDir == 'w') {
        if (endDir == 'n') {
            return "e";
        } else if (endDir == 'e') {
            return "ee";
        } else if (endDir == 's') {
            return "q";
        }
    }
    //this should never happen.
    return "-1";
}

void goForwards_backwardsWorking(class robotPosition &botPos) {
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
        //go forwards in a straight line.
        if (QRE_val_array[1] == LOW and QRE_val_array[2] == HIGH and QRE_val_array[3] == LOW) {
            //Go straight
            analogWrite(leftEnablePin, motorSpeed);
            analogWrite(rightEnablePin, motorSpeed);
            Serial.write("going forwards\n");
        } else if (QRE_val_array[1] == HIGH and QRE_val_array[3] == LOW) {
            //adjust left.
            analogWrite(leftEnablePin, motorSpeed);
            analogWrite(rightEnablePin, motorSpeed - 30);
            Serial.write("Turning left\n");
        } else if (QRE_val_array[1] == LOW and QRE_val_array[3] == HIGH) {
            //adjust to right.
            analogWrite(leftEnablePin, motorSpeed - 30);
            analogWrite(rightEnablePin, motorSpeed);
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
        } else if (QRE_val_array[0] == LOW and QRE_val_array[1] == LOW and QRE_val_array[2] == LOW and QRE_val_array[3] == LOW and QRE_val_array[4] == LOW) {
            //we're off track, or are turning.
            //therefore don't change anything, keep doing what you're doing.
            ;
        } else {
            //Something's up: These cases shouldn't happen, or should only happen while turning.
            //Therefore do nothing. 
            ;
        }
    }
    //change the robot's heading so that it's staying in place. 
    changeHeading(0);
}

