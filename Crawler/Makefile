CC = gcc
WFLAGS = -Wextra -Wall -O3
LFLAGS = -lcurl 
SOURCES = url.c set.c hashtable.c pagedir.c curl.c crawler.c
OBJECTS = $(subst .c,.o,$(SOURCES))

.PHONY: clean

all : crawler

crawler : $(OBJECTS)
	$(CC) $(WFLAGS) $(OBJECTS) $(LFLAGS) -o crawler

%.o: %.c
	$(CC) $(WFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) crawler crawler.o ## clean
