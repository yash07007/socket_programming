# Socket Programming

This repository is made for EE450 Introduction to Computer Networks at University of Southern California's Viterbi School of Engineering. It has C++ implementation of Stream Socket Server serving city to state translations.

<br/>

## Project Details

**Author:** Yash Solanki

**Student Id:** 4369335504

**Project Description:** This project implements a TCP server and TCP client using sockets. Clients quries server the city names for which server uses its own data base in memory to give appropirate state in with the city is. Description of structure of files is as follows

-   `servermain.cpp` : This file implements TCP server side which takes city names from clients and sends respective states stored in a mapping inside memory.
-   `client.cpp` : This file implements TCP client side which takes city name as input from server and quries server for state name and displays the response from server.
-   `MakeFile` : Implements commands to run all the files.
    -   `make all` : Complile all files.
    -   `make servermain` : Complie server file.
    -   `make client` : Complie client file.
    -   `make clean` : Delete all executeables.

<br/>

_This project is tested with all given testcases and it does not contian any idiosyncrasy._

_This code is 100% developed by the mentioned authour and no part of source is referenced/copied from other source._

_Beej reference guide is used to learn about the concept but the code has been written by authour_
