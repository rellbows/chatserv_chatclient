README for 'chatclient'/'chatserv'

Description:

Simple chat scripts that allow two hosts to send messages back and forth. Host running 'chatserv' and listens for a host runnning 'chatclient' to connect. Once connected, hosts can chat back and forth.

Compile Instructions:

'chatclient': Run 'gcc -o chatclient chatclient.c' in a machines command line.
'chatserv': N/A - python scripts do not need to be compiled first.

Execution Instructions:

'chatclient': Type './chatclient [host name] [host port number]' into the command line.
*Note: [host name] is the name of the server to connect to, and [host port number] is the port on that server.*

'chatserv': Type 'python3 chatserv.py [host port number]' into the command line.
*Note: [host port number] is the port on the machine running this server that will listen for connections.*

Usage Instructions:

'chatclient': 
1. User will be prompted to enter a 10 MAX handle to identify by.
2. Once handle is entered, prompt with '> ' will show up indicating the user can type a message to send to the server.
3. Once message is sent, the script will wait for reply messages from the server.
4. Repeat steps 2 and 3 as wanted.
5. To end connection and quit script, type '\quit' into the '> ' prompt.

'chatserv':
1. Server will wait for client to establish connection.
2. Once connection established, server will wait on incoming messages from client.
3. Once message from client is received, server can send message back to client.
4. Repeat steps 2 and 3 as wanted.
5. To end connect, type '\quit' into the '> ' prompt.
6. Server will continue to listen for incoming connections.
7. To end script, enter '⌘C' (command button + 'c' button).

Testing Details:
Scripts were tested on a MacBook Pro running macOS Sierra ver. 10.12.6.