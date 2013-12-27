CXX=g++

# For debugging
CFLAGS = -I. -g
# CFLAGS=-I.

DEPS = realpath.h

# For windows
# OBJ = resolvr.o realpath.o 
OBJ = resolvr.o

all: resolvr

%.o: %.cpp $(DEPS)
	$(CXX) $(CFLAGS) -c -o $@ $<

resolvr: $(OBJ)
	$(CXX) $(LFLAGS) -o $@ $^

clean:
	rm -f *.o resolvr *~

install:
	install resolvr /usr/local/bin

.PHONY: clean all install
