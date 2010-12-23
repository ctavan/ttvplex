CLN_INC = /opt/local/include/
CLN_LIB = /opt/local/lib/libcln.a
INCLUDE = -I$(CLN_INC)
CC = g++
CFLAGS = -O2 -g -Wall
LDFLAGS = -lm
LIBS = /opt/local/lib/libgmp.a $(CLN_LIB)
PROGRAMNAME = ttvplex

# Standardaufruf durch 'make' oder 'make default'
default: main.cpp main.o Simplex.o Log.o
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $(PROGRAMNAME) main.o Simplex.o Log.o $(LIBS)

# Abhaengigkeiten
main.o: main.cpp main.h
Simplex.o: Simplex.cpp Simplex.h
Log.o: Log.cpp Log.h

# Erzeugen oder Aktualisieren einer object-Datei
# $< wird durch den Namen der c-Datei ersetzt
.cpp.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

# Loeschen der object-Dateien und der Programmdatei
clean:
	rm *.o *~ $(PROGRAMNAME) 2>/dev/null
