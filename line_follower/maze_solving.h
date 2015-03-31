#include <Arduino.h>

//Function definitions:

String horizontal_first(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]);
String vertical_first(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]);

//This function finds a path from point A to point B in an empty grid. It is deprecated.
String empty_solver(byte startRow, byte startCol, char &dir, byte destRow, byte destCol);
//this function determines shortest path to move from direction A to direction B. 
String change_dir (char startDir, char endDir);
//This function finds a path from point A to point B and returns a string of directions for the robot to follow that path. 
//It doesn't actually work and fails for no apparent reason
String solve(byte startRow, byte startCol, char startDir, byte endRow, byte endCol, char arena[8][7]);


String blockedSolver(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]) {
    //First need to get to a clear intersection.
    //Given an array of clear rows and an array of clear columns.
    //If I'm in a clear row, go to the clear column nearest to mine.
    //If I'm in a clear column, go to the clear row nearest to mine.
    String clearRows = "";
    String clearCols = "";
    byte currentRow = startRow;
    byte currentCol = startCol;
    
    bool rowClear, colClear;
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
       Serial.print("The nearest clear column is column ");
       Serial.println(nearestClearCol);
       path += horizontal_first(currentRow, currentCol, dir, currentRow, nearestClearCol, arena);
       currentCol = nearestClearCol;
    }
    //We are now no longer in a corner case, if we were in one before. Now, we should check if there is an L-shaped or clear path to the goal. 
    
    //This if statement MIGHT be slightly more efficient then just skipping to the else block? Either way, it will work. 
    if ((startRow == destRow) and (clearRows.indexOf(String(startRow)) != -1) or (startCol == destCol and clearCols.indexOf(String(startCol)) != -1)) {
       //If we're in the same clear row as destination or same clear column as destination then we can go straight there, find the path using emptySolver. 
       path = horizontal_first(startRow, startCol, dir, destRow, destCol, arena);
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
           return path;
        } else if (vertFirst != "0") {
           //the veritcal path works. 
           path += vertFirst; 
           dir = vertEndDir;
           return path;
        } 
    }
    
    //If there is no L shaped path, then we need to get to a clear intersection.
    
    //First, find the nearest clear intersection.
   if (clearCols.indexOf(String(startCol)) == -1) {
       Serial.println("I'm not in a clear column");
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
       Serial.print("The nearest clear column is column ");
       Serial.println(nearestClearCol);
       path += horizontal_first(currentRow, currentCol, dir, currentRow, nearestClearCol, arena);
       //path += "  ";
       currentCol = nearestClearCol;
    } else if(clearRows.indexOf(String(startRow)) == -1) {
         Serial.println("I'm not in a clear row");
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
       Serial.print("The nearest clear column is row ");
       Serial.println(nearestClearRow);
       //Now that we know what the nearest clear row is, we need to move to it.
       path += horizontal_first(currentRow, currentCol, dir, nearestClearRow, currentCol, arena);
       //path += "  ";
       currentRow = nearestClearRow;
    }
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
       return path;
    } else if (vertFirst != "0") {
       //the veritcal path works. 
       path += vertFirst; 
       dir = vertEndDir;
       return path;
    }
    //The following should happen very rarely, if at all. 
    
    //If neither of the above if statements are true, then there is no L shaped path from this clear intersection, so we need to get to a different clear intersection.
    //Any clear intersection in a different corner of the board will work.
    //Arbitrarily choose to do a vertical change. (Move to a different clear intersection in the same column and a different row).
    if((clearRows.length() == 2 and clearRows.indexOf(String(currentRow)) != 1) or clearRows.indexOf(String(currentRow)) < clearRows.length() - 2) {
        //we need to go to clearRows[clearRows.length() - 1]
        //We are in the first or one of the first rows in clearRows, we need to get to the last row in clearRows. 
        path += vertical_first(currentRow, currentCol, dir, clearRows[clearRows.length() - 1], currentCol, arena);
    } else {
       //we are in the last or one of the last rows in clearRows, we need to get to the first row.  
       path += vertical_first(currentRow, currentCol, dir, clearRows[0], currentCol, arena);
    }
    //we are now in the other intersection, time to L-shape. 
    horiEndDir = dir; 
    horiFirst = horizontal_first(currentRow, currentCol, horiEndDir, destRow, destCol, arena);
    vertEndDir = dir;
    vertFirst = vertical_first(currentRow, currentCol, vertEndDir, destRow, destCol, arena);
    
    if (horiFirst != "0") {
       //the horizontal path works.
       path += horiFirst; 
       dir = horiEndDir;
       return path;
    } else if (vertFirst != "0") {
       //the veritcal path works. 
       path += vertFirst; 
       dir = vertEndDir;
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


//This function is deprecated.
String empty_solver(byte startRow, byte startCol, char &dir, byte destRow, byte destCol){
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

/******************************************************************
*******************************************************************
            Nonfunctioning A* code starts here
*******************************************************************
*******************************************************************/

struct Node {
  //This struct is a node in the path the robot takes.
public:
    byte row, col;
    //cost to get to this grid point, cost to get to the destination from this grid point.
    int G, H;
    Node *parent;
    char dir;
    
    Node () {
        parent = NULL;
    }
    Node(byte r, byte c, char d, int GetHereCost, byte endR, byte endC, Node* par) {
        row = r;
        col = c;
        dir = d;
        G = GetHereCost;
        //Predicted path cost is the number of tiles need to move to get to the destination
        H = abs(endR - r) + abs(endC - c);
        //+1 for the turn if a turn is necessary.
        if (!(endR == r or endC == c)) {
            H += 1;
        }
        parent = par;
    }
    ~Node() {
       delete parent; 
    }
};

struct nodeList {
    //This struct contains a list of nodes.
    Node list[8*7];
    //This number of elements in the list.
    byte numElements;
    
    nodeList() {
        numElements = 0;
    }
    
    ~nodeList() {
      delete [] list; 
    }
    
    int inList(byte r, byte c) {
        //If an element with row r and column c is in the list, return its index.
        //otherwise return -1.
        for (byte i = 0; i < numElements; i++){
            if(list[i].row == r and list[i].col == c) {
                return i;
            }
        }
        return -1;
    }
    int removeNode(byte index) {
        //Remove the node in list at index.
        if (index > numElements or numElements == 0) {
            return -1;
        }
        //To remove a node, set it equal to the last node (because order doesn't matter)
        //and decrement lastIndex. (The list now technically has a duplicate
        //but the duplicate should never be accessed.)
        list[index] = list[numElements-1];
        numElements -= 1;
        
        return 0;
    }
    int addNode(Node newNode) {
        //If the node isn't already in the list, adds newNode.
        //Otherwise, updates the existing node.
        //Returns the index of newNode in the list.
        Serial.println("  addNode function called");
        int isPresent = inList(newNode.row, newNode.col);
        
        if ( isPresent != -1) {
            //if the node is already in the list
            Serial.println("  The node is already in the list");
            if (newNode.G < list[isPresent].G) {
                //If the path to get to this new node is shorter than the last recorded path
                //to get to this node, then replace the node in the list with the new node.
                list[isPresent].G = newNode.G;
                list[isPresent].parent = newNode.parent;
                list[isPresent].dir = newNode.dir;
            }
            Serial.println("  returning.");
            return isPresent;
            
        } else {
            //if the node isn't already in the list, add it in and increase the list size.
            Serial.println("  Node not yet in list.");
            list[numElements] = newNode;
            numElements += 1;
            Serial.println("  returning.");
            return numElements - 1;
        }
    }
    int findLowestF() {
        int lowestF = 1000;
        byte fIndex = 0;
        
        for (byte i = 0; i < numElements; i++) {
            if (list[i].G + list[i].H < lowestF){
                lowestF =list[i].G + list[i].H;
                fIndex = i;
            }
        }
        return fIndex;
    }
    
};

String blockedSolverOld(byte startRow, byte startCol, char &dir, byte destRow, byte destCol, char arena[8][7]) {
    //First need to get to a clear intersection.
    //Given an array of clear rows and an array of clear columns.
    //If I'm in a clear row, go to the clear column nearest to mine.
    //If I'm in a clear column, go to the clear row nearest to mine.
    String clearRows = "";
    String clearCols = "";
    byte currentRow = startRow;
    byte currentCol = startCol;
    
    bool rowClear, colClear;
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
    
    //First, determine if we can get there right away. (same clear row as destination or same clear column as destination).
    if ((startRow == destRow) and (clearRows.indexOf(String(startRow)) != -1) or (startCol == destCol and clearCols.indexOf(String(startCol)) != -1)) {
       //If we're in the same clear row as destination or same clear column as destination then we can go straight there, find the path using emptySolver. 
       path = horizontal_first(startRow, startCol, dir, destRow, destCol, arena);
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
           return path;
        } else if (vertFirst != "0") {
           //the veritcal path works. 
           path += vertFirst; 
           dir = vertEndDir;
           return path;
        } 
        //There is no L shaped path.
    }
    
    //If there is no L shaped path, then either we're neither in a clear row or a clear column (i. e. corner case), or we are in one of the 2 and we need to get to a clear intersection.
    
    //If it's not in the same clear row or clear column as the destination already, we need to get to the nearest clear intersection.
    //to do that, we must first find it.
    
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
       Serial.print("The nearest clear column is column ");
       Serial.println(nearestClearCol);
       path += horizontal_first(currentRow, currentCol, dir, currentRow, nearestClearCol, arena);
       currentCol = nearestClearCol;
       
       //from here, there should be an L shaped path to the end.
        
        //check if there's an L shaped path that gets to the end right away. 
        horiEndDir = dir; 
        horiFirst = horizontal_first(currentRow, currentCol, horiEndDir, destRow, destCol, arena);
        vertEndDir = dir;
        vertFirst = vertical_first(currentRow, currentCol, vertEndDir, destRow, destCol, arena);
        
        if (horiFirst != "0") {
           //the horizontal path works.
           path += horiFirst; 
           dir = horiEndDir;
           return path;
        } else if (vertFirst != "0") {
           //the veritcal path works. 
           path += vertFirst; 
           dir = vertEndDir;
           return path;
        } 
   
    } else if (clearCols.indexOf(String(startCol)) == -1) {
       Serial.println("I'm not in a clear column");
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
       Serial.print("The nearest clear column is column ");
       Serial.println(nearestClearCol);
       path += horizontal_first(currentRow, currentCol, dir, currentRow, nearestClearCol, arena);
       path += "  ";
       currentCol = nearestClearCol;
    } else if(clearRows.indexOf(String(startRow)) == -1) {
         Serial.println("I'm not in a clear row");
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
       Serial.print("The nearest clear column is row ");
       Serial.println(nearestClearRow);
       //Now that we know what the nearest clear row is, we need to move to it.
       path += horizontal_first(currentRow, currentCol, dir, nearestClearRow, currentCol, arena);
       path += "  ";
       currentRow = nearestClearRow;
    }
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
       return path;
    } else if (vertFirst != "0") {
       //the veritcal path works. 
       path += vertFirst; 
       dir = vertEndDir;
       return path;
    } else {
      //This should never happen, the robot is lost somehow. We need to decide on what the robot should do here. 
      return "0"; 
    }
}

String solve(byte startRow, byte startCol, char startDir, byte endRow, byte endCol, char arena[8][7]) {
    //this function returns a string that gives directions for the robot to follow to get from the start position to the end position. 
   //robot's start row, robot's start column, robot's start direction, destination row, destination column. 
    //arena is filled with '0' and 'x' and represents the game arena with hoppers on it. 
    
    if(arena[endRow][endCol] == 'x') {
      Serial.print("You're trying to get to a blocked node, dumbass.");
      return "";
    }
    Serial.print("The solve function was called properly\n");
    //list of grid points still to check.
    nodeList openList;
    //Adding the first node to the open list.
    openList.addNode(Node(startRow, startCol, startDir, 0, endRow, endCol, NULL));

    
    //list of grid points that have already been checked.
    nodeList closedList;

    nodeList path;
    Node nextInPath;
    String directions;
    Serial.print("Structs were instantiated.\n");

    
    //Check all squares adjacent to start point and add them in the open list if
    //they are valid places to go. Drop the start square into the closed list.
    
    //Choose the lowest F square in the open list, check all adjacent squares
    //and add them to the open list if they aren't there already.
    //(If they're already in the open list then update them if necessary)
    //Then drop the parent square into the closed list.
    
    //index of the node being checked in openList, index of that same node in closedList, index of the node to be returned.
    byte nodeToCheckIndex, newIndex;
    Node toCheck;
    boolean canGoRight, canGoLeft, canGoUp, canGoDown;
    Serial.println("Commencing while loop");
    while (true) {
        Serial.println("While loop iterated");
        //Find the node with the lowest F.
        nodeToCheckIndex =openList.findLowestF();
        toCheck = openList.list[nodeToCheckIndex];
        
        if(toCheck.row == endRow and toCheck.col == endCol) {
            //if this node is the end node, travel up the path to return it.
            Serial.println("We found the end");
            nextInPath = toCheck;
            while(true) {
                path.addNode(nextInPath);
                if (nextInPath.parent == NULL) {
                    break;
                } else {
                    nextInPath = *(nextInPath.parent);
                }
            }
            //a path has now been found. 
        
            //Iterating through the path from the start point to the end point to create a directions string.
            for (int i = path.numElements - 2; i >= 0; i--) {
              if (path.list[i].dir != path.list[i+1].dir){
                  //If it's not facing in the same direction it was before, need to turn.
                  directions += change_dir(path.list[i+1].dir, path.list[i].dir);
              }
              //Since a turn and a move happen in one step, need to move after the turn was made.
              //(alternatively,no turn was made and just need to move).
              
              //node.dir is the direction the robot was facing to move into this square.
              //If it's moving forwards, += 'w', if it's moving backwards, += 's'.
              if ((path.list[i].dir == 'n' and path.list[i].row - path.list[i+1].row < 0) \
                  or (path.list[i].dir == 's' and path.list[i].row - path.list[i+1].row > 0) \
                  or (path.list[i].dir == 'e' and path.list[i].col - path.list[i+1].col > 0)\
                  or (path.list[i].dir == 'w' and path.list[i].col - path.list[i+1].col < 0)) {
      
                  directions += 'w';
              } else {
                  directions += 's';
              }
          }
            Serial.print("Returning directions.");
            return directions;
        }
        
        //remove toCheck from the openList and add it to closedList
        newIndex = closedList.addNode(toCheck);
        openList.removeNode(nodeToCheckIndex);
        
        //Check to see which nodes are open from this node.

        //you can go right if you're not at the edge and that position isn't blocked.
        canGoRight = (toCheck.col + 1 < 7) and arena[toCheck.row][toCheck.col + 1] != 'x';
        canGoLeft = (toCheck.col - 1 >= 0) and arena[toCheck.row][toCheck.col - 1] != 'x';
        canGoUp = (toCheck.row - 1 >= 0) and arena[toCheck.row - 1][toCheck.col] != 'x';
        canGoDown = (toCheck.row + 1 < 8) and arena[toCheck.row + 1][toCheck.col] != 'x';

        Serial.print("Adding squares to open list\n");        
        //These 4 if statements add the 4 directions to the open list if they are valid points.
        //They also check to see if any of the new grid points added are the destination.
        if (canGoRight) {
            Serial.print("Can go right\n");
            openList.addNode(Node(toCheck.row, toCheck.col + 1, 'e', toCheck.G + 1 + !('e'==toCheck.dir), \
                                                endRow, endCol, &closedList.list[newIndex]));
            Serial.println("Added node to right");
        }
        if (canGoLeft) {
          Serial.print("Can go left\n");
          openList.addNode(Node(toCheck.row, toCheck.col - 1, 'w', toCheck.G + 1+!('w'==toCheck.dir), \
                                                endRow, endCol, &closedList.list[newIndex]));
          Serial.print("Added node to the left\n");  
        }
        if (canGoUp) {
            Serial.print("Can go up\n");
            openList.addNode(Node(toCheck.row-1, toCheck.col, 'n', toCheck.G + 1+!('n'==toCheck.dir), \
                                  endRow, endCol, &closedList.list[newIndex]));
        }
        if (canGoDown) {
            Serial.print("Can go down\n");
            openList.addNode(Node(toCheck.row+1, toCheck.col, 's', toCheck.G + 1+!('s'==toCheck.dir), \
                                   endRow, endCol, &closedList.list[newIndex]));
        }
        Serial.print("Going back to the top.\n");
    }
}

