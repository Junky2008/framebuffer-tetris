CC=gcc
.SUFFIXES:.c
.c.o:
	$(CC) -Wall -g -c $<

build:main.o fbDrawer.o gameControl.o gameDrawer.o
	$(CC) -Wall -o fbTetris $+
	rm main.o fbDrawer.o gameControl.o gameDrawer.o
