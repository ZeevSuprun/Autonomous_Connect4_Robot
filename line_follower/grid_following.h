#include <Arduino.h>

//This function make the robot move forwards for one 
void goForwards(class robotPosition &botPos);

//This function finds a path from point A to point B in an empty grid. 
String empty_solver(byte startRow, byte startCol, char dir, byte destRow, byte destCol);
//this function determines shortest path to move from direction A to direction B. 
String change_dir (char startDir, char endDir);

class robotPosition {
public:
  byte botRow;
  byte botCol;
  //This is either 'n', 's', 'e', 'w' where n means north.
  char botDirection;
}; 

void goForwards(class robotPosition &botPos){
  
}

/******************************************************************
*******************************************************************
               High Level maze solving functions. 
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
    
    //(0, 0) is in the top left. (row, column)
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

