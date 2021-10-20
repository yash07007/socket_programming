/*
** servermain.cpp -- server serving city to state translation to multiple clients
*/

// Header Files

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Definations

// 33 + 504 (last 3 digit of id)
#define PORT "33504"
// connection queue 
#define BACKLOG 10  

// Function to kill zombie processes
void sigchld_handler(int s)
{
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

// get IPv4 sockaddr
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(void)
{
    // sockfd: server socket descriptor
    // new_fd: connected socket descriptor
    int sockfd, new_fd;

    // Intializing structs
    // hints: to initialize addrinfo struct
    // servinfo: complete addrinfo struct
    // their_addr: sockaddr_storage struct to store address information of incoming connection

    // struct addrinfo {
    //     int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
    //     int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
    //     int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
    //     int              ai_protocol;  // use 0 for "any"
    //     size_t           ai_addrlen;   // size of ai_addr in bytes
    //     struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
    //     char            *ai_canonname; // full canonical hostname
    //     struct addrinfo *ai_next;      // linked list, next node
    // };

    // struct sockaddr {
    //     unsigned short    sa_family;    // address family, AF_xxx
    //     char              sa_data[14];  // 14 bytes of protocol address
    // }; 

    // struct sockaddr_in {
    //     short int          sin_family;  // Address family, AF_INET
    //     unsigned short int sin_port;    // Port number
    //     struct in_addr     sin_addr;    // Internet address
    //     unsigned char      sin_zero[8]; // Same size as struct sockaddr
    // };

    // struct in_addr {
    //     uint32_t s_addr; // that's a 32-bit int (4 bytes)
    // };

    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;

    // Helper variables
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET_ADDRSTRLEN];
    int rv;

    // Populate hints to initial values (TCP)
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Populate servinfo using hints, ip and port
    // servinfo will be pointer to link list
    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        // Create Socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Server: Socket creation failed!");
            continue;
        }

        // For Socket to use given port
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("Server: Couldnt force socket to desired port!");
            exit(1);
        }

        // Bind Socket to given ip address and port number
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Server: Unable to bind socket to given ip and port!");
            continue;
        }

        break;
    }

    // Free servinfo struct, its work is done
    freeaddrinfo(servinfo); 

    // None of the results from getaddrinfo could bind a socket
    if (p == NULL)  {
        fprintf(stderr, "Server: failed to create and bind for any results\n");
        exit(1);
    }

    // Listen for connections
    if (listen(sockfd, BACKLOG) == -1) {
        perror("Server: Couldnt listen for connections");
        exit(1);
    }
    printf("Server: Waiting for connections...\n");


    // Reap all dead/zombie processes
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Wait Loop
    while(1) {

        // Accept incoming connection
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("Server: Couldnt accept incoming connection");
            continue;
        }

        // Converting ip address from binary to presentable format
        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("Server: Got connection from %s, port %d\n", s, ((struct sockaddr_in*)&their_addr)->sin_port);

        // Create fork for child process
        if (!fork()) { 
            // Child doesn't need the listener
            close(sockfd); 
            
            // Child handeling
            if (send(new_fd, "Hello, world!", 13, 0) == -1)
                perror("send");

            // Close Child
            close(new_fd);
            exit(0);
        }

        // Parent process doesnt need new_fd
        close(new_fd);

    }

    return 0;
}