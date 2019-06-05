SOURCES = $(wildcard *.c)

###########################################################################

EXECUTABLE = libsemanage-tests
override CFLAGS = -g -Wall -std=gnu99 -I../src -I../include
LDFLAGS += -lcunit -lsemanage -lselinux
OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE) 

$(EXECUTABLE): $(OBJECTS) ../src/libsemanage.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

clean distclean: 
	rm -rf $(OBJECTS) $(EXECUTABLE)

test: all 
	./$(EXECUTABLE) 2>/dev/null

