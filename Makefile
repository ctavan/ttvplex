# Makefile for ttvplex

# Location of CLN (http://www.ginac.de/CLN/) source (expected to lie in 'cln' subdirectory)
# CLN_INC = /opt/local/include/
# Location of compiled CLN library
# CLN_LIB = /opt/local/lib/libcln.a
# Location of the compiled GMP library which is required by CLN
GMP_INC = /opt/local/include/
GMP_LIB = /opt/local/lib/libgmp.a /opt/local/lib/libgmpxx.a

# Include paths that should be searched
INCLUDE = -I$(GMP_INC) -I./
# C compiler command. We use the GNU C++ compiler
CC = g++
# Compiler flags
CFLAGS = -O2 -g -Wall
# Linker flags
LDFLAGS = -lm
# Paths of third party libraries that are used
LIBS = $(GMP_LIB)

# Name of the executable
PROGRAMNAME = ttvplex

# Default target invoked by 'make' or 'make default'
default: main.cpp main.o global.o Simplex.o Log.o Matrix.o LPParser.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $(PROGRAMNAME) main.o global.o Simplex.o Log.o Matrix.o LPParser.o $(LIBS)

# Dependencies
main.o: main.cpp main.h
global.o: global.cpp global.h
Simplex.o: Simplex.cpp Simplex.h
Log.o: Log.cpp Log.h
Matrix.o: Matrix.cpp Matrix.h
LPParser.o: LPParser.cpp LPParser.h

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
