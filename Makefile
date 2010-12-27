# Makefile for ttvplex

# Location of CLN (http://www.ginac.de/CLN/) source (expected to lie in 'cln' subdirectory)
CLN_INC = /opt/local/include/
# Location of compiled CLN library
CLN_LIB = /opt/local/lib/libcln.a
# Location of the compiled GMP library which is required by CLN
GMP_LIB = /opt/local/lib/libgmp.a

# Include paths that should be searched
INCLUDE = -I$(CLN_INC)
# C compiler command. We use the GNU C++ compiler
CC = g++
# Compiler flags
CFLAGS = -O2 -g -Wall
# Linker flags
LDFLAGS = -lm
# Paths of third party libraries that are used
LIBS = $(GMP_LIB) $(CLN_LIB)

# Name of the executable
PROGRAMNAME = ttvplex

# Default target invoked by 'make' or 'make default'
default: main.cpp main.o Simplex.o Log.o Matrix.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $(PROGRAMNAME) main.o Simplex.o Log.o Matrix.o $(LIBS)

# Dependencies
main.o: main.cpp main.h
Simplex.o: Simplex.cpp Simplex.h
Log.o: Log.cpp Log.h
Matrix.o: Matrix.cpp Matrix.h

# Create or refresh an object-file
# $< will be replaced by the name of the cpp-file
.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

# Remove object files, executable and documentation
clean:
	-rm -rf docs/* 2>/dev/null
	-rm *.o $(PROGRAMNAME) 2>/dev/null

# Generate doxygen project documentation
doxy:
	mkdir -p docs && doxygen doxygen.conf
