
CFLAGS=-g -Wall #-static -fpic

LIB_SRC = ratnet.c ratnet_util.c ratnet_linux.c ratnet_win32.c ratnet_buffer.c
LIB_OBJ = ratnet.o ratnet_util.o ratnet_linux.o ratnet_win32.o ratnet_buffer.o

all: ratnet demo


demo:
	gcc -o server $(CFLAGS) simple_server.c ratnet.a
	gcc -o client $(CFLAGS) simple_client.c ratnet.a


ratnet: 
	gcc -c $(CFLAGS) $(LIB_SRC)
	ar r ratnet.a $(LIB_OBJ)


clean:
	rm -rf *.o *.a server client
	
