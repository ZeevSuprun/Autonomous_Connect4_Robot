#include <Arduino.h>
#include "maze_solving.h"

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
   //0--> indiviual leg facing north, 1 --> individual leg facing south. 
   byte orientation;
   
   byte numBalls;
   //the grid row and column you need to get to in order to enter the hopper and retrieve a ball. 
   byte entryCol;
   byte entryRow;
   //direction robot needs to be facing to enter the hopper.
   char entryDir;
   //robot must be entryAngle degrees clockwise from entryDir to enter the column.
   int entryAngle;
   
   //The following variables are used to back out of the hopper.
   //The time you need to go in reverse for. 
   int backUptime;
   //Follow this string of directions to end up in exitRow and exitCol.
   String backUpPath;

   //The row and column you will be at after you finish backing up.
   byte exitRow;
   byte exitCol;   
   
   hopperData(){
   
   }
};

//Note to self: I need to test these 2 functions independently. 
//Given an array of switch pins and an empty arena, this function reads data from those switches to fill the arena 
//by calling the add_hoppers function.
void readSwitches(int dipSwitchArray[10], class hopperData &leftHopper, class hopperData &rightHopper, char arena[8][7]);
//Given a character array arena of an empty board, this function adds hoppers to that array.
void add_hoppers(int hop1_row, int hop1_col, int hop2_col, char arena[8][7]);
//find the hopper nearest to the robot.
String findNearestHopper(hopperData hoppers[4], robotPosition botPos, char arena [8][7], byte &hopperChosen);
//prints the game arena.
void printArena(char arena[8][7]);
//returns which column to place the ball in.
byte whereToPlace(byte ballsPlacedSoFar);
//Figure out which grid point to approach the hopper from. 
void hopperApproachSquare(class hopperData &hop);

byte whereToPlace(byte ballsPlacedSoFar) {
    //given how many balls were placed, return which column to place the next ball in.
    //goes 3, then 4, then 2, then 5, repeat.
    if (ballsPlacedSoFar % 4 == 0) {
        return 3; 
    } else if (ballsPlacedSoFar % 4 == 1) {
        return 4;
    } else if (ballsPlacedSoFar % 4 == 2) {
        return 2;
    } else if (ballsPlacedSoFar % 4 == 3) {
        return 5;
    } else {
        //this should never happen
        return 0;  
    }
}

String findNearestHopper(hopperData hoppers[4], robotPosition botPos, char arena [8][7], byte &hopperChosen) {
    //hoppers = {fixed left, variable left, fixed right, variable right}
    //String blockedSolver(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]) {
    //Find the hopper nearest to the robot, return a path to that hopper, change hopperChosen to be the index in hoppers[] of the chosen hopper
      
    //stores the index of the nearest non empty hopper.
    byte storedIndex = 0;
    byte dist_to_nearest = 250;
    String tempPath;
    String bestPath = "";
    //Iterate through the array of hoppers t
    for (byte i = 0; i < 4; i++) {
       if (hoppers[i].numBalls > 0) {
         tempPath = blockedSolver(botPos.botRow, botPos.botCol, botPos.botDirection, hoppers[i].entryRow, hoppers[i].entryCol, arena);
         tempPath += change_dir(botPos.botDirection, hoppers[i].entryDir);
         
         if(tempPath.length() < dist_to_nearest) {
            storedIndex = i;
            dist_to_nearest = tempPath.length(); 
            bestPath = tempPath;
         }
       }
    }
    //storedIndex now holds the index of the nearest hopper.
    //bestPath is now the path to the nearest hopper.
    hopperChosen = storedIndex;
    return bestPath;
}

void printArena(char arena[8][7]) {
     //Prints the game arena with hoppers and stuff.
     for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 7; j++) {
            Serial.print(arena[i][j]);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
    Serial.println("------------------\n0 1 2 3 4 5 6");
}

void readSwitches(int dipSwitchArray[10], class hopperData &leftHopper, class hopperData &rightHopper, char arena[8][7]) {
    //note: off is 1/HIGH and on is 0/LOW.
    int switchValArray[10];
    //Read the values of all of the switches. 
    //while(true) {
      for (byte i = 0; i < 10; i++) {
          switchValArray[i] = digitalRead(dipSwitchArray[i]);
          Serial.print(switchValArray[i]);
          Serial.print(" ");
      }
      Serial.print("\n");
    //}
    //First 2 switches are a BCD between 0 and 2 representing left hopper row.
    //dipSwitchArray[2] is either 0 or 1, representing left hopper column. 
    //dipSwitchArray[3] is either 0 or 1, representing left hopper orientation.. 
    //dipSwitchArray[4] is either 0 or 1, representing right hopper column. (0 for row 2 and 1 for row 3)
    //dipSwitchArray[5] is either 0 or 1, representing right hopper oreintation. //0 north, 1 south
    //dipSwitchArray[6] represents the ball colour. 0 for white, 1 for black. 
    const int ON = 0;
    const int OFF = 1;
    //SETTING HOPPER ROW
    //off off --> 0
    if (dipSwitchArray[0] == OFF and dipSwitchArray[1] == OFF) {
        leftHopper.hopperRow = 0;
    //off on --> 1 
    } else if (dipSwitchArray[0] == OFF and dipSwitchArray[1] == ON) {
        leftHopper.hopperRow = 1;
    //on off --> 2
    } else if (dipSwitchArray[0] == ON and dipSwitchArray[1] == OFF) {
        leftHopper.hopperRow = 2;
    }
    
    //off --> 1 --> needs to = 0. on --> 0 --> hopperCol = 1
    leftHopper.hopperCol = (dipSwitchArray[2] == ON); 
    leftHopper.orientation = (dipSwitchArray[3] == ON);
    
    rightHopper.hopperRow = 2 - leftHopper.hopperCol;
    rightHopper.hopperCol = (dipSwitchArray[4] == ON) + 2; 
    rightHopper.orientation = (dipSwitchArray[5] == ON);
    
    //On /LOW for white, off / HIGH for black. 
    byte ballColour = (dipSwitchArray[6] == ON);
    
    add_hoppers(rightHopper.hopperRow, rightHopper.hopperCol, leftHopper.hopperCol, arena);
    
    //Figure out the hopper approach square. 
    hopperApproachSquare(leftHopper);
    hopperApproachSquare(rightHopper);
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
    boolean rc = 1;
    boolean cc = 1;
    
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
        for (int r = hop2_row+2 - 1*rc; r < hop2_row + 4 + 1*rc; r++) {
            arena[r][c] = 'x';
            if (cc and (hop2_col == 0 or hop2_col == 2)) {
                arena[r][hop2_col+3] = 'x';
            } else if (cc and (hop2_col == 1 or hop2_col == 3)){
                arena[r][hop2_col] = 'x';
            }
        }
    }    
}

void approachHopper(hopperData hop, int leftClawButton, int rightClawButton) {
   //Assuming we are in the hopper approach square, actually approach the hopper. 
   /*
   //robot must be entryAngle degrees clockwise from entryDir to enter the column.
    int entryAngle;
   
   //The following variables are used to back out of the hopper.
   //The time you need to go in reverse for. 
   int backUptime;
   //Follow this string of directions to end up in exitRow and exitCol.
   String backUpPath;

   //The row and column you will be at after you finish backing up.
   byte exitRow;
   byte exitCol;   
   */
   //turn the correct direction
   if(hop.entryAngle > 0) {
      
   }
   
   
   
}

void hopperApproachSquare(class hopperData &hop) {
   //Given a non-corner hopper that already has a row, column and orientation, give a value to entryCol and entryRow
   if (hop.hopperCol == 0) {
       hop.entryCol = 2; 
       if(hop.orientation == 0) {
           //if hopper facing north
           hop.entryRow = hop.hopperRow + 4;
           hop.entryDir = 'n';
           hop.entryAngle = 0;
       } else if (hop.orientation == 1) {
           //if hopper facing south
           hop.entryRow = hop.hopperRow + 1;
           hop.entryDir = 's';
           hop.entryAngle = 0;
       }
   } else if (hop.hopperCol == 1) {
       hop.entryCol = 1; 
       if(hop.orientation == 0) {
           //if hopper facing north
           hop.entryRow = hop.hopperRow + 2;
           hop.entryDir = 's';
           hop.entryAngle = -45;
       } else if (hop.orientation == 1) {
           //if hopper facing south
           hop.entryRow = hop.hopperRow + 3;
           hop.entryDir = 'n';
           hop.entryAngle = 45;
       }     
   } else if (hop.hopperCol == 2) {
       hop.entryCol = 5;
       if(hop.orientation == 0) {
           //if hopper facing north
           hop.entryRow = hop.hopperRow + 2;
           hop.entryDir = 's';
           hop.entryAngle = 45;
       } else if (hop.orientation == 1) {
           //if hopper facing south
           hop.entryRow = hop.hopperRow + 3;
           hop.entryDir = 'n';
           hop.entryAngle = -45;
       }     
   } else if (hop.hopperCol == 3) {
       hop.entryCol = 4; 
       if(hop.orientation == 0) {
           //if hopper facing north
           hop.entryRow = hop.hopperRow + 4;
           hop.entryDir = 'n';
           hop.entryAngle = 0;
       } else if (hop.orientation == 1) {
           //if hopper facing south
           hop.entryRow = hop.hopperRow + 1;
           hop.entryDir = 's';
           //exact hopper entry angles unknown for now. 
           hop.entryAngle = -10;
       }     
   } 
}

