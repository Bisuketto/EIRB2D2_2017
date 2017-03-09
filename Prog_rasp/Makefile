CC=gcc
CFLAGS=-std=c99 -Wall
EXEC=main clean
SRC=$(wildcard *.c)
OBJ=$(SRC:.c=.o)

all : $(EXEC)

main : $(OBJ)
	@echo "Compilation test"
	@$(CC) $(CFLAGS) link.o test_unitaire_link.o -o test_link
	@$(CC) $(CFLAGS) link.o set_link.o test_unitaire_set_link.o -o test_set_link
	@$(CC) $(CFLAGS) link.o set_link.o algo.o -o test

link.o : link.h
set_link.o: link.h set.h

%.o : %.c
	@$(CC) $(CFLAGS) -o $@ -c $< 

clean : executer
	@echo "Suppression des fichiers crées"
	@rm $(OBJ)

executer : 
	@echo "Execution des tests :"
	@echo '_____________________________________'
	@echo 'Test link:'
	@./test_link
	@echo '_____________________________________'
	@echo 'Test set_link:'
	@./test_set_link


