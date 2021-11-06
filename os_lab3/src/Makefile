CC=gcc
FLAGS =-std=c99 -pthread -w -pipe -O3 -Wextra -Werror -Wall -Wno-sign-compare -pedantic -lm

all: main generator

main: main.c
	$(CC) $(FLAGS) main.c -o main.exe

generator: hex_generator.c
	$(CC) hex_generator.c -o generator.exe