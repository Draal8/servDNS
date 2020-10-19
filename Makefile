###### PATHS #######
HEADPATH = .
BINPATH  = .
OBJPATH	 = .
SRCPATH	 = .

###### COMPIL ######
CPPFLAGS =
CFLAGS   =
LDLIBS   =

DEBUG  = YES
MODE   = GRAPH
LIB    = YES
STATIC = YES

CC = gcc

ifeq ($(DEBUG),YES)
	CPPFLAGS +=
	CFLAGS   +=-Wall -Wextra -g -pedantic -ansi -std=c11
	LDLIBS   +=
	TEXTE 	 +=" debug"

else
	CPPFLAGS +=
	CFLAGS   +=-O2 -pedantic -ansi -std=c11
	LDLIBS   +=
	TEXTE 	 +=" release"
endif

###### FILES ######
EXEC = $(BINPATH)/resolvDNS

SRC  = $(wildcard $(SRCPATH)/*.c)
HEAD = $(wildcard $(HEADPATH)/*.h)
OBJ  = $(SRC:$(SRCPATH)/%.c=$(OBJPATH)/%.o)
DIST = $(SRC) $(HEAD) Makefile Doxyfile

###### TARGETS ######
all: $(EXEC)
	@echo "\nExécutable généré en mode"$(TEXTE)"."
	
$(EXEC): $(OBJPATH)/main.o
	$(CC) $(CPPFLAGS) -o $@ $^ $(CFLAGS) $(LDLIBS)
	@echo "\n----Rule " $@ "----"

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	$(CC) $(CPPFLAGS) -o $@ -c $< $(CFLAGS) $(LDLIBS)
	@echo "\n----Rule " $@ "----"

test:
	bash batterie.sh

clean:
	rm -rf $(OBJPATH)/*.o
	rm -rf $(EXEC)

## Depositories creation ##

######## DOC ########

#### DEPENDANCIES ####

######
