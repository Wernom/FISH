CC = gcc
CFLAGS = -g -Wall -std=c99 -O2 -c -o
LDFLAGS= -g
TARGET = fish cmdlin_test

all : $(TARGET)

$(TARGET) : fish.o cmdline.o cmdexec.o
	$(CC) $(LDFLAGS) $^ -o $@
fish.o : fish.c cmdline.h
	$(CC) $(CFLAGS) $@ $< 
cmdline.o : cmdline.c cmdline.h
	$(CC) $(CFLAGS) $@ $<
cmdline_test.o :cmdline_test.c cmdline.h
	$(CC) $(CFLAGS) $@ $<
cmdexec.o : cmdexec.c cmdexec.h
	$(CC) $(CFLAGS) $@ $<
clean :
	rm -f *.o
mrproper : clean
	rm -f $(TARGET)
