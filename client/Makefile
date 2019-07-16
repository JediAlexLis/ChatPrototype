all: client

client: main.c client.c
	gcc -o client.out main.c client.c -lncurses -pthread

clean:
	rm client.out