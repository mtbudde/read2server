CC=gcc
CFLAGS=-lssh2
DEPS = read2serverfunc.h
OBJ = sftptest.o read2serverfunc.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

sftptest: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
