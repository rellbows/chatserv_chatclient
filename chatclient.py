# Program Name: chatclient
# Description: Simple chat server.
# Author: Ryan Ellis
# Class: CS372 - Intro to Networks

from socket import *
import sys

def start_up(a_server_name, a_port_num):
	server_name = a_server_name
	server_port = a_port_num
	init_client_socket = socket(AF_INET, SOCK_STREAM)
	init_client_socket.connect((server_name, server_port))
	return init_client_socket

def get_handle():
	print('Hey there! Welcome to chatclient!')
	print('What handle do you want to use? *MAX LENGTH 10 CHARACTERS*')
	handle = input('>> ')
	return handle

def get_input(a_handle):
	message = input(a_handle + '> ')
	return message

def send_message(a_handle, a_message, a_socket):
	raw_handle_message = a_handle + '> ' + a_message
	handle_message = bytes(raw_handle_message, 'utf-8')
	a_socket.send(handle_message)

def receive_message(a_socket):
	raw_message = a_socket.recv(1024)
	message = raw_message.decode('utf-8')
	print(message)

def main():
	server_name = str(sys.argv[1])
	server_port = int(sys.argv[2])
	client_socket = start_up(server_name, server_port)
	handle = get_handle()
	raw_message = None

	while 1:
		if(raw_message == '\quit'):
			client_socket.close()
			break
		else:
			raw_message = get_input(handle)
			send_message(handle, raw_message, client_socket)
			state = receive_message(client_socket)

# main method
if __name__ == '__main__':
	main()	

	