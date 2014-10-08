.SUFFIXES:	.o .c .cpp .cc .C .a


RM = -rm


.cpp.o:	
	$(C++) $(CCFLAGS) $*.cpp


#############################################################

C++        = g++
CCFLAGS    = -c -I./include
LDFLAGS    = -lGL -lGLU -lglut 


VPATH   = obj

OBJECTS1 = Grid.o Isoline.o Isolines.o UniformGrid.o ScalarAttributes.o ColormapRenderer.o MarchingSquares.o 

all: default

default: marchingSquares

marchingSquares: $(OBJECTS1) 
	-mv $(OBJECTS1) $(VPATH)
	cd obj ; $(C++) -o ../marchingSquares $(OBJECTS1) $(LDFLAGS); cd .

clean:
	$(RM) $(VPATH)/*.o marchingSquares
 

