CFLAGS = -Wall -pedantic -std=c11 # tutaj można dodawać inne flagi kompilatora
LIBS = -lm # tutaj można dodawać biblioteki
PHASE=placement
INPUTBOARDFILE=board01.txt
OUTPUTBOARDFILE=board01.txt
PENGUINS=3

OBJ =\
  rybka_bot.o
all: rybka_bot

clean:
	rm -f *.o rybka_bot
.c.o:
	$(CC) -c $(INCLUDES) $(CFLAGS) $<

main: $(OBJ)
	$(CC) $(OBJ) $(LIBS) -o test

run:
	./rybka_bot phase=$(PHASE) penguins=$(PENGUINS) inputboardfile=$(INPUTBOARDFILE) outputboardfile=$(OUTPUTBOARDFILE)
