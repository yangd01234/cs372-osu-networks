'''
Author: Derek Yang
Program Name: chatserve.py
Course: CS372
Description: chatserver waits on a port for a client request.
client sends the initial connection message.  Chat server
then receives the message and starts a connection.
Once the connection is made, both client and server can
alternate sending messages.
Most of the connection items were referenced from:
http://docs.python.org/release/2.6.5/library/internet.html,

Last Modified: 2/10/2019 at 4PM PT
'''

import socket
import sys

# initial handshake for testing purposes
def clientthread(conn, addr):
	conn.send("Welcome to this chatroom!")

'''
Receives messages from the client and closes connection if \quit is triggered.
Pre-condition:  Server is ready to receive information and 
                client sends out information to server.
Post-condition: Server receives information and sends
                new information back to client. 
'''
def receive_information(client_socket, server):
	while 1:
		try:
			receive = client_socket.recv(2048).decode()
			print(receive)
			if receive:
				receive_no_handle = receive.split("> ",1)[1]
				if receive_no_handle == "\quit":
					break
				to_send = input("Input message to send-> ")
				client_socket.send(bytes("SERVER_A> "+to_send, encoding='utf-8'))
			else:
				print("else triggered")
				remove(client_socket)
				break
		except:
			break

'''
Sends messages to the client.
Pre-condition:  Server is ready to send information and 
                client is ready to receive.
Post-condition: Server sends information and
                client receives information from server. 
'''
def send_message(client_socket):
	to_send = input("Input message to send-> ")
	client_socket.send(bytes("SERVER_A> " + to_send, encoding='utf-8'))


'''
Starts the server and binds the server at ip and port.
Pre-condition:  No other server is using port.
Post-condition: Creates server.
'''
def start_server(server):
# binds the server at the ip and port
	server.bind(('', Port))
	server.listen(2)

if __name__ == "__main__":
	# start the server
	print("Server started")
	if len(sys.argv) != 2:
		print("Please enter args: script, port number")
		print(sys.argv)

	Port = int(sys.argv[1])
	# AF_INET is the domain of the socket, SOCK_STREAM starts tcp stream
	server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	start_server(server)

	while 1:
		# start accepting connections
		client_socket, address = server.accept()
		receive_information(client_socket, server)
		client_socket.close()
		print("Closed socket")
