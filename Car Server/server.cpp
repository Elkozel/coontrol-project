#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <netinet/in.h>

#define server_port 0000



void read_conn(int, int*);
int analize(char[], int, int*);
void check(int, const char*);
void error(const char *);

int main() {
	int new_socket_fd, socket_fd, n;
	int *car_fd = (int *) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE | PROT_EXEC | PROT_NONE, MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	*car_fd = 0;
	int pid;
	socklen_t client_length;
	struct sockaddr_in serv_addr, cli_addr;
	if (server_port < 2) {
		error("Invalid port provided!");
		exit(1);
	}
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	check(socket_fd, "Unable to open socket");
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(server_port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	n = bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	check(n, "Unable to bind with the socket");
	listen(socket_fd, 5);
	client_length = sizeof(cli_addr);
	while (true) {
		new_socket_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &client_length);
		check(new_socket_fd, "Unable to accept call");
		pid = fork();
		check(pid, "Unable to fork");
		if (pid == 0) {
			read_conn(new_socket_fd, car_fd);
			exit(0);
			error("Child forced to be killed!");
			kill(getpid(), SIGTERM);
		}
	}
	close(new_socket_fd);
	close(socket_fd);
	error("Broke the inf. while loop?!");
	return 0;
}

void read_conn(int socket, int *car_socket) {
	while (true) {
		int n;
		char buffer[256];
		bzero(buffer, 255);
		n = read(socket, buffer, 255);
		check(n, "Unable to read from socket");
		n = analize(buffer, socket, car_socket);
		if (n)
			break;
	}
}

int analize(char buffer[], int socket, int *car_socket) {
	int n;
	char loc = buffer[0];
	char com[2] = { buffer[1], buffer[2] };
	if (loc == 'C') {
		if (com[0] == 'O' && com[1] == 'N') {
			*car_socket = socket;
			n = write(*car_socket, "200 OK", sizeof("200 OK"));
			check(n, "Unable to write to Car!");
		}
		else if (com[0] == 'O' && com[1] == 'F') {
			n = write(*car_socket, "COF", 3);
			check(n, "Unable to write to server! (COF)");
		}
		else if (com[0] == 'S' && com[1] == 'T') {
			if (*car_socket <= 0) {
				n = write(socket, "Car not connected!", 18);
				check(n, "Unable to write to server! (Car not connected)");
			}
			else {
				n = write(socket, "CUP", 3);
				check(n, "Unable to write to server! (CUP)");
			}
		}
		else if (com[0] == 'E' && com[1] == 'D') {
			n = write(socket, "CED", 3);
			check(n, "Unable to write to socket! (CED)");
			close(socket);
			n = write(*car_socket, "CED", 3);
			check(n, "Unable to write to car! (CED)");
			if (n > 0)
				return 1;
		}
		else {
			n = write(socket, "ER_Server could not read the command!", 37);
			check(n, "Unable to write to client! (ER)");
		}
	}
	else if (loc == 'D') {
		n = write(*car_socket, buffer, strlen(buffer));
		check(n, "Could not write to car! (D)");
	}
	else {
		n = write(socket, "ER_Server could not read the locator!", 37);
		check(n, "Unable to write to client! (ER)");
	}

	return 0;
}

void check(int num, const char *message) {
	if (num < 0) {
		error(message);
	}
}

void error(const char *message) {
	perror(message);
}
