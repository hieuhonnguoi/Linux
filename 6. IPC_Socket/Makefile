CC = gcc
CFLAGS = -Wall -O2

DIR_TCP = 6.1.IPv4_Stream_socket
DIR_UDP = 6.2.IPv4_Datagram_Socket
DIR_US  = 6.3.Unix_Stream_Socket
DIR_UD  = 6.4.Unix_Datagram_Socket

SRCS := $(wildcard $(DIR_TCP)/*.c $(DIR_UDP)/*.c $(DIR_US)/*.c $(DIR_UD)/*.c)
BINS := $(SRCS:.c=)

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BINS) *.o *.sock
