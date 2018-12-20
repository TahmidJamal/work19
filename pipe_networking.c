#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
	printf("Making WKP\n");
	mkfifo("WKP", 0644);
	printf("Client has to connect now\n");
	int to_server = open("WKP", O_RDONLY);
	char pid[HANDSHAKE_BUFFER_SIZE];
	read(to_server, pid, HANDSHAKE_BUFFER_SIZE);
	printf("Server knows client private pipe\n");
	*to_client = open(pid, O_WRONLY);
	printf("Now accessing client by said private pipe\n");
	write(*to_client, ACK, sizeof(ACK));
	printf("Waiting to get from client\n");
	char buff[HANDSHAKE_BUFFER_SIZE];
	read(to_server, buff, HANDSHAKE_BUFFER_SIZE);
	printf("Server sent [%s]\n", buff);
	printf("Removing WKP\n");
	remove("WKP");
	return to_server;
  }


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
	printf("Making client private pipe\n");
	char pid[HANDSHAKE_BUFFER_SIZE];
	sprintf(pid, "%d", getpid());
	mkfifo(pid, 0644);
	printf("Giving private pipe to server\n");
	*to_server = open("WKP", O_WRONLY);
	write(*to_server, pid, HANDSHAKE_BUFFER_SIZE);
	printf("Verifying server received said pipe\n");
	int message = open(pid, O_RDONLY);
	char buff[HANDSHAKE_BUFFER_SIZE];
	read(message, buff, HANDSHAKE_BUFFER_SIZE);
	printf("Got the message [%s]\n", buff);
	printf("Now writing to server\n");
	write(*to_server, ACK, sizeof(ACK));
	printf("Removing private now\n");
	remove(pid);
	return message;
}
