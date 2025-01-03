# Fruit Ninja Multiplayer Network Games
a Fruit Ninja inspired text-based Dragonfly game engine in C++, with real-time mouse control fruit slicing mechanics. 

Features includes:

·      Real-time mouse control fruit slicing mechanics, bomb decoy feature, customized audio, and art assets.      

·      Installed Save Load functionality to preserve game state, including object positions, velocities, and scores. 

·      Multiplayer mode, supporting 2+ players to participate simultaneously for a more engaging experience, including network messaging system, enabling real-time communication between multiple players.  

## Project Overview
**Fruit Ninjas Multiplayer - Latency Compensation** is a project of game that allows multiple players connect to a server and play the game of slicing fruits in real time, compete with each other based on the score displayed on screen. The scoring system: players gains +10 points per cut, -25 if the player lets the fruits out of the screen.

This project adds features to the networked version of **Fruit Ninjas** to handle and compensate for network latency. The game simulates different levels of latency and introduces prediction techniques for sword and fruit movement to mitigate the negative impact of high latency on gameplay.

## Features
- **Simulated Latency:** Artificial latency is added to client-server communication to simulate real-world network conditions.
- **Ping Display:** Displays round-trip latency from client to server in milliseconds, color-coded based on latency thresholds.
- **Sword Movement Prediction:** The client moves the sword immediately based on mouse input without waiting for server confirmation.
- **Fruit Movement Prediction:** Adjusts fruit position on the client based on latency, making slicing more accurate during high latency.
- **Evaluation:** Gameplay is evaluated with and without latency and compensation techniques to assess player performance and experience.

## Platform
- **Tested Platforms:** 
  - Windows 10 and 11, Mac OS X, and Linux (Ubuntu 20.04)
- **Game Engine:** Dragonfly 4.8 (https://dragonfly.wpi.edu)

## Prerequisites
- **Dragonfly Game Engine**
- **Compiler:** 
  - GCC for Linux
  - Clang or XCode for Mac
  - Visual Studio 2022 for Windows

## Compilation Instructions
- **Navigate to the project directory**
cd fruit-ninjas-latency

OR Window (Visual Studio 2022): navigate to vs-2022-fruit-client and vs-2022-fruit-server

- **Compile**
Windows (Visual Studio 2022): Use the provided fruit-client.sln and fruit-server.sln files to open the project and build the server .exe and client .exe files.

## Running the Game 
1. Run server .exe
2. Run at least 2 client .exe files
3. Enter server host name "localhost" for each connecting clients
4. Game start after at least 2 players connected to server
   
![FruitNinja_run](https://github.com/user-attachments/assets/b100dfd0-cbb9-4260-a8ca-12b68bee8681)

## Customization
The game is set to DELAY of 15 ticks (495 ms). To change the DELAY of the game, navigate to util.h to change the value of DELAY, in ticks. 1 tick = 33ms

Player performance (scores) and ping latency data are logged to a text file located in the game directory.

## Authorship  
The dragonfly folder and SFML-2.6 folder are from https://dragonfly.wpi.edu. The Dragonfly engine, the sounds, and most of the sprites are made by professor Mark ClayPool. Other programming files, which enabling the functions, are developed by me.  
