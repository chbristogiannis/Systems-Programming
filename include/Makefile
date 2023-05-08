# path
MODULES = modules
INCLUDE = 

# flags
CFLAGS = -Wall

# libraries
LD = -lreadline

# object files
OBJS =  main.o $(MODULES)/history.o $(MODULES)/aliases.o $(MODULES)/signals.o $(MODULES)/shell.o $(MODULES)/redirection.o  $(MODULES)/wildcharacters.o $(MODULES)/background.o  $(MODULES)/pipes.o $(MODULES)/comands.o 

# name of exec
EXEC = main

# args
ARGS =

# compiler
CC = gcc


$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LD)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)

