CC = gcc
CFLAGS = -Wall -g
#CFLAGS = -O2 -DNDEBUG
LDLIBS = -lm
OBJS1 = kadai01/main.o kadai01/func2.o kadai01/optimize2.o
OBJS2 = kadai02/main.o kadai02/regression.o kadai02/optimize3.o
TARGET1 = optimizer1
TARGET2 = optimizer2

.PHONY: all
all: ;

$(TARGET1): $(OBJS1)
	$(CC) -o $@ $^ $(LDLIBS)

$(TARGET2): $(OBJS2)
	$(CC) -o $@ $^ $(LDLIBS)

.PHONY: tmpclean clean
tmpclean:
	rm -f *~
clean: tmpclean
	rm -f $(OBJS1) $(OBJS2) $(TARGET1) $(TARGET2)
