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

int forward, right, backward, left;
int analize(char[], int);
void report(const char[]);
int chartoint(char, char);
void controls(int, int, int, int);
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
			forward = 10;
			right = 10;
			backward = 10;
			left = 10;
			close(socket);
			return 1;
		}
	} 
	// UDP server's job
	else
		if (loc == 'D') {
			std::cout << "For now: " << std::endl << "F: " << forward << "R: " << right << "B: " << backward << "L: " << left;
			if (strlen(buf) == 13) {
				forward = chartoint(buf[2], buf[3]);
				right = chartoint(buf[5], buf[6]);
				backward = chartoint(buf[8], buf[9]);
				left = chartoint(buf[11], buf[12]);
			}
			else {
				int pos;
				for (int s = 0; s < (strlen (buf)-1) / 3; s++) {
					pos = (s * 3) + 1;
					std::cout << "Caught pos: " << buf[pos] << std::endl;
					switch (buf[pos]) {
					case 'F':
						forward = chartoint(buf[pos + 1], buf[pos + 2]); break;
					case 'R':
						right = chartoint(buf[pos + 1], buf[pos + 2]); break;
					case 'B':
						backward = chartoint(buf[pos + 1], buf[pos + 2]); break;
					case 'L':
						left = chartoint(buf[pos + 1], buf[pos + 2]); break;
					}
				}
			}
			controls(forward, right, backward, left);
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
void controls(int forward, int right, int backward, int left) {
	forward = (forward - 10) * 20;
	right = (right - 10) * 20;
	backward = (backward - 10) * 20;
	left = (left - 10) * 20;
	std::cout << "F: " << forward << std::endl << "R: " << right << std::endl << "B: " << backward << std::endl << "L: " << left << std::endl;
	if (forward != 0 && backward != 0) {
		if (forward > backward) {
			backward = 0;
		}
		else if (backward > forward) {
			forward = 0;
		}
		else {
			forward = 0;
			backward = 0;
		}
	}
	if (backward > 0) {
		digitalWrite(int1, LOW);
		digitalWrite(int2, HIGH);
		pwmWrite(ena, backward);
	}
	if (forward > 0) {
		digitalWrite(int1, HIGH);
		digitalWrite(int2, LOW);
		pwmWrite(ena, forward);
	}
	if (left != 0 && right != 0) {
		if (left > right) {
			right = 0;
		}
		else if (right > left) {
			left = 0;
		}
		else {
			left = 0;
			right = 0;
		}
	}
	if (left > 0) {
		digitalWrite(int3, HIGH);
		digitalWrite(int4, LOW);
		pwmWrite(enb, left);
	}
	if (right > 0) {
		digitalWrite(int3, LOW);
		digitalWrite(int4, HIGH);
		pwmWrite(enb, right);
	}
	if (left == 0) {
		digitalWrite(int3, LOW);
	}
	if (right == 0) {
		digitalWrite(int4, LOW);
	}
	if (forward == 0) {
		digitalWrite(int1, LOW);
	}
	if (backward == 0) {
		digitalWrite(int2, LOW);
	}
}

void wire_start() {
	wiringPiSetup();
	pinMode(int1, OUTPUT);
	pinMode(int2, OUTPUT);
	pinMode(int3, OUTPUT);
	pinMode(int4, OUTPUT);
	pinMode(ena, PWM_OUTPUT);
	pinMode(enb, PWM_OUTPUT);
	forward = 10;
	right = 10;
	backward = 10;
	left = 10;
}

int chartoint(char num1, char num2) {
	int ret = num2 - '0';
	ret += (num1 - '0') * 10;
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
