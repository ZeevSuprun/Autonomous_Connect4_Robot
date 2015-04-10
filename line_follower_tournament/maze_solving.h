#include <Arduino.h>

//Function definitions:

String horizontal_first(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]);
String vertical_first(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]);

//this function determines shortest path to move from direction A to direction B. NOTE: AFTER CALLIN change_dir YOU SHOULD ALWAYS UPDATE THE ROBOTS DIRECTION SO THAT ITS FACING THE RIGHT WAY.
String change_dir (char startDir, char endDir);

String blockedSolver(byte startRow, byte startCol, char dir, byte destRow, byte destCol, char arena[8][7], char &endDir);

String blockedSolver(byte startRow, byte startCol, char dir, byte destRow, byte destCol, char arena[8][7], char &endDir) {
    //First need to get to a clear intersection.
    //Given an array of clear rows and an array of clear columns.
    //If I'm in a clear row, go to the clear column nearest to mine.
    //If I'm in a clear column, go to the clear row nearest to mine.
    //Serial.println("Blocked solver function caled");
    String clearRows = "";
    String clearCols = "";
    byte currentRow = startRow;
    byte currentCol = startCol;
    
    boolean rowClear, colClear;
    //Deterine which rows are clear.
    for (int row = 0; row < 8; row++)
    {
        rowClear = true;
        for (int col = 0; col < 7; col++)
        {
            if(arena[row][col] == 'x') {
              rowClear = false;
            }
        }
        if (rowClear) {
            //append the row to the clearRows string. 
            clearRows += row;
        }
    }
    //determine which columns are clear.
    for (int col = 0; col < 7; col++) {
        colClear = true;
        for (int row = 0; row < 8; row++) {
            if(arena[row][col] == 'x') {
                colClear = false;
            }
        }
        if (colClear) {
            //append the column to the clear columns string. 
            clearCols += col;
        }
    }
    
    //Serial.print("Clear columns:");
    //Serial.println(clearCols);
    //Serial.print("Clear Rows:");
    //Serial.println(clearRows);
    String path = ""; 

    //end direction if you go horizontally first.
    char horiEndDir; 
    //horizontal first path. 
    String horiFirst;
    //end direction if you go vertically first. 
    char vertEndDir;
    //vertical first string path.
    String vertFirst;
    
    //First, check if we are in a corner case and get out of the corner case if we are. 
    if (clearCols.indexOf(String(startCol)) == -1 and clearRows.indexOf(String(startRow)) == -1) {
       //if in neither a clear row or a clear column, then we need to get out first. 
       
       //first, find the nearest clear column. (there will always be a clear path to it).
       int nearestClearCol=0;
       int lowestDist = 8;
       int dist;
       for (int i = 0; i < clearCols.length(); i++) {
           dist = abs(String(clearCols[i]).toInt() - startCol);
           if (dist < lowestDist) {
              lowestDist = dist;
              //toInt() function works on strings but not characters. As a result this is convoluted. 
              nearestClearCol = (String(clearCols[i])).toInt();
           }
       }
       //Now that we know what the nearest clear column is, we need to move to it.
       //Serial.print("I'm getting out of a corner case. The nearest clear column is column ");
       //Serial.println(nearestClearCol);
       path += horizontal_first(currentRow, currentCol, dir, currentRow, nearestClearCol, arena);
       currentCol = nearestClearCol;
    }
    //We are now no longer in a corner case, if we were in one before. Now, we should check if there is an L-shaped or clear path to the goal. 
    //Serial.println(path);

    //This if statement MIGHT be slightly more efficient then just skipping to the else block? Either way, it will work. 
    if ((startRow == destRow) and (clearRows.indexOf(String(startRow)) != -1) or (startCol == destCol and clearCols.indexOf(String(startCol)) != -1)) {
       //If we're in the same clear row as destination or same clear column as destination then we can go straight there, find the path using emptySolver. 
       path = horizontal_first(startRow, startCol, dir, destRow, destCol, arena);
       endDir = dir;
       return path; 
    } else {
        //check if there's an L shaped path that gets to the end right away. 
        horiEndDir = dir; 
        horiFirst = horizontal_first(currentRow, currentCol, horiEndDir, destRow, destCol, arena);
        vertEndDir = dir;
        vertFirst = vertical_first(currentRow, currentCol, vertEndDir, destRow, destCol, arena);
        
        if (horiFirst != "0") {
           //the horizontal path works.
           path += horiFirst; 
           dir = horiEndDir;
           endDir = dir;
           return path;
        } else if (vertFirst != "0") {
           //the veritcal path works. 
           path += vertFirst; 
           dir = vertEndDir;
           endDir = dir;
           return path;
        } 
    }
    //If there is no L shaped path, then we need to get to a clear intersection.
    //Serial.println("There is no L shaped path. going to an intersection");
    //First, find the nearest clear intersection.
    if (clearCols.indexOf(String(startCol)) == -1) {
       //Serial.println("I'm not in a clear column");
       //If the robot isn't in a clear column, it must move to a clear column. 
       //First, find the nearest clear column.
       int nearestClearCol=0;
       int lowestDist = 8;
       int dist;
       for (int i = 0; i < clearCols.length(); i++) {
           dist = abs(String(clearCols[i]).toInt() - startCol);
           if (dist < lowestDist) {
              lowestDist = dist;
              //toInt() function works on strings but not characters. As a result this is convoluted. 
              nearestClearCol = (String(clearCols[i])).toInt();
           }
       }
       //Now that we know what the nearest clear column is, we need to move to it.
       //Serial.print("The nearest clear column is column ");
       //Serial.println(nearestClearCol);
       path += horizontal_first(currentRow, currentCol, dir, currentRow, nearestClearCol, arena);
       //path += "  ";
       currentCol = nearestClearCol;
    } else if(clearRows.indexOf(String(startRow)) == -1) {
       //Serial.println("I'm not in a clear row");
       //If the robot isn't in a clear row, it must move to a clear row. 
       //First, find the nearest clear row.
       int nearestClearRow;
       int lowestDist = 8;
       int dist;
       for (int i = 0; i < clearRows.length(); i++) {
           dist = abs(String(clearCols[i]).toInt() - startRow);
           if (dist < lowestDist) {
              lowestDist = dist;
              //toInt() function works on strings but not characters. As a result this is convoluted. 
              nearestClearRow = (String(clearRows[i])).toInt();
           }
       }
       //Serial.print("The nearest clear column is row ");
       //Serial.println(nearestClearRow);
       //Now that we know what the nearest clear row is, we need to move to it.
       path += horizontal_first(currentRow, currentCol, dir, nearestClearRow, currentCol, arena);
       //path += "  ";
       currentRow = nearestClearRow;
    }
    //Serial.println(path);
    //Serial.println("We should now be in an intersection.");
    //We now have the path to the nearest clear intersection. From here we can go to the destination.
    //There are 2 ways to go: horizontal first and vertical first. One of them is invalid.
    
    //direction is passed by reference, so we don't want to change dir before we know which of the 2 paths actually works. 
    horiEndDir = dir; 
    horiFirst = horizontal_first(currentRow, currentCol, horiEndDir, destRow, destCol, arena);
    vertEndDir = dir;
    vertFirst = vertical_first(currentRow, currentCol, vertEndDir, destRow, destCol, arena);
    
    if (horiFirst != "0") {
       //the horizontal path works.
       path += horiFirst; 
       dir = horiEndDir;
       endDir = dir;
       return path;
    } else if (vertFirst != "0") {
       //the veritcal path works. 
       path += vertFirst; 
       dir = vertEndDir;
       endDir = dir;
       return path;
    }
    //The following should happen very rarely, if at all. 
    //Serial.println("There is still no L shaped path");
    //If neither of the above if statements are true, then there is no L shaped path from this clear intersection, so we need to get to a different clear intersection.
    //Any clear intersection in a different corner of the board will work.
    
    //Arbitrarily choose to do a horizontal change. (Move to a different clear intersection in the same column and a different row).
    //Since we're doing a horizontal change the new intersection can be in the same row as the old intersection but not in the same column or within 1 column.
    for (int i = 0; i < clearCols.length(); i++) {
       if(abs(String(clearCols[i]).toInt() - currentCol) > 2) {
          //If the new clear column is more than 2 columns away than this one, it is in a different corner.
          path += horizontal_first(currentRow, currentCol, dir, currentRow, String(clearCols[i]).toInt(), arena);
          currentCol = String(clearCols[i]).toInt();
          break;
       } 
    }
    
    
    //Serial.print("Having gone to a different corner, the complete path is now ");
    //Serial.println(path);
    //we are now in the other intersection, time to L-shape. 
    horiEndDir = dir; 
    horiFirst = horizontal_first(currentRow, currentCol, horiEndDir, destRow, destCol, arena);
    vertEndDir = dir;
    vertFirst = vertical_first(currentRow, currentCol, vertEndDir, destRow, destCol, arena);
    
    if (horiFirst != "0") {
       //the horizontal path works.
       path += horiFirst; 
       dir = horiEndDir;
       endDir = dir;
       return path;
    } else if (vertFirst != "0") {
       //the veritcal path works. 
       path += vertFirst; 
       dir = vertEndDir;
       endDir = dir;
       return path;
    }
    //Now this should never happen, and if it does we're kind of fucked.
    return "0"; 
}


String horizontal_first(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]){
    /* This function attempts to find a path from (startRow, startCol) to (destRow, destCol)
     * by moving horizontally first, and then moving vertically.
     * It returns a string path. consiting of "w", "s" "q" and "e"
     * If the path is blocked, it the string it returns will be "0".
     */
    //stores the path that the robot takes.
    String path = "";
    
    //(0, 0) is in the top right. (row, column)
    //bottom right corner is (7, 6)
    //(0, 0) is in the top right. (row, column)
    //bottom right corner is (7, 6)
    
    //first move to required column.
    boolean crossRed = 0;

    if (startCol > destCol) {
        //I need to go west.
        
        //change direction to face west.
        path += change_dir(dir, 'w');
        dir = 'w';
        if (3 < startCol and destCol < 3) {
            //If it's passing through the red column.      
            crossRed = 1;
        }
        //Move forwards until I get to required position.
        for (int i = startCol - crossRed; i > destCol; i--) {
            if(arena[startRow][i] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
            path += "w";
        }
    } else if (startCol < destCol) {
        //I need to go east.
        
        //change direction to face east.
        path += change_dir(dir, 'e');
        dir = 'e';
        if (3 < destCol and startCol < 3) {
            //If it's passing through the red column.      
            crossRed = 1;
        }        

        //Move forwards until I get to required position.
        for (int i = startCol+ crossRed; i < destCol; i++) {
            if(arena[startRow][i] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
            path += "w";
        }
    }
    //then Move to required row.
    if (startRow > destRow) {
        //I need to go north.
        
        //change direction to face north.
        path += change_dir(dir, 'n');
        dir = 'n';
        //Move forwards until I get to required position.
        for (int i = startRow; i > destRow; i--) {
            path += "w";
            if(arena[i][destCol] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
        }
    } else if (startRow < destRow) {
        //I need to go south.
        
        //change direction to face south.
        path += change_dir(dir, 's');
        dir = 's';

        //Move forwards until I get to required position.
        for (int i = startRow; i < destRow; i++) {
            if(arena[i][destCol] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
            path += "w";
        }
    }
    return path;
}

String vertical_first(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]){
    /* This function attempts to find a path from (startRow, startCol) to (destRow, destCol)
     * by moving vertically first, and then moving horizontally.
     * It returns a string path. consiting of "w", "s" "q" and "e"
     * If the path is blocked, it the string it returns will be "0".
     */
    //stores the path that the robot takes.
    String path = "";
    
    //first Move to required row.
    if (startRow > destRow) {
        //I need to go north.
        
        //change direction to face north.
        path += change_dir(dir, 'n');
        dir = 'n';
        //Move forwards until I get to required position.
        for (int i = startRow; i > destRow; i--) {
            path += "w";
            if(arena[i][startCol] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
        }
    } else if (startRow < destRow) {
        //I need to go south.
        
        //change direction to face south.
        path += change_dir(dir, 's');
        dir = 's';
        //Move forwards until I get to required position.
        for (int i = startRow; i < destRow; i++) {
            if(arena[i][startCol] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
            path += "w";
        }
    }
    boolean crossRed = 0;
    //then move to required column.
    if (startCol > destCol) {
        //I need to go west.
        
        //change direction to face west.
        path += change_dir(dir, 'w');
        dir = 'w';
        //Move forwards until I get to required position.
        if (3 < startCol and destCol < 3) {
            //If it's passing through the red column.      
            crossRed = 1;
        }
        for (int i = startCol - crossRed; i > destCol; i--) {
            if(arena[destRow][i] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
            path += "w";
        }
    } else if (startCol < destCol) {
        //I need to go east.
        //change direction to face east.
        path += change_dir(dir, 'e');
        dir = 'e';
        if (3 < destCol and startCol < 3) {
            //If it's passing through the red column.      
            crossRed = 1;
        }        
        
        //Move forwards until I get to required position.
        for (int i = startCol + crossRed; i < destCol; i++) {
            if(arena[destRow][i] == 'x') {
                //If the path tries to go through a blocked space it won't work.
                return "0";
            }
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
