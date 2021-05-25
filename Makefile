LIBCURL = 1

EXEC = ip-notificator
OBJDIR = ./obj/
SRCDIR = ./src/
ifeq ($(PREFIX),)
    PREFIX = /usr/local
endif

#                                                          execution time|code size|memory usage|compile time
#-O0 		optimization for compilation time (default) 		+ 	+ 	- 	-
#-O1 or -O 	optimization for code size and execution time 		- 	- 	+ 	+
#-O2 		optimization more for code size and execution time 	-- 	  	+ 	++
#-O3 		optimization more for code size and execution time 	--- 	  	+ 	+++
#-Os 		optimization for code size 	  				-- 	  	++
#-Ofast 	O3 with fast none accurate math calculations 		--- 	  	+ 	+++

#CC = arm-linux-gnueabihf-gcc
#-mcpu=arm6 -mfpu=vfp
CC = gcc

LDFLAGS = -lm `pkg-config --libs uuid`
COMMON = -Iexternal/klib -Isrc/ `pkg-config --cflags uuid`
#-D_POSIX_C_SOURCE=199309L fixes CLOCK_REALTIME error on pi zero 
CFLAGS = -pthread -O3 -fPIC -Wall -Wno-implicit-function-declaration -Wno-unused-function -DNDEBUG -std=c11 -D_POSIX_C_SOURCE=199309L
OBJ = utils.o main.o

ifeq ($(LIBCURL), 1) 
    COMMON += `pkg-config --cflags libcurl`
    LDFLAGS += `pkg-config --libs libcurl`
    OBJ += http.o
endif

OBJS = $(addprefix $(OBJDIR), $(OBJ))

all: clean obj $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(CC) $(COMMON) $(CFLAGS) -c $< -o $@

obj:
	mkdir -p obj

.PHONY: install
install: $(EXEC)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 111 $(EXEC) $(DESTDIR)$(PREFIX)/bin

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC)


.PHONY: clean
clean:
	rm -rf $(OBJDIR)
