// Client side C/C++ program to demonstrate Socket 
// programming 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <iostream>
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <fcntl.h>
#include <signal.h>
#define PORT 8080 

int fd_set_blocking(int fd, int blocking);
void handle_siging(int sig);
int client_fd; 

int main(int argc, char const* argv[]) 
{ 
  if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
    printf("\n Socket creation error \n"); 
    return -1; 
  } 

  struct sockaddr_in serv_addr; 
  serv_addr.sin_family = AF_INET; 
  serv_addr.sin_port = htons(PORT); 

  // Convert IPv4 and IPv6 addresses from text to binary 
  // form 
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) 
    <= 0) { 
    printf( 
      "\nInvalid address/ Address not supported \n"); 
    return -1; 
  } 
  int status;
  if ((status 
    = connect(client_fd, (struct sockaddr*)&serv_addr, 
        sizeof(serv_addr))) 
    < 0) { 
    printf("\nConnection Failed \n"); 
    return -1; 
  } 

  char* hello = "Client connected message\0"; 
  char* get_message = "Get message\0";
  send(client_fd, hello, strlen(hello), 0); 
  printf("Connect message sent\n"); 
  signal(SIGINT, handle_siging);

  char buffer[1024] = { 0 }; 
  while (true) {
    int valread = read(client_fd, buffer, 1024);
    send(client_fd, get_message, strlen(get_message), 0);
    printf("%s", buffer); 
    printf("Do you want to answer y/n\n");
    char yn;
    scanf("%c", &yn);
    if (yn == 'y') {
	
      scanf("%s", buffer);
      send(client_fd, buffer, strlen(buffer), 0);
      continue;
    }
  }

  // closing the connected socket 
  close(client_fd); 
  return 0; 
}

int fd_set_blocking(int fd, int blocking) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    return 0;
  }
  if (blocking) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }
  return fcntl(fd, F_SETFL, flags) != -1;
}

void handle_siging(int sig) {
  char* disconnect = "disconnect me\0";
  send(client_fd, disconnect, strlen(disconnect), 0);
  shutdown(client_fd, SHUT_RDWR);
  exit(0);
}
