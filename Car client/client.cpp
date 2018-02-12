#include <iostream>
#include <wiringPi.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/tcp.h>

#define server_port 0000
#define server_name "hello.com"

int X, Y;
int analize(char[], int);
void report(const char[]);
int chartoint(char, char, char);
void controls(int, int);
int ser_start();
void wire_start();
void check(int, const char*);
void error(const char *);
int main() {
	char buf[256];
	int socket_fd;
	while (true) {
		wire_start();
		std::string buffer;
		int n;
		do {
			socket_fd = ser_start();
		} while (socket_fd < 0);
		do {
			bzero(buf, 256);
			n = write(socket_fd, "CON", sizeof("CON"));
			check(n, "Unable to write to server (CON)");
			if (n < 0)
				break;
			n = read(socket_fd, buf, sizeof(buf));
			check(n, "Unable to read from server");
			if (n < 0)
				break;
			buffer = buf;
			report("Waiting 3 seconds...");
			sleep(3);

		} while (buffer != "200 OK");
		if (buf[0] == '2' && buf[4] == 'O') {
			report("Recieved 200 OK");
			while (true) {
				report("Second while loop!");
				bzero(buf, 255);
				int n = 0;
				do {
					report("Reading from server");
					n = read(socket_fd, buf, sizeof(buf));
				} while (n < 0);
				n = analize(buf, socket_fd);
				if (n)
					break;
			}
		}
	}
	close(socket_fd);
	error("Broke the inf. while loop?!");
	exit(0);
	kill(getpid(), SIGTERM);
}
int ser_start() {
	report("Starting client");
	int n;
	int socket_fd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	check(socket_fd, "Unable to open socket");
	server = gethostbyname(server_name);
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host as %s\n", server_name);
		exit(0);
	}
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(server_port);
	n = connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	check(n, "Could not connect to the server");
	if (n > 0) {
		report("Sucessfully connected to server!");
	}
	return socket_fd;
}
int analize(char buf[], int socket) {
	std::cout << "Recieved message: " << buf << std::endl;
	int n;
	char loc = buf[0];
	char com[2] = { buf[1], buf[2] };
	if (loc == 'C') {
		if (com[0] == 'P' && com[1] == 'I') {
			n = write(socket, "CPO", 3);
			check(n, "Could not write to server! (CPI)");
		}else
		if (com[0] == 'E' && com[1] == 'D') {
			X = 0;
			y = 0;
			close(socket);
			return 1;
		}
	} 
	// UDP server's job
	else
		if (loc == 'D') {
			std::cout << "For now: " << std::endl << "X: " << X << "Y: " << Y;
			if (strlen(buf) == 11) {
				X = chartoint(buf[2], buf[3], buf[4], buf[5]);
				Y = chartoint(buf[7], buf[8], buf[9], buf[10]);
			}
			else {
				int pos;
				for (int s = 0; s < (strlen (buf)-1) / 5; s++) {
					pos = (s * 5) + 1;
					switch (buf[pos]) {
					case 'X':
						X = chartoint(buf[pos + 1], buf[pos + 2], buf[pos + 3], buf[pos + 4]); break;
					case 'Y':
						Y = chartoint(buf[pos + 1], buf[pos + 2], buf[pos + 3], buf[pos + 4]); break;
					}
				}
			}
			controls(X, Y);
		}
	// UDP server's job (END)
	bzero(buf, 255);
	return 0;
}

// Define Pins
#define int1 0
#define int2 2
#define int3 3
#define int4 12
#define ena 1
#define enb 7
void controls(int X, int Y) {
	std::cout << "X: " << X << std::endl << "Y: " << Y << std::endl;
	if(X > 0){
		digitalWrite(int3, LOW);
		digitalWrite(int4, HIGH);
	}
	else if(X < 0){
		digitalWrite(int3, HIGH);
		digitalWrite(int4, LOW);
	}
	else{
		digitalWrite(int3, LOW);
		digitalWrite(int4, LOW);
	}
	pwmWrite(enb, X);
	if(Y > 0){
		digitalWrite(int1, HIGH);
		digitalWrite(int2, LOW);
	}
	else if(Y < 0){
		digitalWrite(int1, LOW);
		digitalWrite(int2, HIGH);
	}
	else{
		digitalWrite(int1, LOW);
		digitalWrite(int2, LOW);
	}
	pwmWrite(ena, Y);
}

void wire_start() {
	wiringPiSetup();
	pinMode(int1, OUTPUT);
	pinMode(int2, OUTPUT);
	pinMode(int3, OUTPUT);
	pinMode(int4, OUTPUT);
	pinMode(ena, PWM_OUTPUT);
	pinMode(enb, PWM_OUTPUT);
	X = 0;
	Y = 0;
}

int chartoint(char num1, char num2, char num3, char num4) {
	int ret = num4 - '0';
	ret += (num3 - '0') * 10;
	if (num2 == '1') {
		ret = 100;
	}
	if (num1 == '-') {
		ret *= -1;
	}
	return ret;
}

void check(int num, const char *message) {
	if (num < 0) {
		error(message);
	}
}

void error(const char * message) {
	perror(message);
	exit(0);
}

void report(const char msg[]) {
	std::cout << msg << std::endl;
}
