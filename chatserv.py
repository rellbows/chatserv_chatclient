# Program Name: chatserv
# Description: Simple chat server.
# Author: Ryan Ellis
# Class: CS372 - Intro to Networks
# Citations: Used sample code for Python TCP connection (slide 9/10) from
# Lecture 15 as reference. Also, used Python's documentation site.
# https://docs.python.org/2/library/socket.html

from socket import *
import sys

SERV_HANDLE = 'chatserv'

# Description: setups connection for server
# Argument: port number for server to run on
# Returns: a socket that is ready to receive incoming messages
def start_up(a_port_num):
	server_port = a_port_num
	server_name = gethostbyname(gethostname())
	init_server_socket = socket(AF_INET, SOCK_STREAM)
	init_server_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
	init_server_socket.bind((server_name, server_port))
	return init_server_socket

# Description: shuts down open connection
# Argument: socket
# Returns: nothing
def shut_down(a_connection_socket):
	a_connection_socket.close()

# Description: builds the message to be sent to client
# Arguments: raw message (no handle) and socket
# Returns: nothing
def send_message(a_message, a_connection_socket):
	raw_handle_message = SERV_HANDLE + '> ' + a_message
	handle_message = bytes(raw_handle_message, 'utf-8')
	a_connection_socket.send(handle_message)

# Description: receives messages from client
# Arguments: socket
# Returns: flag indicated if connection has been broken by client
def receive_message(a_connection_socket):
	raw_message = a_connection_socket.recv(501)
	if not raw_message:
		return 0
	else:
		message = raw_message.decode('utf-8')
		print(message)
		return 1

def main():
	port_num = int(sys.argv[1])
	server_socket = start_up(port_num)

	# holds reply message, initializing here so break in while loops will work
	raw_reply = '';

	while(1):
		server_socket.listen(1)
		connection_socket = None
		connection_flag = 1;
		print('Lisenting on port %s.' % port_num)
		while connection_flag:
			if(connection_socket == None):
				connection_socket, addr = server_socket.accept()
				print('Client has connected at %s' % addr[1])
			else:
				connection_flag = receive_message(connection_socket)
				if(connection_flag == 0):
					shut_down(connection_socket)
				else:
					raw_reply = input(SERV_HANDLE + '> ')
					if(raw_reply == '\quit'):
						shut_down(connection_socket)
						break
					else:
						send_message(raw_reply, connection_socket)
		if(raw_reply == '\quit'):
			break;

# main method
if __name__ == '__main__':
	main()	