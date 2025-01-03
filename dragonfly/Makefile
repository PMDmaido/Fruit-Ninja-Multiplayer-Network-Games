#
# Makefile for game using Dragonfly
#
# Copyright Mark Claypool and WPI, 2024
#
# 'make depend' to generate new dependency list
# 'make clean' to remove all constructed files
# 'make' to build executable
#
# Variables of interest:
#   GAMESRC is the source code files for the game
#   GAME is the game main() source
#

#### Adjust these as appropriate for build setup. ###
HOME= /home/claypool
DF= $(HOME)/proj/dragonfly/dragonfly

### Fixed for all setups. ###
SFMLLIB= -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
CC= g++
CFLAGS= -std=c++17

### Uncomment only 1 of the below! ###

# 1) Uncomment below for Linux (64-bit)
DFLIB= -ldragonfly-x64-debug
LINKLIB= $(SFMLLIB) $(DFLIB) 
LINKDIR= -L$(DF) -L$(HOME)/src/SFML/lib   
INCDIR= -I$(DF) -I$(HOME)/src/SFML/include 

# 2) Uncomment below for Mac (64-bit)
#DFLIB= -ldragonfly-mac64
#LINKLIB= $(SFMLLIB) $(DFLIB)
#LINKDIR= -L/usr/local/Cellar/sfml/2.4.0/lib/ -L$(DF)
#INCDIR= -I/usr/local/Cellar/sfml/2.4.0/include/ -I$(DF)

######

GAMESRC= \

GAME= game.cpp
EXECUTABLE= game
OBJECTS= $(GAMESRC:.cpp=.o)

all: $(EXECUTABLE) Makefile 

$(EXECUTABLE): $(OBJECTS) $(GAME) $(GAMESRC) Makefile
	$(CC) $(GAME) $(OBJECTS) $(CFLAGS) -o $@ $(INCDIR) $(LINKDIR) $(LINKLIB) 

.cpp.o: 
	$(CC) -c $(INCDIR) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) core *.log *~
