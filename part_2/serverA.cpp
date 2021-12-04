/*
** serverA.cpp -- a datagram socket backend server
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

// Backend Server A -> UDP: 30504
#define MYPORT "30504"
// Main Server -> UDP(with servers): 32504
#define MAIN_SERVER "32504"
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
    // COMPUTATION
    
    // Read the text file
    // initalising Maps (Dictionary)
    // Store state name and city names in map
    // Store no of distinct cities in a map
  	map<string, int> Set;
    map<string, string> Cities;
    map<string, int> DistinctCities;
    string states[20];

    // opening file in read mode
  	ifstream myfile("dataA.txt");

    if(myfile.is_open())
  	{
  	    string line,stateName;
  	    int isState = 1;
        int i = 1;

    	while(getline(myfile,line))
    	{
            // If state store it in state variable
      		if(isState == 1){
                stateName = line;
                // cout << stateName << endl;
                states[i++] = stateName;
                isState = 0;
      	    }
      	    else {
                size_t pos = 0;
                string city;
                string cities = "";
                int distinctCities = 0;
                while ((pos = line.find(',')) != string::npos) {
                    city = line.substr(0, pos);
                    // if location map do not have city
                    if(Set.find(city) == Set.end()) {
                        distinctCities += 1;
                        cities += city + ',';
                    }
                    Set.insert(make_pair(city, 1));
                    line.erase(0, pos + 1);
                }
                city = line;
                if(Set.find(city) == Set.end()) {
                    distinctCities += 1;
                        cities += city;
                }
                Set.insert(make_pair(city, 1));
                DistinctCities.insert(make_pair(stateName, distinctCities));
                Cities.insert(make_pair(stateName, cities));
                isState = 1;
      	    }
        }
        myfile.close();
		// cout << "\nServer A has read the state list from dataA.txt" << endl;
        // cout << "States: " << states;
    }
  	else {
		cout << "Unable to open list.txt file" << endl;
        exit(1); 
	}
    
    // Show bootup message
    // Server A is up and running using UDP on port <server A port number>
    cout << "Server A is up and running using UDP on port " << MYPORT << endl;

    // Communication

    // servAFD: Server A socket descriptor
    int servAFD;

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
        if ((servAFD = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("ServerA: Socket creation failed!");
            continue;
        }

        // For Socket to use given port
        if (setsockopt(servAFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("ServerA: Couldnt force socket to desired port!");
            exit(1);
        }

        // Bind Socket to given ip address and port number
        if (bind(servAFD, p->ai_addr, p->ai_addrlen) == -1) {
            close(servAFD);
            perror("ServerA: Unable to bind socket to given ip and port!");
            continue;
        }

        break;
    }

    // Free servinfo struct, its work is done
    freeaddrinfo(servinfo);

    // None of the results from getaddrinfo could bind a socket
    if (p == NULL) {
        fprintf(stderr, "ServerA: failed to create and bind for any results\n");
        return 2;
    }

    // SERVER SOCKET

    int serverfd;
    int numbytes;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo("127.0.0.1", MAIN_SERVER, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and make a socket
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((serverfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("ServerA: Communication Socket creation failed!");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);
    
    if (p == NULL) {
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }

    char buf[MAXBUFLEN];
    // Receive Request from main server for metadata
    socklen_t addr_len;
    addr_len = sizeof their_addr;
    if ((numbytes = recvfrom(servAFD, buf, MAXBUFLEN-1 , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1) {
        perror("recvfrom");
        exit(1);
    }
    buf[numbytes] = '\0';

    cout << buf << endl;

    
    // if(buf == "metadata") {
        
    //     // Send list of states and thier distinct no of cities
    //     // Server A has sent a state list to Main Server
    //     string states_str;
    //     int states_len = sizeof(states)/sizeof(states[0])
    //     for (int  i = 0; i < states_len; i++)
    //     {
    //         states_str += states[i];
    //         if(i != states_len - 1) {
    //             states_str += ', '
    //         }
    //     }

    //     if ((numbytes = sendto(serverfd, &states_str[0], states_str.length(), 0, p->ai_addr, p->ai_addrlen)) == -1) {
    //         perror("ServerA: Couldnt send meta data to the server");
    //         exit(1);
    //     }
        
    // }

}







// Receive statename from mainserver 
// Server A has received a request for <State Name>

// Retrieve city names from the map
// Server A found <num> distinct cities for <State Name>: <city name1>, <city name2>...

// Send the city names to requesting server
// Server A has sent the results to Main Server

