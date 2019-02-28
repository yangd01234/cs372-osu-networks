Derek Yang
CS372 Programming Assignment#1

Description:
The following programs run a chat client and chat server where the client is written in C and the 
server is written in Python 3.  The following events will occur:
1.  The server starts
2.  The client starts and asks user for a handle
3.  The client then asks the user for the initial message to send to the server
4.  The client sends the initial message
5.  Server receives message
6.  Connection is established
7.  Client and Server can send messages between each other until one sends the command \quit

Files:
chatserve.py
chatclient.c

Requires:
python3 and c89

To compile chatserve.c use gcc chatclient.c -o chatclient

To run server and client:
First run chatserve.py using the command: python3 chatserve.py [port]
Then run chatclient.c using the command: ./chatclient [flipX.engr.oregonstate.edu] [port]

The code was tested using flip1.engr.oregonstate.edu