#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>
#include "fbDrawer.h"
#include "gameControl.h"

void SetTimer(int sec, void(*proc)(int)) {
	struct itimerval timer;
	timer.it_interval.tv_sec = 1;
	timer.it_interval.tv_usec = 0;
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, NULL);
	signal(SIGALRM, proc);
}


void alarm_handler(int signum) {
	moveDown();
}

static struct termios newtermset, oldtermset;

void noecho(void) {
	tcgetattr(STDIN_FILENO, &oldtermset);
	newtermset = oldtermset;
	newtermset.c_lflag &= ~ICANON;
	newtermset.c_lflag &= ~ECHO;
	newtermset.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &newtermset);
}

void reset_term(void) {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldtermset);
}

int readfd(char *path) {
	char buf;
	int fd = open(path, O_RDONLY);
	read(fd, &buf, 1);
	close(fd);
	return (buf == '0');
}

void setdirection(char *path) {
		int fd = open(path, O_WRONLY);
		write(fd, "in", 2);
		close(fd);
}

void exportgpio(char *gpio) {
	int fd = open("/sys/class/gpio/export", O_WRONLY);
	write(fd, gpio, strlen(gpio));
	close(fd);
}

void initgpio(int num) {
	if(num < 0 || num > 999)
	{
		return;
	}
	char gpio[4];
	memset(gpio, '\0', 4);
	sprintf(gpio, "%d", num);
	exportgpio(gpio);
	char gpiodirection[34];
	memset(gpiodirection, '\0', 34);
	sprintf(gpiodirection, "/sys/class/gpio/gpio%d/direction", num);
	setdirection(gpiodirection);
}

int getgpiovalue(int num) {
	if(num < 0 || num > 999)
	{
		return 0;
	}

	char gpiovalue[30];
	memset(gpiovalue, '\0', 30);
	sprintf(gpiovalue, "/sys/class/gpio/gpio%d/value", num);
	return readfd(gpiovalue);
}

int getch(void) {
	//noecho();
	//ch = getchar();
	//reset_term();

	if(getgpiovalue(125))
	{
		while(getgpiovalue(125));
		return 'q';
	}
	if(getgpiovalue(126))
	{
		while(getgpiovalue(126));
		return 'a';
	}
	if(getgpiovalue(124))
	{
		while(getgpiovalue(124));
		return 'w';
	}
	if(getgpiovalue(9))
	{
		while(getgpiovalue(9));
		return 'd';
	}
	if(getgpiovalue(58))
	{
		while(getgpiovalue(58));
		return 's';
	}

	return '\0';
}

int main(void) {
	initgpio(125);
	initgpio(126);
	initgpio(124);
	initgpio(9);
	initgpio(58);

	openfb("/dev/fb0");
	initGame();
	SetTimer(1,alarm_handler);
	while(1) {
		char c;
		c = getch();
		fflush(NULL);
		switch(c) {
		case 'w':
			rotate();
			break;
		case 'a':
			moveLeft();
			break;
		case 's':
			moveDown();
			break;
		case 'd':
			moveRight();
			break;
		case 'q':
			printf("Game Aborted!!!\n");
			printf("Your score:%d\n",getScore());
			exit(0);
		default:
			break;
		}
	}
	closefb();
	return 0;
}
