# make file 
# The file to be compiled
file = src/main.cpp

# name of the exicutable
executable_name = asteroid-bouncer


# compile the exacutable
all : $(file)
	g++ $(file) -g -lSDL2 -lSDL2_image -lSDL2_mixer -ljsoncpp -lSDL2_ttf -o $(executable_name)