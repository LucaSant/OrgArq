all: registros.o main.o
	gcc -o ex1 registros.o main.o

registros.o: registros.h
main.o: registros.h

run:
	./ex1

clean: 
	rm *.o ex1