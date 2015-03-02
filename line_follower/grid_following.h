#include <Arduino.h>

class robotPosition {
  //This class keeps track of the robot's position.
public:
  byte botRow;
  byte botCol;
  //This is either 'n', 's', 'e', 'w' where n means north.
  char botDirection;
}; 

class hopperData {
public:
  //row can be 0, 1, 2. 
   byte hopperRow;
   //column can be 0,1,2,3
   byte hopperCol;
   byte orientation;
   
   byte numBalls; 
  
};


//Note to self: I need to test these 2 functions independently. 
//Given an array of switch pins and an empty arena, this function reads data from those switches to fill the arena 
//by calling the add_hoppers function.
void readSwitches(int dipSwitchArray[7], class hopperData &leftHopper, class hopperData &rightHopper, char arena[8][7]);
//Given a character array arena of an empty board, this function adds hoppers to that array.
void add_hoppers(int hop1_row, int hop1_col, int hop2_col, char arena[8][7]);

//NOTE TO SELF THIS THING IS UNFINISHED
void readSwitches(int dipSwitchArray[7], class hopperData &leftHopper, class hopperData &rightHopper, char arena[8][7]) {

    int switchValArray[7];
    //Read the values of all of the switches. 
    for (byte i = 0; i < 7; i++) {
        switchValArray[i] = digitalRead(dipSwitchArray[i]);
    }
    //First 2 switches are a BCD between 0 and 2 representing left hopper row.
    //dipSwitchArray[2] is either 0 or 1, representing left hopper column. 
    //dipSwitchArray[3] is either 0 or 1, representing left hopper orientation.. 
    //dipSwitchArray[4] is either 0 or 1, representing right hopper column. (0 for row 2 and 1 for row 3)
    //dipSwitchArray[5] is either 0 or 1, representing right hopper oreintation.
    //dipSwitchArray[6] represents the ball colour. 0 for white, 1 for black. 
    
    //SETTING HOPPER ROW.
    if (dipSwitchArray[0] == LOW and dipSwitchArray[1] == LOW) {
        leftHopper.hopperRow = 0;
    } else if (dipSwitchArray[0] == LOW and dipSwitchArray[1] == HIGH) {
        leftHopper.hopperRow = 1;
    } else if (dipSwitchArray[0] == HIGH and dipSwitchArray[1] == LOW) {
        leftHopper.hopperRow = 2;
    }
    
    leftHopper.hopperCol = dipSwitchArray[2] == HIGH; 
    leftHopper.orientation = dipSwitchArray[3] == HIGH;
    
    rightHopper.hopperRow = 2 - leftHopper.hopperCol;
    rightHopper.hopperCol = (dipSwitchArray[4] == HIGH) + 2; 
    rightHopper.orientation = (dipSwitchArray[5] == HIGH);
    
    //0 / low for white, 1 / HIGH for black. 
    byte ballColour = (dipSwitchArray[5] == HIGH);
    
    add_hoppers(rightHopper.hopperRow, rightHopper.hopperCol, leftHopper.hopperCol, arena);
}


void add_hoppers(int hop1_row, int hop1_col, int hop2_col, char arena[8][7]) {
    //This function adds the hopper positions as obstacles in the gameboard.
    //hop1_row and hop2_row are between 0 and 2 (inclusive).
    //hop1_col and hop2_col are between 0 and 3 (inclusive).
    
    //hopper row +2, +3 are blocked off. hopper row +1, +4 might be.
    //hopper column +1. +2 are blocked off. For even numbers, +3 is a maybe, for odd numbers, +0 is.
    
    //For now, I'm erring on the side of caution and blocking off all of the maybes.
    //I will need to recheck all of the maybes later once the robot is complete.
    
    int hop2_row = 2 - hop1_row;
    
    //These variables are row caution and column caution.
    //if rc = 0, treat row caution squares as empty, else treat them as blocked.
    bool rc = 0;
    bool cc = 0;
    
    //filling in all the blocked grid points and all the maybes for hopper 1.
    for (int c = hop1_col+1; c < hop1_col+2 + 1; c++) {
        for (int r = hop1_row + 2 - 1*rc; r < hop1_row + 4 + 1*rc; r++) {
            arena[r][c] = 'x';
            if (cc and (hop1_col == 0 or hop1_col == 2)) {
                arena[r][hop1_col+3] = 'x';
            } else if (cc and (hop1_col == 1 or hop1_col == 3)){
                arena[r][hop1_col] = 'x';
            }
        }
    }
    //filling in all the blocked grid points and all the maybes for hopper 2.
    for (int c = hop2_col+1; c < hop2_col+2 + 1; c++) {
        for (int r = hop2_row+2 - 1*rc; r < hop2_row + 4 + 1; r++) {
            arena[r][c] = 'x';
            if (cc and (hop2_col == 0 or hop2_col == 2)) {
                arena[r][hop2_col+3] = 'x';
            } else if (cc and (hop2_col == 1 or hop2_col == 3)){
                arena[r][hop2_col] = 'x';
            }
        }
    }    
}

