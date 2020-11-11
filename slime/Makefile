all: slime

slime: main.c slime.c ../random.c
	gcc -o $@ main.c slime.c ../random.c -O3 -pthread -flto

clean:
	rm slime