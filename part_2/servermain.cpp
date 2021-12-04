/*
** servermain.cpp -- a datagram socket server
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

using namespace std;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

// Port Allocation and Definitions

// Main Server -> UDP(with servers): 32504
#define MYPORT "32504"
// Backend Server A -> UDP: 30504
#define SERVER_A "30504"
// Backend Server B -> UDP: 31504
#define SERVER_B "31504"
// Max number of bytes we can get at once
#define MAXBUFLEN 1024

// Utility Functions
// get IPv4 sockaddr
void *get_in_addr(struct sockaddr *sa)
{
    return &(((struct sockaddr_in*)sa)->sin_addr);
}

// Main Function
int main(void)
{
    // Communication

    // sockfd: servermain socket descriptor
    int sockfd;

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
    // socklen_t sin_size;
    // struct sigaction sa;
    int yes=1;
    char s[INET_ADDRSTRLEN];
    int rv;

    // Populate hints to initial values (TCP)
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    // Populate servinfo using hints, ip and port
    // servinfo will be pointer to link list
    if ((rv = getaddrinfo("127.0.0.1", MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        
        // Create Socket
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Servermain: Socket creation failed!");
            continue;
        }

        // For Socket to use given port
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("Servermain: Couldnt force socket to desired port!");
            exit(1);
        }

        // Bind Socket to given ip address and port number
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("Servermain: Unable to bind socket to given ip and port!");
            continue;
        }

        break;
    }

    // Free servinfo struct, its work is done
    freeaddrinfo(servinfo);

    // None of the results from getaddrinfo could bind a socket
    if (p == NULL) {
        fprintf(stderr, "Servermain: failed to create and bind for any results\n");
        return 2;
    }

    // Initialising backend communucation sockets
    int NO_OF_SERVERS = 2;
    int BACKEND_SERVER_FDS[NO_OF_SERVERS];
    string BACKEND_SERVER_PORTS[NO_OF_SERVERS] = {SERVER_A, SERVER_B};

    // Creating backend sockets
    // for (int i = 0; i < NO_OF_SERVERS; i++)
    // {
    //     struct addrinfo hints, *servinfo, *p;
    //     int rv;
    //     int numbytes;

    //     memset(&hints, 0, sizeof hints);
    //     hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    //     hints.ai_socktype = SOCK_DGRAM;

    //     if ((rv = getaddrinfo("127.0.0.1", BACKEND_SERVER_PORTS[i].c_str(), &hints, &servinfo)) != 0) {
    //         fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    //         return 1;
    //     }

    //     // loop through all the results and make a socket
    //     for(p = servinfo; p != NULL; p = p->ai_next) {
    //         if ((BACKEND_SERVER_FDS[i] = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
    //             perror("Servermain: Backed communication socket creation failed!");
    //             continue;
    //         }

    //         break;
    //     }

    //     freeaddrinfo(servinfo);
        
    //     if (p == NULL) {
    //         fprintf(stderr, "Servermain: failed to create backend socket\n");
    //         return 2;
    //     }

    // }

    // struct addrinfo hints, *servinfo, *p;
    // int rv;
    // int numbytes;
    int servAFD;
    int servBFD;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo("127.0.0.1", SERVER_A, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((servAFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("Servermain: Backed communication socket creation failed!");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);
    
    if (p == NULL) {
        fprintf(stderr, "Servermain: failed to create backend socket\n");
        return 2;
    }

    // Display mainserver bootup message
    // Main server is up and running.
    cout << "Main server is up and running." << endl;

    // Main server contacts both the backend servers for metadata
    int numbytes;
    string query = "metadata";
    // for (int i = 0; i < NO_OF_SERVERS; i++)
    // {
    //     if ((numbytes = sendto(BACKEND_SERVER_FDS[i], &query[0], query.length(), 0, p->ai_addr, p->ai_addrlen)) == -1) {
    //         perror("Servermain: Couldnet ask backend server for metadata");
    //         exit(1);
    //     }
    // }

    if ((numbytes = sendto(servAFD, &query[0], query.length(), 0, p->ai_addr, p->ai_addrlen)) == -1) {
        perror("Servermain: Couldnet ask backend server for metadata");
        exit(1);
    }

    // Receive metadata from the respective server
    // int backendfd;
    // addr_len = sizeof their_addr;
    // if ((numbytes = recvfrom(serverfd, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
    //     perror("recvfrom");
    //     exit(1);
    // }
    // buf[numbytes] = '\0';

    // Main server has received the state list from server A using UDP over port <Main server UDP port number>

    // Store the metadata from backend servers into a map (state, server)
    // List the results of which states serverA/B is responsible for
    // Server A
    // <State Name 1>
    // <State Name 2>
    // 
    // Server B
    // <State Name 3>

    // Ask for user input for a state name
    // Enter state name:

    // If input state is not found in map print message 
    // <State Name> does not show up in server A&B
    // If input state is found in map retrive server id
    // <State Name> shows up in server <A or B>

    // Send statename to server whose server id has been found
    // The Main Server has sent request for <State Name> to server <A or B> using UDP over port <Main server UDP port number>

    // Receive the city names from the server where request has been posted
    // print in following format
    // The Main server has received searching result(s) of <State Name> from server<A or B>
    // There are <num> distinct cities in <State Name>: <city name1>, <city name2>...

    // Ask for a new query
    // -----Start a new query-----


}


