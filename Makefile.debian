# Makefile for ttvplex

CWD = $(shell pwd)
# Location of CLN (http://www.ginac.de/CLN/) source (expected to lie in 'cln' subdirectory)
# CLN_INC = /opt/local/include/
# Location of compiled CLN library
# CLN_LIB = /opt/local/lib/libcln.a
# Location of the compiled GMP library which is required by CLN
GMP_INC = ./libs/gmp/
GMP_LIB = ./libs/gmp/.libs/
GMP_LIB_A = $(GMP_LIB)libgmp.a
GMPXX_LIB_A = $(GMP_LIB)libgmpxx.a 
GMP_LIBS = $(GMP_LIB_A) $(GMPXX_LIB_A) $(CWD)/$(GMP_LIB_A) $(CWD)/$(GMPXX_LIB_A)
GMP_LIB_PATH = 

# Include paths that should be searched
INCLUDE = -I$(GMP_INC) -I./libs/
# C compiler command. We use the GNU C++ compiler
CC = g++
# Compiler flags
CFLAGS = -O2 -g -Wall
# Linker flags
LDFLAGS = -lm $(GMP_LDFLAGS)
# Paths of third party libraries that are used
LIBS = $(GMP_LIB_PATH) 

# Name of the executable
PROGRAMNAME = ttvplex

# Default target invoked by 'make' or 'make default'
default: main.cpp main.o global.o Simplex.o Log.o Matrix.o LPParser.o $(GMPXX_LIB_A)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBS) $(LDFLAGS) -o $(PROGRAMNAME) main.o global.o Simplex.o Log.o Matrix.o LPParser.o $(GMP_LIBS)

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

# Build libs if necessary
$(GMPXX_LIB_A):
	cd libs && ./install-gmp.sh

# Remove locally compiled libraries
clean-libs:
	-rm -rf libs/gmp
	-rm -rf libs/gmp-5.0.1

# Remove object files, executable and documentation
clean:
	-rm -rf docs/* 2>/dev/null
	-rm *.o $(PROGRAMNAME) 2>/dev/null

# Perform all clean operations
clean-all: clean clean-libs

# Generate doxygen project documentation
doxy:
	mkdir -p docs && doxygen doxygen.conf
