# Add your test source files here:
SOURCES = $(sort $(wildcard *.c))
CILS = $(sort $(wildcard *.cil))

###########################################################################

EXECUTABLE = libsemanage-tests
CFLAGS += -g -O0 -Wall -W -Wundef -Wmissing-noreturn -Wmissing-format-attribute -Wno-unused-parameter
override CFLAGS += -I../src -I../include
override LDLIBS += -lcunit -lbz2 -laudit -lselinux -lsepol

OBJECTS = $(SOURCES:.c=.o)
POLICIES = $(CILS:.cil=.policy)

all: $(EXECUTABLE) $(POLICIES) test_module.cil.bz2
	$(MAKE) -C test-modules all

$(EXECUTABLE): $(OBJECTS) ../src/libsemanage.a
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.policy: %.cil
	secilc $*.cil -o $*.policy -f /dev/null

test_module.cil.bz2: test_module.cil
	bzip2 -c test_module.cil >test_module.cil.bz2

clean distclean: 
	rm -rf $(OBJECTS) $(POLICIES) $(EXECUTABLE) test_module.cil.bz2
	$(MAKE) -C test-modules clean

test: all 
	./$(EXECUTABLE)

