# make file 
# The file to be compiled
file = src/main.cpp

# name of the exicutable
executable_name = asteroid-bouncer


# compile the exacutable
all : $(file)
	g++ $(file) -g -I/usr/include/eigen3 -lSDL2 -lSDL2_image -lnoise -ljsoncpp -o $(executable_name)