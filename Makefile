ex04: main.o weather.o third-party/libs/cjson/libcjson.a
	gcc main.o weather.o -Lthird-party/libs/curl -Lthird-party/libs/cjson  -lcurl -lcjson -o ex04
main.o: main.c
	gcc -Wall -Wextra -Wpedantic -std=c11 -c main.c -o main.o
weather.o: weather.c
	gcc -Wall -Wextra -Wpedantic -std=c11 -Ithird-party/includes -c weather.c -o weather.o
clear:
	rm -f ex04 *.o
third-party/libs/cjson/libcjson.a: third-party/src/cjson/cJSON.c
	gcc -c third-party/src/cjson/cJSON.c -Ithird-party/includes/cjson/ -o third-party/src/cjson/cjson.o
	ar -crs third-party/libs/cjson/libcjson.a third-party/src/cjson/cjson.o
	rm -f third-party/src/cjson/cjson.o
