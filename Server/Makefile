INCLUDE_DIR = ./include
SOURCE_DIR = ./source
OBJECT_DIR = ./object

CC = gcc
CFLAGS=-g -I$(INCLUDE_DIR) -pthread -lm
OUT = myhttpd

_DEPS = Arguments.h ErrorCodes.h Http.h MyString.h Server.h ServerCommands.h SharedGlobals.h Threads.h
#pattern matching from  _DEPS to include directory
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = Arguments.o Http.o MyString.o Server.o ServerCommands.o Threads.o
#same pattern matching principe
OBJ = $(patsubst %,$(OBJECT_DIR)/%,$(_OBJ))

.PHONY: clean

############goals#######################

#general rule for all object files
$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

#default goal
$(OUT): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

count:
	wc $(SOURCE_DIR)/*.c $(DEPS)

clean:
	rm -f $(OBJECT_DIR)/*.o ./myhttpd
