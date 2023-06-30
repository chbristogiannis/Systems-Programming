# Specify the compiler and its flags
CC = gcc
CFLAGS = -Wall #-Werror
LD = -lreadline -lpthread

#path
SERVER_PATH = server
SERVER_PATH_BUFFER = server/buffer
SERVER_PATH_HELPERS = server/helpers
CLIENT_PATH = client

# Specify the source files for each program
SERVER_SRCS = $(SERVER_PATH)/main.c $(SERVER_PATH)/poller.c $(SERVER_PATH_BUFFER)/buffer.c $(SERVER_PATH_HELPERS)/helpers.c
CLIENT_SRCS = $(CLIENT_PATH)/main.c $(CLIENT_PATH)/pollSwayer.c

# Specify the object files for each program
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)

# Specify the output executables
SERVER = poller
CLIENT = pollSwayer


# Compile
$(SERVER): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_OBJS) $(LD)

$(CLIENT): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_OBJS) $(LD)

# Clean all .o files
clean:
	rm -f $(SERVER) $(CLIENT) $(SERVER_OBJS) $(CLIENT_OBJS)
