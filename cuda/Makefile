all: diamond

diamond: main.cu
	nvcc -ccbin clang-8 -o $@ main.cu

cpu: main.c ../random.c
	gcc -o main main.c ../random.c -Wall -Werror -pthread -O3

clean:
	rm diamond main