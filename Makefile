ex04: main.o weather.o
	gcc main.o weather.o -o ex04
main.o: main.c
	gcc -Wall -Wextra -Wpedantic -std=c11 -c main.c -o main.o
weather.o: weather.c
	gcc -Wall -Wextra -Wpedantic -std=c11 -c weather.c -o weather.o
clear:
	rm ex04 *.o
