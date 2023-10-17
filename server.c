#define MAX_EVENTS 5
#define READ_SIZE 1024
#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#define PORT 8080 
 
int init_epoll(int server_fd, struct sockaddr_in address, int addrlen);
int fd_set_blocking(int fd, int blocking);

int main(int argc, char const* argv[]) 
{ 
  // Creating socket file descriptor 
  // AF_INET for ipv4 if need replace with IF_INET6 for ipv6
  // SOCK_STREAM for tcp
  // 0 for ip protocol
  int server_fd;  
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
    perror("socket failed"); 
    exit(EXIT_FAILURE); 
  } 
  printf("server_fd: %d\n", server_fd);

  // Forcefully attaching socket to the port 8080 
  int opt = 1; 
  if (setsockopt(server_fd, SOL_SOCKET, 
        SO_REUSEADDR | SO_REUSEPORT, &opt, 
        sizeof(opt))) { 
    perror("setsockopt"); 
    exit(EXIT_FAILURE); 
  } 
  struct sockaddr_in address; 
  int addrlen = sizeof(address); 
  address.sin_family = AF_INET; 
  address.sin_port = htons(PORT); 
  address.sin_addr.s_addr = inet_addr("127.0.0.1"); 

  // Forcefully attaching socket to the port 8080 
  if (bind(server_fd, (struct sockaddr*)&address, 
      sizeof(address)) 
    < 0) { 
    perror("bind failed"); 
    exit(EXIT_FAILURE); 
  } 
  int backlog = 10;
  if (listen(server_fd, backlog) < 0) { 
    perror("listen"); 
    exit(EXIT_FAILURE); 
  } 
  init_epoll(server_fd, address, addrlen);

  //send(new_client, hello, strlen(hello), 0); 

  // closing the connected socket 
  //close(new_client); 
  // closing the listening socket 
  //shutdown(server_fd, SHUT_RDWR); 
  return 0; 
}

int init_epoll(int server_fd, struct sockaddr_in address, int addrlen) {
  int running = 1, event_count, i;
  size_t bytes_read;
  char read_buffer[READ_SIZE + 1];

  struct epoll_event event, events[MAX_EVENTS];
  int epoll_fd = epoll_create1(0);

  if (epoll_fd == -1) {
    fprintf(stderr, "Failed to create epoll file descriptor\n");
    return 1;
  }

  event.events = EPOLLIN;
  event.data.fd = server_fd;

  if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event)) {
    fprintf(stderr, "Failed to add file descriptor to epoll\n");
    close(epoll_fd);
    return 1;
  }

  while (running) {
    printf("\nPolling for input...\n");
    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
    printf("%d ready events\n", event_count);
    for (i = 0; i < event_count; i++) {
      if (events[i].data.fd == server_fd) {
	int new_client;
	if ((new_client = accept(server_fd, (struct sockaddr*) &address, (socklen_t*)&addrlen)) < 0) { 
	  perror("accept"); 
	  exit(EXIT_FAILURE); 
	} 
        printf("new_clinent_fd: %d\n", new_client);
	fd_set_blocking(new_client, 1);
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = new_client;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_client, &event) == -1) {
	  perror("epoll_ctl: conn_sock");
	  exit(EXIT_FAILURE);
	}
      } else {
        printf("Reading file descriptor '%d' -- ", events[i].data.fd);
        bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
        printf("%zd bytes read.\n", bytes_read);
        read_buffer[bytes_read] = '\0';
        printf("Read '%s'\n", read_buffer);
    
        if(!strncmp(read_buffer, "stop\n", 5))
        running = 0;
      }
    }
  }

  if (close(epoll_fd)) {
    fprintf(stderr, "Failed to close epoll file descriptor\n");
    return 1;
  }
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
