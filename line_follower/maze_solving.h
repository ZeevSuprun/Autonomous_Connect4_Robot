#include <Arduino.h>

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
};

struct nodeList {
    //This struct contains a list of nodes.
    Node list[8*7];
    //This number of elements in the list.
    byte numElements;
    
    nodeList() {
        numElements = 0;
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
    byte addNode(Node newNode) {
        //If the node isn't already in the list, adds newNode.
        //Otherwise, updates the existing node.
        //Returns the index of newNode in the list.
        int isPresent = inList(newNode.row, newNode.col);
        
        if ( isPresent != -1) {
            //if the node is already in the list
            if (newNode.G < list[isPresent].G) {
                //If the path to get to this new node is shorter than the last recorded path
                //to get to this node, then replace the node in the list with the new node.
                list[isPresent].G = newNode.G;
                list[isPresent].parent = newNode.parent;
                list[isPresent].dir = newNode.dir;
            }
            return isPresent;
            
        } else {
            //if the node isn't already in the list, add it in and increase the list size.
            list[numElements] = newNode;
            numElements += 1;
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
//Function definitions:

//This function finds a path from point A to point B and returns a string of directions for the robot to follow that path. 
String solve(byte startRow, byte startCol, char startDir, byte endRow, byte endCol, char arena[8][7]);
//This function finds a path from point A to point B in an empty grid. It is deprecated.
String empty_solver(byte startRow, byte startCol, char dir, byte destRow, byte destCol);
//this function determines shortest path to move from direction A to direction B. 
String change_dir (char startDir, char endDir);

String solve(byte startRow, byte startCol, char startDir, byte endRow, byte endCol, char arena[8][7]) {
    //this function returns a string that gives directions for the robot to follow to get from the start position to the end position. 
   //robot's start row, robot's start column, robot's start direction, destination row, destination column. 
    //arena is filled with '0' and 'x' and represents the game arena with hoppers on it. 
    
    //list of grid points still to check.
    nodeList openList;
    //Adding the first node to the open list.
    openList.addNode(Node(startRow, startCol, startDir, 0, endRow, endCol, NULL));
    
    //list of grid points that have already been checked.
    nodeList closedList;
    nodeList path;
    Node nextInPath;
    
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
    
    while (true) {
        //Find the node with the lowest F.
        nodeToCheckIndex =openList.findLowestF();
        toCheck = openList.list[nodeToCheckIndex];
        
        if(toCheck.row == endRow and toCheck.col == endCol) {
            //if this node is the end node, travel up the path to return it.
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
            String directions;
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
        
        //These 4 if statements add the 4 directions to the open list if they are valid points.
        //They also check to see if any of the new grid points added are the destination.
        if (canGoRight) {
            openList.addNode(Node(toCheck.row, toCheck.col + 1, 'e', toCheck.G + 1 + !('e'==toCheck.dir), \
                                                endRow, endCol, &closedList.list[newIndex]));
        }
        if (canGoLeft) {
            openList.addNode(Node(toCheck.row, toCheck.col - 1, 'w', toCheck.G + 1+!('w'==toCheck.dir), \
                                                endRow, endCol, &closedList.list[newIndex]));
        }
        if (canGoUp) {
            openList.addNode(Node(toCheck.row-1, toCheck.col, 'n', toCheck.G + 1+!('n'==toCheck.dir), \
                                  endRow, endCol, &closedList.list[newIndex]));
        }
        if (canGoDown) {
            openList.addNode(Node(toCheck.row+1, toCheck.col, 's', toCheck.G + 1+!('s'==toCheck.dir), \
                                   endRow, endCol, &closedList.list[newIndex]));
        }
    }
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

//This function is deprecated.
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


