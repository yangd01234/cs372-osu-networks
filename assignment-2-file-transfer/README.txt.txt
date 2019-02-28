Derek Yang
CS372 Programming Assignment#2

Description:
The following programs implement a simple file transfer system where the server is written in C and the 
client is written in Python 3.  The following events will occur:

1. ftserver starts on Host A, and validates command-line parameters
(<SERVER_PORT>).
2. ftserver waits on <PORTNUM> for a client request.
3. ftclient starts on Host B, and validates any pertinent command-line parameters.
(<SERVER_HOST>, <SERVER_PORT>, <COMMAND>, <FILENAME>,
<DATA_PORT>, etc…)
4. ftserver and ftclient establish a TCP control connection on <SERVER_PORT>. (For
the remainder of this description, call this connection P)
5. ftserver waits on connection P for ftclient to send a command.
6. ftclient sends a command (-l (list) or -g <FILENAME> (get)) on connection P.
7. ftserver receives command on connection P.
If ftclient sent an invalid command
• ftserver sends an error message to ftclient on connection P, and ftclient
displays the message on-screen.
otherwise
• ftserver initiates a TCP data connection with ftclient on <DATA_PORT>.
(Call this connection Q)
• If ftclient has sent the -l command, ftserver sends its directory to ftclient
on connection Q, and ftclient displays the directory on-screen.
• If ftclient has sent -g <FILENAME>, ftserver validates FILENAME, and
either
- sends the contents of FILENAME on connection Q. ftclient saves the
file in the current default directory (handling "duplicate file name" error
if necessary), and displays a "transfer complete" message on-screen
or
- sends an appropriate error message (“File not found”, etc.) to ftclient on
connection P, and ftclient displays the message on-screen.
• ftserver closes connection Q (don’t leave open sockets!).
8. ftclient closes connection P (don’t leave open sockets!) and terminates.
9. ftserver repeats from 2 (above) until terminated by a supervisor (SIGINT).

Files:
ftclient.py
ftserver.c

Requires:
python3 and c89

To compile chatserve.c use gcc chatclient.c -o chatclient

To run server and client:
First run ftserver.c using the command: python3 chatserve.py [port]
Then run ftclient.py using the command: ./chatclient [flipX.engr.oregonstate.edu] [port]

The code was tested using flip1.engr.oregonstate.edu