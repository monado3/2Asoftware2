CC = gcc
CFLAGS = -Wall
OBJS1 = main.o encode1.o
OBJS2 = huffman1/main.o

huffman: $(OBJS1)
	$(CC) -o $@ $^ $(LDLIBS)

huffman1: $(OBJS2)
	$(CC) -o $@ $^ $(LDLIBS)

.PHONY: tmpclean clean
tmpclean:
	rm -f *~
clean: tmpclean
	rm -f $(OBJS) huffman
