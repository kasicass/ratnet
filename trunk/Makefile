
CFLAGS=-g -Wall #-static -fpic

LIB_SRC = ratnet.c ratnet_util.c ratnet_linux.c ratnet_win32.c
LIB_OBJ = ratnet.o ratnet_util.o ratnet_linux.o ratnet_win32.o

all: ratnet demo


demo:
	gcc -o server simple_server.c ratnet.a
	gcc -o client simple_client.c ratnet.a


ratnet: 
	gcc -c $(CFLAGS) $(LIB_SRC)
	ar r ratnet.a $(LIB_OBJ)


clean:
	rm -rf *.o *.a server client
	
