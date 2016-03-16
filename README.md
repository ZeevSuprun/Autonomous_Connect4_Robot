# Autonomous Connect-4 Robot

This repository contains the code used for an Autonomous Robog design project. It was uploaded to an arduino and used to control a robot that needed to navigate a grid, pick up ping-pong balls and place them into a connect 4 gameboard, while competing against another robot that was 

The dipSwitches folder contains a sketch that reads from the dip switches and nothing else. These switches were used to input the position of 2 of the ping-pong ball containers, since they could have been in one of several positions. 
The grid_sensing_array folder contains a sketch that takes input from the grid sensors and nothing else. 
The two differentialSteering folders contain versions of differentialSteering.
line_followerOld contains a script that combines grid sensing and differential steering in order to (attempt to) line follow. This script is now deprecated. 

The purpose of the above code was to test new features before adding them to the main program, which is in the line_follower folder.

the line_follower folder contains everything responsible for controlling the actual autonomous connect 4 robot.

the line_follower_tournament folder contains the version of the code that was used for the final competition. 
