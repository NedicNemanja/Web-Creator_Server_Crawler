INCLUDE_DIR = ./include
SOURCE_DIR = ./source
OBJECT_DIR = ./object

CC = gcc
CFLAGS=-g -I$(INCLUDE_DIR) -pthread -lm
OUT = mycrawler

_DEPS = Arguments.h Crawler.h CrawlerCommands.h HtmlPage.h ErrorCodes.h Http.h LinkQueue.h MyString.h Sockets.h Threads.h
#pattern matching from  _DEPS to include directory
DEPS = $(patsubst %,$(INCLUDE_DIR)/%,$(_DEPS))

_OBJ = Arguments.o Crawler.o CrawlerCommands.o HtmlPage.o Http.o LinkQueue.o MyString.o Sockets.o Threads.o
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
	rm -f $(OBJECT_DIR)/*.o ./mycrawler
