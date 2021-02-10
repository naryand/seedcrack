all: diorite

diorite: main.c ../random.c
	gcc -o $@ main.c ../random.c -O3 -pthread -flto

profile: main.c ../random.c
	gcc -o $@ main.c ../random.c -O3 -pthread -flto -pg

clean:
	rm diorite profile