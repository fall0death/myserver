#example:src/bitcask.h src/hello.cc src/bitcask.cc
#	g++ -std=c++11  src/bitcask.cc src/hello.cc -o example

# example: start_server.o server.o
# 	g++ -std=c++11  start_server.o server.o -o example

# server.o:src/server.cc src/server.h
# 	g++ -std=c++11 -g -c src/server.cc 

# start_server.o:src/start_server.cc src/server.h
# 	g++ -std=c++11 -g  -c src/start_server.cc -pthread

# clean:
# 	rm -rf *.o biu

all: example
LIBS = -pthread #-lsocket
example: src/server.cc
	g++ -g -W -Wall $(LIBS) -o $@ $<
