CC 			=cc

CCFLAGS 	= -g
INCPATH 	=
LIBS 		=


OBJECTS 	= main.o c_timer.o
TARGET 		= main

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $(INCPATH) $<

all:	$(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(CCFLAGS) $(LDFLAGS) $(LIBS)

clean:	
	rm -f $(OBJECTS) $(TARGET)
	rm -f *~ core.* core 