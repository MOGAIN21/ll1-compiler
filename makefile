# Compiler to use
CC = gcc

# Compiler flags: 
# -Wall and -Wextra enable standard warnings to help catch bugs
# -g adds debugging information (useful for gdb/valgrind)
CFLAGS = -Wall -Wextra -g

# The name of your final executable
TARGET = compiler

# List of object files required to build the target
OBJS = driver.o Parser.o utils.o lexer.o

# The default rule that runs when you just type 'make'
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rules to compile each .c file into a .o (object) file
driver.o: driver.c ParserHeader.h
	$(CC) $(CFLAGS) -c driver.c

Parser.o: Parser.c ParserHeader.h
	$(CC) $(CFLAGS) -c Parser.c

utils.o: utils.c utils.h ParserHeader.h
	$(CC) $(CFLAGS) -c utils.c

lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

# Rule to clean up compiled files (run 'make clean')
clean:
	rm -f $(OBJS) $(TARGET)