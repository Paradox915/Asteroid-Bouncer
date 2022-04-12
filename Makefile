# make file
# The file to be compiled
file = src/main.cpp

# name of the exicutable
executable_name = asteroid-bouncer

# compile the exacutable
all : $(file)
	g++ $(file) -w -lSDL2 -o $(executable_name)