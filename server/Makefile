all: server

server: main.o server.o
	gcc -o server.out main.o server.o `xml2-config --cflags --libs` -lncurses -pthread

server.o: server.c
	gcc -c server.c `xml2-config --cflags --libs` -lncurses -pthread

main.o: main.c
	gcc -c main.c `xml2-config --cflags --libs` -lncurses -pthread

clean:
	rm server.out
	rm main.o
	rm server.o