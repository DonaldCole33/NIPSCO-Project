#this make file is mostly for compiling on BigRed II
#using MPI and their local compiler cc

COMPILER=cc	#compiler
CFLAGS=-c	#Compile Flags
EXE=Cor		#App Name
LDFLAGS=	#Linker Flags
SOURCES=Distributed\ Computing\ Project.c data.c hash.c libcsv.c
OBJECTS=$(SOURCES:.c=.o)
SOURCE="./src"

all:	$(EXE) 

$(EXE): Distributed\ Computing\ Project.o data.o libcsv.o hash.o
	$(COMPILER) $(LDFLAGS) $(OBJECTS) -o $@

Distributed\ Computing\ Project.o:
	$(COMPILER) $(CFLAGS) Distributed\ Computing\ Project.c

data.o:
	$(COMPILER) $(CFLAGS) data.c

hash.o:
	$(COMPILER) $(CFLAGS) hash.c

libcsv.o:
	$(COMPILER) $(CFLAGS) libcsv.c

clean:
	rm *.o
