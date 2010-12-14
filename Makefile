CC = g++ -O3
CFLAGS = -o
LIBS = -g -Wall -lm
PROGRAMNAME = ttvplex

# Standardaufruf durch 'make' oder 'make default'
default: main.cpp main.o Simplex.o Log.o
	$(CC) $(CFLAGS) $(PROGRAMNAME) main.o Simplex.o Log.o $(LIBS)

# Abhaengigkeiten
main.o: main.cpp main.h
Simplex.o: Simplex.cpp Simplex.h
Log.o: Log.cpp Log.h

# Erzeugen oder Aktualisieren einer object-Datei
# $< wird durch den Namen der c-Datei ersetzt
.cpp.o:
	$(CC) -c -g -Wall $<

# Loeschen der object-Dateien und der Programmdatei
clean:
	rm *.o *~ $(PROGRAMNAME) 2>/dev/null
