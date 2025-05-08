# Compiler and Flags
CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
# -Wall -g -m64
LIBS = `pkg-config --libs gtk+-3.0`
DATASHEET = student.xls

# Executable Name
EXECUTABLE = myapp

# Source Files
SOURCE = Main.c Login.c Home.c HeaderMenu.c AddRecord.c ListRecord.c SaveRecord.c ModifyRecord.c DeleteRecord.c SortRecord.c

# Object Files
OBJECTS = $(SOURCE:.c=.o)

# Header Files
HEADERS = header.h

# Rule to create the executable
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(EXECUTABLE) $(LIBS)

# Rule to compile .c files into .o files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(DATASHEET)