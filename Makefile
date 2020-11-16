###### PATHS #######
HEADPATH = head
BINPATH  = .
OBJPATH	 = obj
SRCPATH	 = src
DOCPATH  = doc

vpath %.c ./src
vpath %.o ./obj

###### COMPIL ######
CC = gcc
CPPFLAGS =-Ihead
CFLAGS   =-std=gnu11 -pedantic
LDLIBS   =-lm

DEBUG  = YES

ifeq ($(DEBUG),YES)
	CPPFLAGS +=
	CFLAGS   +=-Wall -Wextra -g
	LDLIBS   +=
	TEXTE 	 +=" debug"

else
	CPPFLAGS +=
	CFLAGS   +=-O2 
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
	@echo "${LCYAN}\nExécutable généré en mode"$(TEXTE)".${NC}"
	
compile: $(OBJ)
	@echo "${LCYAN}\n\n--------------- Rule" $@ "---------------${NC}"
	$(CC) $(CPPFLAGS) $(SENDER_OBJS) -o sender $(CFLAGS) $(LDLIBS)
	$(CC) $(CPPFLAGS) $(RECVER_OBJS) -o recver $(CFLAGS) $(LDLIBS)
	

$(OBJPATH)/%.o: $(SRCPATH)/%.c | OBJ
	@echo "${LCYAN}\n\n--------------- Rule" $@ "---------------${NC}"
	$(CC) $(CPPFLAGS) -o $@ -c $< $(CFLAGS) $(LDLIBS)
	
test:
	bash tests/batterie.sh tests .

tests: test

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
SENDER_OBJS = $(OBJPATH)/sender.o $(OBJPATH)/tourniquet.o $(OBJPATH)/common.o
RECVER_OBJS = $(OBJPATH)/recver.o $(OBJPATH)/common.o
sender.o : common.h sender.h tourniquet.h
recver.o : common.h recver.h
tourniquet.o : common.h tourniquet.h
common.o : common.h

###### MISC ######
LCYAN=\033[1;36m
NC=\033[0m # No Color

