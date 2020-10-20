###### PATHS #######
HEADPATH = .
BINPATH  = .
OBJPATH	 = obj
SRCPATH	 = src
DOCPATH  = doc

vpath %.c ./src
vpath %.o ./obj

###### COMPIL ######
CC = gcc
CPPFLAGS =
CFLAGS   =
LDLIBS   =

DEBUG  = YES

ifeq ($(DEBUG),YES)
	CPPFLAGS +=
	CFLAGS   +=-Wall -Wextra -Werror -g -pedantic -ansi -std=c11
	LDLIBS   +=
	TEXTE 	 +=" debug"

else
	CPPFLAGS +=
	CFLAGS   +=-O2 -pedantic -ansi -std=c11
	LDLIBS   +=
	TEXTE 	 +=" release"
endif

###### FILES ######
EXEC = sender recver

SRC  = $(wildcard $(SRCPATH)/*.c)
HEAD = $(wildcard $(HEADPATH)/*.h)
OBJ  = $(SRC:$(SRCPATH)/%.c=$(OBJPATH)/%.o)
DIST = $(SRC) $(HEAD) Makefile

###### TARGETS ######
all: compile
	@echo "\nExécutable généré en mode"$(TEXTE)"."
	
compile: $(OBJPATH)/sender.o $(OBJPATH)/recver.o
	$(CC) $(CPPFLAGS) $(OBJPATH)/sender.o -o sender $(CFLAGS) $(LDLIBS)
	$(CC) $(CPPFLAGS) $(OBJPATH)/recver.o -o recver $(CFLAGS) $(LDLIBS)
	@echo "\n----Rule" $@ "----"

$(OBJPATH)/%.o: $(SRCPATH)/%.c | OBJ
	$(CC) $(CPPFLAGS) -o $@ -c $< $(CFLAGS) $(LDLIBS)
	@echo "\n----Rule" $@ "----"

test:
	bash tests/batterie.sh tests .

.PHONY: clean mrproper

clean:
	rm -rf $(OBJPATH)/*.o
	rm -rf $(EXEC)
	
clean_doc:
	rm -rf ./doc

clean_dist:
	rm -rf jduprat_algoRes.tar.xz
	rm -rf jduprat_algoRes.zip

mrpropre: clean clean_doc
	rm -rf $(EXEC)
	rm -rf $(OBJPATH)


## Depositories creation ##
OBJ:
	@mkdir -p $(OBJPATH)
	
DOC:
	@mkdir -p $(DOCPATH)

######## DOC ########

#### DEPENDANCIES ####
######
