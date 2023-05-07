# path
MODULES = modules

# Compile options. Το -I<dir> χρειάζεται για να βρει ο gcc τα αρχεία .h
# CFLAGS = -Wall -Werror -g -I$(INCLUDE)

# Αρχεία .o, εκτελέσιμο πρόγραμμα και παράμετροι
OBJS = main.o $(MODULES)/shell.o $(MODULES)/redirection.o $(MODULES)/history.o $(MODULES)/aliases.o $(MODULES)/wildcharacters.o $(MODULES)/background.o  $(MODULES)/pipes.o $(MODULES)/comands.o $(MODULES)/signals.o
#$(MODULES)/mapelements.o $(MODULES)/entity.o $(MODULES)/npc.o $(MODULES)/functions.o $(MODULES)/avatar.o
 
EXEC = exec
ARGS =

# -g -fsanitize=address
$(EXEC): $(OBJS)
	gcc $(OBJS) -o $(EXEC) -lreadline

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)