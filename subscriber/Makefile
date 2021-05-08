CC=gcc

LIBS=-lmicrocdr -lmicroxrcedds_client

sub:mainonesession.o HelloWorld.o
	$(CC) -o $@ $^ $(LIBS)

mainonesession.o:mainonesession.c
	$(CC) -c $^
HelloWorld.o:HelloWorld.c
	$(CC) -c $^

clean:
	rm -rf sub 
	rm -rf *.o