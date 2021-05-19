CC=gcc

LIBS=-lmicrocdr -lmicroxrcedds_client

all: sub pub

sub:sub.o HelloWorld.o ssdp_messageQ.o
	$(CC) -o $@ $^ $(LIBS)

pub:pub.o HelloWorld.o ssdp_messageQ.o
	$(CC) -o $@ $^ $(LIBS)


%.o:%.c
	$(CC) -c -o $@ $<


clean: 
	rm -rf *.o
	rm sub pub
