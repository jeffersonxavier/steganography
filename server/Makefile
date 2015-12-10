SRC_DIR = src

CC = g++

CFLAGS = -W -Wall -pedantic -ansi -std=c++11
INCLUDES = -Iinclude

.PHONY: clean depend

all:
	$(CC) -o server $(INCLUDES) $(SRC_DIR)/*.cpp $(CFLAGS) -lssl -lcrypto

clean:
	@echo Cleaning...
	@rm -rf server *~
	@echo Done!
