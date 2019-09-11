CC = gcc
.PHONY: default compile build run
default: build run
compile: aasm.c
	$(CC) aasm.c -o aasm
build: main.asm
	./aasm -i main.asm
run: out.asm 
	fasm out.asm
	ld out.o -o out
	./out
