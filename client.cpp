/*
** client.cpp -- client asking for city to state translation
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

using namespace std;

// Server port client will be connecting to
#define PORT "33504" 
// Max number of bytes we can get at once
#define MAXDATASIZE 1024

// get IPv4 sockaddr
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

int main(int argc, char *argv[])
{
    // sockfd: client scoket descriptor
    int sockfd;

    // Intializing structs
    // hints: to initialize addrinfo struct
    // servinfo: complete addrinfo struct
    //

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

    // Helper variables
    int rv;
    char s[INET_ADDRSTRLEN];

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

    // Loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        // Create Socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Client: Socket creation failed!");
            continue;
        }

        // Connect to Server
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Client: Couldnt connect to server!");
            continue;
        }

        break;
    }

    // None of the results from getaddrinfo could bind a socket
    if (p == NULL) {
        fprintf(stderr, "Client: failed to create and bind for any result\n");
        return 2;
    }

    // Converting ip address from binary to presentable format
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    // printf("Client: Connecting to %s, Port %s\n", s, PORT);

    // Free servinfo struct, its work is done
    freeaddrinfo(servinfo); // all done with this structure

    cout << "Client is up and running." << endl;

    while(1) {

        string query;

        cout << "Enter City Name: ";
        getline(cin, query);

        if(send(sockfd, &query[0], query.length(), 0) != -1) {
            cout << "Client has sent city " << query << " to Main Server using TCP." << endl;
        }
        else {
            cout << "Client query failed!" << endl;
            exit(1);
        }

        // numbytes: max no of bytes that can be recieved at once
        // buf: buffer to store incoming data
        int numbytes;  
        char buf[MAXDATASIZE];

        if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) != -1) {
            buf[numbytes] = '\0';
            cout << buf << endl;
        }
        else {
            cout << "Recieve Failed" << endl;
            exit(1);
        }

        cout << "----Start a new query----" << endl;

    }

    // Close Socket
    close(sockfd);

    return 0;
}