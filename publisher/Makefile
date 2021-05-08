CC=g++

LIBS=-lmicrocdr -lmicroxrcedds_client

pub:main.o HelloWorld.o device.o
	$(CC) -o $@ $^ $(LIBS)


main.o:main.cpp
	$(CC) -c $^
HelloWorld.o:HelloWorld.c
	$(CC) -c $^
device.o: device.cpp
	$(CC) -c $^
clean:
	rm -rf pub 
	rm -rf *.o